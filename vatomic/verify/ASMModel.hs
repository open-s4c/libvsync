-- Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
-- SPDX-License-Identifier: MIT


module ASMModel where

import Data.Maybe (listToMaybe)
import Data.List
import Data.Tuple


newtype Register = Register String 
                    deriving (Show, Eq)

data Condition = EQ | NE | HS | LO | HI | LS deriving Show
data CompareCondition = Cb_Z | Cb_NZ
instance Show CompareCondition where
    show Cb_Z = "z"
    show Cb_NZ = "nz"


data Atomic = Atomic
    { name :: String
    , parameters :: [Register]
    , inputClobbers :: [Register]
    , outputClobbers :: [Register]
    , outputNoOverlapClobbers :: [Register]
    , locals :: [Register]
    , returns :: Maybe Register
    , instructions :: InstrSeq
    }

data Instr = Ld Bool Register Register
            | St  Bool Register Register
            | Ldx Bool Register Register
            | Stx Bool Register Register Register

            | Swp Bool Bool Register Register Register
            | Cas Bool Bool Register Register Register

            | Ldumax Bool Bool Register Register Register
            | Ldclr Bool Bool Register Register Register
            | Ldset Bool Bool Register Register Register
            | Ldeor Bool Bool Register Register Register 
            | Ldadd Bool Bool Register Register Register

            | Stumax Bool Register Register
            | Stclr Bool Register Register
            | Stset Bool Register Register
            | Steor Bool Register Register
            | Stadd Bool Register Register

            | Mvn Register Register
            | Neg Register Register

            | B  Condition String Char
            | Cb CompareCondition Register String Char
            | Cmp Register Register
            | Add Register Register Register
            | Sub Register Register Register
            | And Register Register Register
            | Eor Register Register Register
            | Orr Register Register Register
            | Mov Register Register
            | Wfe
            | Sevl
            | Label String
            | Ignored String
                deriving Show

newtype InstrSeq = InstrSeq [Instr] deriving Show

data Method = Method String [String] InstrSeq

paramTypes = ["vatomic64_t *", "vuint64_t", "vuint32_t", "const vatomic64_t *", "const vuint64_t", "const vuint32_t"]
cTypes = ["vuint64_t", "void", "void *", "vatomicptr_t *"]

rmwInvariants :: Atomic -> String
rmwInvariants atm = let params = [reg | (Register reg) <- parameters atm] in
                        let a = head params
                            v = (head.tail) params in 
                            "    assert step >= old(step);\n\
                            \    assert states[step]->gpr[" ++ a ++ "] == old(states[step]->gpr[" ++ a ++ "]);\n\
                            \    assert states[step]->gpr[" ++ v ++ "] == old(states[step]->gpr[" ++ v ++ "]);\n\
                            \    assert no_writes(old(step), step, effects);\n\n"
xchgInvariants :: Atomic -> [Char]
xchgInvariants atm = let params = [reg | (Register reg) <- parameters atm] in
                        let lst = if isCMPXCHG (name atm) then (head.tail.tail) params else (head.tail) params
                        in "assert states[step]->gpr[" ++ lst ++ "] == old(states[step]->gpr[" ++ lst ++ "]);\n"  ++ rmwInvariants atm           

awaitInvariants atm = let evReg = if hasWFE atm then "assert(event_register[step] || global_monitor_exclusive[step]);\n\n"
                                  else ""
                      in rmwInvariants atm ++ evReg 

readsWritesFunctions :: [String]
readsWritesFunctions = ["read", "read_acq", "read_rlx", "write", "write_rel", "write_rlx"]

awaitConditions :: [String]
awaitConditions = ["neq", "eq", "le", "lt", "ge", "gt"]

rmwFunctions :: [String]
rmwFunctions = getRMWFunctions ++ simpRMWFunctions

awaitFunctions :: [String]
awaitFunctions = do
    cond <- awaitConditions
    fence <- ["", "_acq", "_rlx"]
    return $ "await_" ++ cond ++ fence

rmwGetFences = ["", "_acq", "_rel", "_rlx"]
rmwSimpFences = ["", "_rel", "_rlx"]

modify :: [String]
modify = ["max", "and", "xor", "or", "add", "sub"]

getRMWFunctions :: [String]
getRMWFunctions = do
    op <- modify
    fence <- rmwGetFences
    return $ "get_" ++ op ++ fence

xchgFunctions :: [String]
xchgFunctions = do
    t <- ["xchg", "cmpxchg"]
    fence <- rmwGetFences
    return $ t ++ fence

simpRMWFunctions :: [String]
simpRMWFunctions = do
    op <- modify
    fence <- rmwSimpFences
    return $ op ++ fence

extractCondition :: String -> String
extractCondition a = let res = filter (`isInfixOf` a) awaitConditions in if null res then "" else head res

isRMW :: String -> Bool
isRMW = flip elem rmwFunctions

isRead :: String -> Bool 
isRead = isInfixOf "read"

isWrite :: String -> Bool 
isWrite = isInfixOf "write" 

isAwait :: String -> Bool
isAwait = flip elem awaitFunctions

isCMPXCHG :: String -> Bool
isCMPXCHG = isInfixOf "cmpxchg"

isXCHG :: String -> Bool
isXCHG = flip elem xchgFunctions

isGet :: String -> Bool
isGet = isInfixOf "get"

checkLabel :: Instr -> Bool
checkLabel (Label ".1") = True
checkLabel _ = False

ops :: [String]
ops = modify ++ ["xchg", "cmpxchg"]

extractOp :: String -> String
extractOp atomicName = let res = filter (`isInfixOf` atomicName) ops in if null res then "" else head res

assumeCondition :: String -> String
assumeCondition name
    | isGet name            = loadOrder ++ storeOrder
    | isRMW name            = storeOrder
    | isRead name           = loadOrder
    | isWrite name          = storeOrder
    | isAwait name          = loadOrder
    | otherwise             = loadOrder ++ storeOrder -- XCHG

        where 
        loadOrder   = let res = filter (`isInfixOf` name) ["_rel", "_acq", "_rlx"] in 
                        if null res then "assume load_order == order_acq_sc;\n" 
                        else let ord = head res in if ord /= "_rel" then "assume load_order == order" ++ ord ++ ";\n"
                                                   else "assume load_order == order_rlx;\n"

        storeOrder  = let res = filter (`isInfixOf` name) ["_rel", "_acq", "_rlx"] in 
                        if null res then "assume store_order == order_rel_sc;\n"
                        else let ord = head res in if ord /= "_acq" then "assume store_order == order" ++ head res ++ ";\n"
                                                   else "assume store_order == order_rlx;\n"

registerSubstitution :: [(Register, Register)] -> [Register] -> [Register]
registerSubstitution table = map (chg (map swap table))
    where chg substitutionTable currRegister = case lookup currRegister substitutionTable of
                        Just (Register substituteRegister) -> Register substituteRegister
                        Nothing           -> currRegister

isWFE :: Instr -> Bool
isWFE Wfe = True 
isWFE _   = False

hasWFE :: Atomic -> Bool
hasWFE atm = let (InstrSeq instr) = instructions atm 
             in foldr (\x currFoundWFE -> currFoundWFE || isWFE x ) False instr

-- 4 - vatomic_fence_(seq|acq|rel|rlx)
-- 3 - vatomic64_read_(seq|acq|rlx)
-- 3 - vatomic64_write_(seq|rel|rlx)
-- 32 = 8*4 - vatomic64_(xchg|cmpxchg|get_(max|and|or|xor|add|sub))_(seq|acq|rel|rlx)
-- 18 = 6*3 - vatomic64_(max|and|or|xor|add|sub)_(seq|rel|rlx)
-- 18 = 6*3 - vatomic64_await_(eq|neq|le|lt|ge|gt)_(seq|acq|rlx)
