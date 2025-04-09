-- Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
-- SPDX-License-Identifier: MIT

module ASMParsers where

import ASMModel
import Text.Parsec
import Text.Parsec.String ( Parser, Parser )
import Control.Monad (void)
import Data.Foldable (asum)
import Prelude hiding (EQ)

nextAsmParam :: Parser Register
nextAsmParam = do
    many (noneOf "[\n")
    char '['
    reg <- many (noneOf "]\n")
    char ']'
    return $ Register reg

ignoredParser :: Parser Instr
ignoredParser = do
    instr <- string "prfm" <|> string ".align"
    rest <- many (noneOf "\n")
    spaces
    return $ Ignored (instr ++ rest)

eventParser :: Parser Instr
eventParser = do
    instr <- (Sevl <$ try (string "sevl")) <|> (Wfe <$ try (string "wfe"))
    many (noneOf "\n")
    spaces
    return instr

labelParser :: Parser Instr
labelParser = do
    label <- many (noneOf ": \n")
    char ':'
    spaces
    return $ Label ('.' : label)


addParser :: Parser Instr
addParser = do
    string "add"
    reg1 <- nextAsmParam
    reg2 <- nextAsmParam
    reg3 <- nextAsmParam
    many (noneOf "\n")
    spaces
    return $ Add reg1 reg2 reg3

subParser :: Parser Instr
subParser = do
    string "sub"
    reg1 <- nextAsmParam
    reg2 <- nextAsmParam
    reg3 <- nextAsmParam
    many (noneOf "\n")
    spaces
    return $ Sub reg1 reg2 reg3

andParser :: Parser Instr
andParser = do
    string "and"
    reg1 <- nextAsmParam
    reg2 <- nextAsmParam
    reg3 <- nextAsmParam
    many (noneOf "\n")
    spaces
    return $ And reg1 reg2 reg3

eorParser :: Parser Instr
eorParser = do
    string "eor"
    reg1 <- nextAsmParam
    reg2 <- nextAsmParam
    reg3 <- nextAsmParam
    many (noneOf "\n")
    spaces
    return $ Eor reg1 reg2 reg3

orrParser :: Parser Instr
orrParser = do
    string "orr"
    reg1 <- nextAsmParam
    reg2 <- nextAsmParam
    reg3 <- nextAsmParam
    many (noneOf "\n")
    spaces
    return $ Orr reg1 reg2 reg3

cmpParser :: Parser Instr
cmpParser = do
    string "cmp"
    reg1 <- nextAsmParam
    reg2 <- nextAsmParam
    many (noneOf "\n")
    spaces
    return $ Cmp reg1 reg2

movParser :: Parser Instr
movParser = do
    string "mov"
    reg1 <- nextAsmParam
    reg2 <- nextAsmParam
    many (noneOf "\n")
    spaces
    return $ Mov reg1 reg2

parseCondition :: Parser Condition
parseCondition = EQ <$ try (string "eq")
             <|> NE <$ try (string "ne")
             <|> HS <$ try (string "hs")
             <|> LO <$ try (string "lo")
             <|> HI <$ try (string "hi")
             <|> LS <$ try (string "ls")

swpParser :: Parser Instr
swpParser = do
    string "swp"
    acq <- option False (char 'a' >> return True)
    rel <- option False (char 'l' >> return True)
    reg1 <- nextAsmParam
    reg2 <- nextAsmParam
    reg3 <- nextAsmParam
    many (noneOf "\n")
    spaces
    return $ Swp acq rel reg1 reg2 reg3

casParser :: Parser Instr
casParser = do
    string "cas"
    acq <- option False (char 'a' >> return True)
    rel <- option False (char 'l' >> return True)
    reg1 <- nextAsmParam
    reg2 <- nextAsmParam
    reg3 <- nextAsmParam
    many (noneOf "\n")
    spaces
    return $ Cas acq rel reg1 reg2 reg3

mvnParser :: Parser Instr 
mvnParser = do
    string "mvn"
    reg1 <- nextAsmParam
    reg2 <- nextAsmParam
    many (noneOf "\n")
    spaces

    return $ Mvn reg1 reg2

negParser :: Parser Instr 
negParser = do
    string "neg"
    reg1 <- nextAsmParam
    reg2 <- nextAsmParam
    many (noneOf "\n")
    spaces

    return $ Neg reg1 reg2

ldumaxParser :: Parser Instr
ldumaxParser = do
    string "ldumax"
    acq <- option False (char 'a' >> return True)
    rel <- option False (char 'l' >> return True) 
    reg1 <- nextAsmParam
    reg2 <- nextAsmParam
    reg3 <- nextAsmParam
    many (noneOf "\n")
    spaces
    return $ Ldumax acq rel reg1 reg2 reg3



ldclrParser :: Parser Instr
ldclrParser = do
    string "ldclr"
    acq <- option False (char 'a' >> return True)
    rel <- option False (char 'l' >> return True) 
    reg1 <- nextAsmParam
    reg2 <- nextAsmParam
    reg3 <- nextAsmParam
    many (noneOf "\n")
    spaces
    return $ Ldclr acq rel reg1 reg2 reg3

ldsetParser :: Parser Instr
ldsetParser = do
    string "ldset"
    acq <- option False (char 'a' >> return True)
    rel <- option False (char 'l' >> return True) 
    reg1 <- nextAsmParam
    reg2 <- nextAsmParam
    reg3 <- nextAsmParam
    many (noneOf "\n")
    spaces
    return $ Ldset acq rel reg1 reg2 reg3

ldeorParser :: Parser Instr
ldeorParser = do
    string "ldeor"
    acq <- option False (char 'a' >> return True)
    rel <- option False (char 'l' >> return True) 
    reg1 <- nextAsmParam
    reg2 <- nextAsmParam
    reg3 <- nextAsmParam
    many (noneOf "\n")
    spaces
    return $ Ldeor acq rel reg1 reg2 reg3

ldaddParser :: Parser Instr
ldaddParser = do
    string "ldadd"
    acq <- option False (char 'a' >> return True)
    rel <- option False (char 'l' >> return True) 
    reg1 <- nextAsmParam
    reg2 <- nextAsmParam
    reg3 <- nextAsmParam
    many (noneOf "\n")
    spaces
    return $ Ldadd acq rel reg1 reg2 reg3

stumaxParser :: Parser Instr
stumaxParser = do 
    string "stumax"
    rel <- option False (char 'l' >> return True)
    reg1 <- nextAsmParam
    reg2 <- nextAsmParam
    many (noneOf("\n"))
    spaces
    return $ Stumax rel reg1 reg2

stclrParser :: Parser Instr
stclrParser = do 
    string "stclr"
    rel <- option False (char 'l' >> return True)
    reg1 <- nextAsmParam
    reg2 <- nextAsmParam
    many (noneOf("\n"))
    spaces
    return $ Stclr rel reg1 reg2

stsetParser :: Parser Instr
stsetParser = do 
    string "stset"
    rel <- option False (char 'l' >> return True)
    reg1 <- nextAsmParam
    reg2 <- nextAsmParam
    many (noneOf("\n"))
    spaces
    return $ Stset rel reg1 reg2

steorParser :: Parser Instr
steorParser = do 
    string "steor"
    rel <- option False (char 'l' >> return True)
    reg1 <- nextAsmParam
    reg2 <- nextAsmParam
    many (noneOf("\n"))
    spaces
    return $ Steor rel reg1 reg2

staddParser :: Parser Instr
staddParser = do 
    string "stadd"
    rel <- option False (char 'l' >> return True)
    reg1 <- nextAsmParam
    reg2 <- nextAsmParam
    many (noneOf("\n"))
    spaces
    return $ Stadd rel reg1 reg2

branchParser :: Parser Instr
branchParser = do
    string "b."
    cond <- parseCondition
    spaces
    label <- many (noneOf "bf\n")
    direction <- oneOf "bf"
    spaces
    return $ B cond label direction

cbParser :: Parser Instr
cbParser =
    let parseCond = Cb_NZ <$ string "nz" <|> Cb_Z <$ string "z"
    in do
    string "cb"
    cond <- (parseCond)
    reg <- nextAsmParam
    many $ noneOf "1234567890"
    label <- many (noneOf "bf\n")
    direction <- oneOf "bf"
    spaces
    return $ Cb cond reg label direction

ldParser :: Parser Instr
ldParser = do
    string "ld"
    b <- option False (char 'a' >> return True)
    param1 <- nextAsmParam
    param2 <- nextAsmParam
    spaces
    return $ Ld b param1 param2

ldxParser :: Parser Instr
ldxParser = do
    string "ldx"
    param1 <- nextAsmParam
    param2 <- nextAsmParam
    spaces
    return $ Ldx False param1 param2

ldaxParser :: Parser Instr
ldaxParser = do
    string "ldax"
    param1 <- nextAsmParam
    param2 <- nextAsmParam
    spaces
    return $ Ldx True param1 param2

stParser :: Parser Instr
stParser = do
    string "st"
    b <- option False (char 'l' >> return True)
    param1 <- nextAsmParam
    param2 <- nextAsmParam
    spaces
    return $ St b param1 param2

stxParser :: Parser Instr
stxParser = do
    string "stx"
    param1 <- nextAsmParam
    param2 <- nextAsmParam
    param3 <- nextAsmParam
    spaces
    return $ Stx False param1 param2 param3

stlxParser :: Parser Instr
stlxParser = do
    string "stlx"
    param1 <- nextAsmParam
    param2 <- nextAsmParam
    param3 <- nextAsmParam
    spaces
    return $ Stx True param1 param2 param3

instrParser :: Parser Instr
instrParser = spaces *> (
            try ldumaxParser
           <|> try ldclrParser
           <|> try ldsetParser
           <|> try ldeorParser
           <|> try ldaddParser 

           <|> try stumaxParser
           <|> try stclrParser
           <|> try stsetParser
           <|> try steorParser
           <|> try staddParser

           <|> try stlxParser
           <|> try stxParser
           <|> try stParser
           <|> try ldaxParser
           <|> try ldxParser
           <|> try ldParser

           <|> try negParser
           <|> try mvnParser

           <|> try swpParser
           <|> try casParser

           <|> try andParser
           <|> try addParser
           <|> try subParser
           <|> try eorParser
           <|> try orrParser
           <|> try cmpParser
           <|> try movParser
           <|> try branchParser
           <|> try cbParser
           <|> try eventParser
           <|> try ignoredParser
           <|> try labelParser
        ) <?> "Unknown instruction: "

programParser :: Parser InstrSeq
programParser = InstrSeq <$> (many (instrParser <* spaces) <* eof)



