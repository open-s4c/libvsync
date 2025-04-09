-- Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
-- SPDX-License-Identifier: MIT

module BoogieTranslator where
import ASMModel
import Data.Char (toLower)
import Data.Maybe

strLower :: String -> String
strLower = map toLower

instrToBoogie :: Instr -> String
instrToBoogie (Ld b (Register r1) (Register r2)) = "call execute(ld(" ++ strLower (show b) ++ ", " ++ r1 ++ ", " ++ r2 ++ "));\n"
instrToBoogie (Ldx b (Register r1) (Register r2)) = "call execute(ldx(" ++ strLower (show b) ++ ", " ++ r1 ++ ", " ++ r2 ++ "));\n"
instrToBoogie (St b (Register r1) (Register r2)) = "call execute(st(" ++ strLower (show b) ++ ", " ++ r1 ++ ", " ++ r2 ++ "));\n"
instrToBoogie (Stx b (Register r1) (Register r2) (Register r3)) = "call execute(stx(" ++ strLower (show b) ++ ", " ++ r1 ++ ", " ++ r2 ++ ", " ++ r3 ++ "));\n"

instrToBoogie (Swp acq rel (Register r1) (Register r2) (Register r3)) = "call execute(swp(" ++ strLower (show acq) ++ ", " ++ strLower (show rel) ++ ", " ++ r1 ++ ", " ++ r2 ++ ", " ++ r3 ++ "));\n"
instrToBoogie (Cas acq rel (Register r1) (Register r2) (Register r3)) = "call execute(cas(" ++ strLower (show acq) ++ ", " ++ strLower (show rel) ++ ", " ++ r1 ++ ", " ++ r2 ++ ", " ++ r3 ++ "));\n"

instrToBoogie (Mvn (Register r1) (Register r2)) = "call execute(mvn(" ++ r1 ++ ", " ++ r2 ++ "));\n"
instrToBoogie (Neg (Register r1) (Register r2)) = "call execute(neg(" ++ r1 ++ ", " ++ r2 ++ "));\n"

instrToBoogie (Ldumax acq rel (Register r1) (Register r2) (Register r3)) = "call execute(ldumax(" ++ strLower (show acq) ++ ", " ++ strLower (show rel) ++ ", " ++ r1 ++ ", " ++ r2 ++ ", " ++ r3 ++ "));\n"
instrToBoogie (Ldclr acq rel (Register r1) (Register r2) (Register r3)) = "call execute(ldclr(" ++ strLower (show acq) ++ ", " ++ strLower (show rel) ++ ", " ++ r1 ++ ", " ++ r2 ++ ", " ++ r3 ++ "));\n"
instrToBoogie (Ldset acq rel (Register r1) (Register r2) (Register r3)) = "call execute(ldset(" ++ strLower (show acq) ++ ", " ++ strLower (show rel) ++ ", " ++ r1 ++ ", " ++ r2 ++ ", " ++ r3 ++ "));\n"
instrToBoogie (Ldeor acq rel (Register r1) (Register r2) (Register r3)) = "call execute(ldeor(" ++ strLower (show acq) ++ ", " ++ strLower (show rel) ++ ", " ++ r1 ++ ", " ++ r2 ++ ", " ++ r3 ++ "));\n"
instrToBoogie (Ldadd acq rel (Register r1) (Register r2) (Register r3)) = "call execute(ldadd(" ++ strLower (show acq) ++ ", " ++ strLower (show rel) ++ ", " ++ r1 ++ ", " ++ r2 ++ ", " ++ r3 ++ "));\n"

instrToBoogie (Stumax b (Register r1) (Register r2)) = "call execute(stumax(" ++ strLower (show b) ++ ", " ++ r1 ++ ", " ++ r2 ++ "));\n"
instrToBoogie (Stclr b (Register r1) (Register r2)) = "call execute(stclr(" ++ strLower (show b) ++ ", " ++ r1 ++ ", " ++ r2 ++ "));\n"
instrToBoogie (Stset b (Register r1) (Register r2)) = "call execute(stset(" ++ strLower (show b) ++ ", " ++ r1 ++ ", " ++ r2 ++ "));\n"
instrToBoogie (Steor b (Register r1) (Register r2)) = "call execute(steor(" ++ strLower (show b) ++ ", " ++ r1 ++ ", " ++ r2 ++ "));\n"
instrToBoogie (Stadd b (Register r1) (Register r2)) = "call execute(stadd(" ++ strLower (show b) ++ ", " ++ r1 ++ ", " ++ r2 ++ "));\n"

instrToBoogie (Add (Register r1) (Register r2) (Register r3)) = "call execute(add(" ++ r1 ++ ", " ++ r2 ++ ", " ++ r3 ++ "));\n"
instrToBoogie (Sub (Register r1) (Register r2) (Register r3)) = "call execute(sub(" ++ r1 ++ ", " ++ r2 ++ ", " ++ r3 ++ "));\n"
instrToBoogie (And (Register r1) (Register r2) (Register r3)) = "call execute(andd(" ++ r1 ++ ", " ++ r2 ++ ", " ++ r3 ++ "));\n"
instrToBoogie (Eor (Register r1) (Register r2) (Register r3)) = "call execute(eor(" ++ r1 ++ ", " ++ r2 ++ ", " ++ r3 ++ "));\n"
instrToBoogie (Orr (Register r1) (Register r2) (Register r3)) = "call execute(orr(" ++ r1 ++ ", " ++ r2 ++ ", " ++ r3 ++ "));\n"
instrToBoogie (Mov (Register r1) (Register r2)) = "call execute(mov(" ++ r1 ++ ", " ++ r2 ++ "));\n"
instrToBoogie (Cmp (Register r1) (Register r2)) = "call execute(cmp(" ++ r1 ++ ", " ++ r2 ++ "));\n"
instrToBoogie (Label s) = s ++ ":\n"
instrToBoogie (Ignored s) = "// " ++ s ++ "\n"
instrToBoogie (B cond label _) = "if (branch(" ++ show cond ++ "(), " ++ "states[step]" ++ ")) { goto ." ++ label ++ "; }\n"
instrToBoogie (Cb cond (Register r) label b) = "if (cb" ++ show cond ++ "(" ++ r ++ ", states[step]" ++ ")) { goto ." ++ label ++ "; }\n"
instrToBoogie i = "call execute(" ++ (strLower.show) i ++ "());\n"

boogieParameters :: [Register] -> String
boogieParameters lreg = init . init $ foldr (\(Register x) l -> l ++ x ++ ", ") "" lreg

boogieLocals :: [Register] -> String
boogieLocals = foldr (\(Register x) l -> l ++ "var " ++ x ++ ": Register;\n") ""

boogieMethodHeader :: String -> [Register] -> Maybe Register-> String
boogieMethodHeader name lreg ret = basicHeader name lreg ++ retHeader ret
    where 
        basicHeader name lreg
            | isGet name            = "implementation {:verboseName \"" ++ name ++ "\"} get_rmw(op : RMWOp, load_order, store_order: OrderRelation, " ++ boogieParameters lreg ++ " : Register)" 
            | isRMW name            = "implementation {:verboseName \"" ++ name ++ "\"} rmw(op : RMWOp, store_order: OrderRelation, " ++ boogieParameters lreg ++ " : Register)"
            | isCMPXCHG name        = "implementation {:verboseName \"" ++ name ++ "\"} cmpxchg(load_order, store_order: OrderRelation, " ++ boogieParameters lreg ++ " : Register)"
            | isRead name           = "implementation {:verboseName \"" ++ name ++ "\"} read(load_order: OrderRelation, "++ boogieParameters lreg ++ " : Register)"
            | isWrite name          = "implementation {:verboseName \"" ++ name ++ "\"} write(store_order: OrderRelation, "++ boogieParameters lreg ++ " : Register)"
            | isXCHG name           = "implementation {:verboseName \"" ++ name ++ "\"} xchg(load_order, store_order: OrderRelation, " ++ boogieParameters lreg ++ " : Register)"
            | otherwise             = "implementation {:verboseName \"" ++ name ++ "\"} await(op: AwaitOp, load_order: OrderRelation, " ++  boogieParameters lreg ++ " : Register)" -- Await
        retHeader Nothing = ""
        retHeader (Just (Register s)) = "returns (" ++ s ++ ": Register)" 
    

boogieCode :: Atomic -> String
boogieCode a =
    let (InstrSeq l) = instructions a
        procName     = name a in
          boogieLocals (locals a) ++ assumeOp procName ++ assumeCondition procName ++
          assumeDifferentRegisters a ++ do
            instr <- l
            "    " ++ instrToBoogie instr ++ addInvariant procName instr
        ++ generateConstInputAssert a
    where
        addInvariant name instr
            | isRMW name && checkLabel instr  = rmwInvariants a
            | isAwait name && checkLabel instr = awaitInvariants a
            | isXCHG name  && checkLabel instr = xchgInvariants a
            | otherwise                       =  ""
        assumeOp name
            | isRMW name        = "assume (op == " ++  extractOp name ++ ");\n"
            | isAwait name      = "assume (op == " ++ extractCondition name ++ ");\n"
            | otherwise         = ""
        generateConstInputAssert at = let inp = inputClobbers at 
                                      in do 
                                        (Register reg) <- inp
                                        "assert " ++ reg ++ " == old(" ++ reg ++ ");\n";

boogieAtomic :: Atomic -> String
boogieAtomic a = boogieMethodHeader (name a) (parameters a) (returns a) ++ "{\n" ++ boogieCode a ++ "}\n"

assumeDifferentRegisters :: Atomic -> String 
assumeDifferentRegisters atm = 
    let inp = inputClobbers atm
        outOv = outputNoOverlapClobbers atm
        out = outputClobbers atm
    in  let dif = [r1 ++ " != " ++ r2 ++ " && " | (Register r1) <- inp, (Register r2) <- outOv, r1 /= r2] 
               ++ [r1 ++ " != " ++ r2 ++ " && " | (Register r1) <- out, (Register r2) <- out, r1 /= r2]
        in case dif of 
            [] -> ""
            _  -> "assume " ++ (init.init.init.concat) dif ++ ";\n"
