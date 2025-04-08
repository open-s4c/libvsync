-- Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
-- SPDX-License-Identifier: MIT

import Text.Parsec
import Text.Parsec.String (Parser)
import Control.Monad
import Data.Foldable (asum)
import Data.Maybe (catMaybes, listToMaybe, maybeToList)
import ASMModel
import ASMParsers (programParser)
import BoogieTranslator
import Data.List
import System.IO
import GHC.IO.Handle.Text
import System.IO (hPrint)
import System.Exit
import System.Environment

funcParser :: [String] -> Parser (String, [Register], [Register], [Register], [Register], [Register], Maybe Register, String)
funcParser retTypes = do
    string "static"
    skipMany1 space
    string "inline"
    skipMany1 space
    retType <- asum (map (try.string) retTypes)
    skipMany1 space
    -- currently parsing just vatomic64
    string "vatomic64_"
    name <- many (noneOf "(\n")
    char '('
    regs <- many $ parameterParser paramTypes
    spaces
    char '{'
    spaces
    locals <- many $ localParser paramTypes
    string "__asm__ volatile("
    code <- getCode
    spaces
    char ':'
    (outputOperands, outputNoOverlap) <- getOutputOperands
    spaces
    char ':'
    inputOperands <- getOperandList
    skipMany $ noneOf ";"
    char ';'
    spaces
    ret <- returnMaybeParser
    skipMany $ noneOf "}"
    char '}'
    spaces
    return (name
        ,   registerSubstitution (outputOperands ++ inputOperands) $ reverse regs
        ,   map fst inputOperands
        ,   map fst outputOperands
        ,   outputNoOverlap
        ,   (registerSubstitution (outputOperands ++ inputOperands) . reverse . deleteReg ret) locals
        ,   (listToMaybe . registerSubstitution (outputOperands ++ inputOperands) . maybeToList) ret
        ,   code
            )
deleteReg :: Maybe Register -> [Register] -> [Register]
deleteReg Nothing = id
deleteReg (Just x) = delete x

parseReturn :: Parser Register
parseReturn = do
    spaces
    string "return"
    spaces
    reg <- many $ noneOf ";"
    char ';'
    spaces
    return $ Register reg

returnMaybeParser :: Parser (Maybe Register)
returnMaybeParser = (Just <$> try parseReturn) <|> (spaces >> return Nothing)

getOperand :: Parser (Maybe (Register, Register))
getOperand = do
    spaces
    oneOf "[("
    spaces
    lf <- many $ noneOf " ])"
    spaces
    oneOf "])"
    spaces
    char '\"'
    op <- many $ noneOf "\""
    char '\"'
    spaces
    oneOf "[("
    spaces
    rt <- many $ noneOf " ])"
    spaces
    oneOf "])"
    spaces
    optional $ char ','
    spaces
    if 'Q' `elem` op then return $ Just (Register lf, Register lf) else return $ Just (Register lf, Register rt)

getOperandList :: Parser [(Register, Register)]
getOperandList = catMaybes <$> manyTill getOperand (try $ lookAhead colonAtNewLine)

getOutputOperand :: Parser (Maybe (Register, Register), Maybe Register)
getOutputOperand = do
    spaces
    oneOf "[("
    spaces
    lf <- many $ noneOf " ])"
    spaces
    oneOf "])"
    spaces
    char '\"'
    op <- many $ noneOf "\""
    char '\"'
    spaces
    oneOf "[("
    spaces
    rt <- many $ noneOf " ])"
    spaces
    oneOf "])"
    spaces
    optional $ char ','
    spaces
    return (if 'Q' `elem` op then Nothing else Just (Register lf, Register rt),
            if '&' `elem` op then Just $ Register lf              else Nothing
           )
getOutputOperands :: Parser ([(Register, Register)], [Register])
getOutputOperands = do
    x <- manyTill getOutputOperand (try $ lookAhead colonAtNewLine)
    return (catMaybes $ map fst x
          , catMaybes $ map snd x)

parameterParser :: [String] -> Parser Register
parameterParser types = do
    spaces
    pType <- asum (map (try.string) types)
    spaces
    name <- many (noneOf ",)\n")
    char ',' <|> char ')'
    spaces
    return $ Register name

localParser :: [String] -> Parser Register
localParser types = do
    spaces
    pType <- asum $ map (try.string) types
    spaces
    name <- many $ noneOf ";\n"
    char ';'
    spaces
    optional $ string "({"
    spaces
    return $ Register name

getCode :: Parser String
getCode = do
    spaces
    result <- manyTill parseQuotedSection (try (lookAhead colonAtNewLine))
    return (concat result)

parseQuotedSection :: Parser String
parseQuotedSection = do
    char '"'
    content <- many (noneOf "\"")
    char '"'
    spaces
    return content

colonAtNewLine :: Parser Char
colonAtNewLine = do
    spaces
    char ':'

skipLine :: Parser ()
skipLine = void (manyTill anyChar newline)

lineParser :: [String] -> Parser (Maybe (String, [Register], [Register], [Register], [Register], [Register], Maybe Register, String))
lineParser retTypes = (Just <$> try (funcParser retTypes)) <|> (skipLine >> return Nothing)

fileParser :: [String] -> Parser [(String, [Register], [Register], [Register], [Register], [Register], Maybe Register, String)]
fileParser retTypes = catMaybes <$> many (lineParser retTypes)

parseFile :: [String] -> String -> Either ParseError [(String, [Register], [Register], [Register], [Register], [Register], Maybe Register, String)]
parseFile retTypes = parse (fileParser retTypes) ""

toAtomic :: [(String, [Register], [Register], [Register], [Register], [Register], Maybe Register, String)] -> Either String [Atomic]
toAtomic = mapM toupleToAtomic
    where toupleToAtomic :: (String, [Register], [Register], [Register], [Register], [Register], Maybe Register, String) -> Either String Atomic
          toupleToAtomic tpl =
            let (aname, lreg, inputClobbers, outputClobbers, outputOvClobbers, llocal, ret, program) = tpl
            in case parse programParser "" program of
                Left err -> Left $ show err ++ "In atomic " ++ aname
                Right seq -> Right $ Atomic {name = aname
                                            , parameters = lreg
                                            , outputClobbers = outputClobbers
                                            , inputClobbers = inputClobbers
                                            , outputNoOverlapClobbers = outputOvClobbers
                                            , locals = llocal
                                            , returns = ret
                                            , instructions = seq }

pruneDoubleBSlash :: String -> String
pruneDoubleBSlash [] = []
pruneDoubleBSlash ('\\':'n':xs) = '\n' : pruneDoubleBSlash xs
pruneDoubleBSlash (x:xs) = x : pruneDoubleBSlash xs

checkJumps :: Atomic -> Either String Atomic
checkJumps a = let (InstrSeq instr) = instructions a in
                    let err = checkForward instr [] ++ checkBackward instr [] in
                        case err of
                        "" -> Right a
                        _   -> Left err
                where checkForward :: [Instr] -> [String] -> String
                      checkForward [] [] = ""
                      checkForward [] l = "Missused forward jumps: " ++ concatMap (++ ", ") l ++ "\n"
                      checkForward ((B _ label 'f') : t) l = checkForward t (label : l)
                      checkForward ((Cb _ _ label 'f') : t) l = checkForward t (label : l)
                      checkForward ((Label ('.' : s)) : t) l = checkForward t $ filter (/= s) l
                      checkForward (h : t) l = checkForward t l

                      checkBackward :: [Instr] -> [String] -> String
                      checkBackward [] _ = ""
                      checkBackward ((B _ label 'b') : t) l
                        | label `elem` l        = checkBackward t l
                        | otherwise             = "Missused backward jump [" ++ name a ++ "]: " ++ label ++ "\n" ++ checkBackward t l
                      checkBackward ((Cb _ _ label 'b') : t) l
                        | label `elem` l        = checkBackward t l
                        | otherwise             = "Missused backward jump [" ++ name a ++ "]: " ++ label ++ "\n" ++ checkBackward t l
                      checkBackward ((Label  ('.' : s)) : t) l = checkBackward t (s : l)
                      checkBackward (h : t) l = checkBackward t l
checkAtomic :: Atomic -> Either String (String, Atomic)
checkAtomic a = do
    r2 <- checkJumps a
    r3 <- getFile r2
    return (r3, r2)

knownAtomics = sort $ simpRMWFunctions ++ getRMWFunctions ++ xchgFunctions ++ awaitFunctions ++ readsWritesFunctions

checkIfAll :: [Atomic] -> Either String [Atomic]
checkIfAll l = let names = sort . map name $ l
               in if names == knownAtomics
                    then Right l
                  else Left "Error: Atomics don't match list of names\n"

getFile :: Atomic -> Either String String
getFile a
    | isRead  (name a) || isWrite  (name a)  = Right "reads_writes.bpl"
    | isAwait $ name a                       = Right "awaits.bpl"
    | isRMW $ name a                         = Right "rmws.bpl"
    | isXCHG $ name a                        = Right "xchgs.bpl"
    | otherwise                              = Left $ "Unknown atomic " ++ name a ++ "\n"


printInFiles :: [Atomic] -> IO()
printInFiles = foldr ((>>) . printer . checkAtomic) (return ())
    where printer :: Either String (String, Atomic) -> IO()
          printer h = case h of
                Left err -> exitWithErrorMessage (show err) (ExitFailure 2)
                Right (file, a) -> appendFile file (boogieAtomic a)

exitWithErrorMessage :: String -> ExitCode -> IO a
exitWithErrorMessage str e = hPutStrLn stderr str >> exitWith e

printAtomicList :: [Atomic] -> IO()
printAtomicList = foldr ((>>) . (\a -> appendFile "atomics_list.txt" (name a ++ "\n"))) (return ())

main :: IO ()
main = do
    args <- getArgs
    case args of
        (fileName:_) -> do
            putStrLn $ "fileName: " ++ fileName
            file <- readFile fileName
            writeFile "reads_writes.bpl" ""
            writeFile "awaits.bpl" ""
            writeFile "rmws.bpl" ""
            writeFile "xchgs.bpl" ""
            writeFile "atomics_list.txt" ""
            case parseFile cTypes $ pruneDoubleBSlash file of
                Left err -> exitWithErrorMessage (show err) (ExitFailure 2)
                Right result ->  let atomicList =  toAtomic result
                                in case atomicList of
                                    Left err -> exitWithErrorMessage (show err) (ExitFailure 2)
                                    Right atomicListNoError ->
                                        case checkIfAll atomicListNoError of
                                            Left err    -> exitWithErrorMessage (show err) (ExitFailure 2)
                                            Right lat   -> do
                                                        printInFiles lat
                                                        printAtomicList lat
        _ -> putStrLn "Insufficient arguments! expecting atomic file"
