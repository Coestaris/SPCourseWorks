package course.work.sp.firstpass;

import course.work.sp.identifierstorage.AssumeRegisterStorage;
import course.work.sp.fileparser.Operand;
import course.work.sp.identifierstorage.Constant;
import course.work.sp.identifierstorage.IdentifierStore;
import course.work.sp.identifierstorage.Label;
import course.work.sp.identifierstorage.Segment;
import course.work.sp.tokenizer.Token;
import course.work.sp.tokenizer.TokenType;

import java.util.List;

public class Mnem {
    private static final int error = -1;
    private static final int next = 1;
    private static final int index = 0;

    public int mnem(Token instruction, List<Operand> operands, int number) {
        if (!(instruction.getType().equals(TokenType.Empty) || (instruction.getType().equals(TokenType.Unknown)))) {
            switch (instruction.getStringToken()) {
                case ("STC"):
                    return stc(operands);
                case ("PUSH"):
                    return push(operands);
                case ("MOV"):
                    return mov(operands);
                case ("SUB"):
                    return sub(operands);
                case ("SBB"):
                    return sub(operands);
                case ("MUL"):
                    return mul(operands);
                case ("XOR"):
                    return xor(operands);
                case ("OR"):
                    return or(operands);
                case ("BTR"):
                    return btr(operands);
                case ("ADC"):
                    return adc(operands);
                case ("DB"):
                    return dbDir(operands);
                case ("DD"):
                    return ddDir(operands);
                case ("DW"):
                    return dwDir(operands);
                case ("JMP"):
                    return jmp(operands, number);
                case ("JNC"):
                    return jnc(operands, number);
                case ("ASSUME"):
                    return assume(operands);
                case ("SEGMENT"):
                case ("ENDS"):
                case ("END"):
                    return 0;
                default:
                    break;
            }
        }
        return 0;
    }

    private int stc(List<Operand> operands) {
        int size = 1;
        if (operands.size() == 0)
            return size;
        return error;
    }

    private int push(List<Operand> operands) {
        int size = 1;
        if (operands.size() == 1) {
            if (operands.get(index).equalOperandType(TokenType.Reg32) || operands.get(index).equalOperandType(TokenType.Reg16)) {
                if (operands.get(index).equalOperandType(TokenType.Reg16)) size++;
                return size;
            }
        }
        return error;
    }

    private int mov(List<Operand> operands) {
        int size = 2;
        if (operands.size() == 2) {
            Operand operand = operands.get(index);
            if (operand.equalOperandType(operands.get(index + next).getOperandType())) {
                if (operand.equalOperandType(TokenType.Reg16)) return size + 1;
                return size;
            } else return error;

        }
        return error;
    }

    private int sub(List<Operand> operands) {
        int size = 3;
        if (operands.size() == 2) {
            if ((operands.get(index).equalOperandType(TokenType.Reg32) || operands.get(index).equalOperandType(TokenType.Reg16))
                    && operands.get(index + next).equalOperandType(TokenType.Imm)) {
                if (operands.get(index).equalOperandType(TokenType.Reg16)) size++;
                return size;
            }
        }

        return error;
    }

    private int mul(List<Operand> operands) {
        int size = 2;
        if (operands.size() == 1) {
            if (operands.get(index).equalOperandType(TokenType.Reg8) ||
                    operands.get(index).equalOperandType(TokenType.Reg16) ||
                    operands.get(index).equalOperandType(TokenType.Reg32)) {
                return size;
            }
            Operand operand = operands.get(index);
            if (operand.equalOperandType(TokenType.DwIdentifier) ||
                    operand.equalOperandType(TokenType.DdIdentifier) ||
                    operand.equalOperandType(TokenType.DbIdentifier)) return error;
            if (mem(operand) < 0) return error;
            return size + mem(operand);
        }
        return error;
    }

    private int xor(List<Operand> operands) {
        int size = 2;
        if (operands.size() == 2) {
            if (operands.get(index).equalOperandType(TokenType.Reg32) || operands.get(index).equalOperandType(TokenType.Reg16)) {
                if (operands.get(index).equalOperandType(TokenType.Reg16)) size++;
            } else return error;
            Operand operand = operands.get(index + next);
            if (mem(operand) < 0) return error;
            if (isTypeMemRegGood(operand, operands.get(index)))
                return size + mem(operand);
        }
        return error;
    }

    private int or(List<Operand> operands) {
        int size = 2;
        if (operands.size() == 2) {
            if (operands.get(index + next).equalOperandType(TokenType.Reg32) || operands.get(index + next).equalOperandType(TokenType.Reg16)) {
                if (operands.get(index + next).equalOperandType(TokenType.Reg16)) size++;
            } else return error;
            Operand operand = operands.get(index);
            if (mem(operand) < 0) return error;
            if (isTypeMemRegGood(operand, operands.get(index + next)))
                return size + mem(operand);
        }
        return error;
    }


    private int btr(List<Operand> operands) {
        int size = 2;
        if (operands.size() == 2) {
            Operand operand = operands.get(index);
            Operand operand2 = operands.get(index + next);
            if (mem(operand) < 0) return error;
            size += mem(operand);
            if (operand2.equalOperandType(TokenType.Reg32) || operand2.equalOperandType(TokenType.Reg16)) {
                if (operand2.equalOperandType(TokenType.Reg16)) size++;
            } else return error;
            if (isTypeMemRegGood(operand, operand2))
                return size;
        }
        return error;
    }

    private int adc(List<Operand> operands) {
        int size = 2;
        int number = index;
        if (operands.size() == 2) {
            Operand operand = operands.get(index);
            if (mem(operand) < 0) return error;
            size += mem(operand);
            if (operand.isSibRegIs16()) size--;
            operand = operands.get(index + next);
            if (imm(operand) < 0) return error;
            return size + imm(operand);
        }
        return error;
    }

    private int jmp(List<Operand> operands, int number) {
        int labelNumber = 0;
        int size = 1;
        boolean isLabel = false;
        if (operands.size() == 1) {
            if (operands.get(index).equalOperandType(TokenType.Identifier)) {
                for (Label lbl : IdentifierStore.getInstance().getLabelList()) {
                    isLabel = lbl.getLabel().equals(operands.get(index).getTokens().get(index).getStringToken());
                    if (isLabel) {
                        labelNumber = lbl.getIndex();
                        break;
                    }
                }
                if (isLabel) {
                    if (labelNumber > number) return size + 4;
                    else return size + 1;
                } else return error;
            }
        }
        return error;
    }

    private int jnc(List<Operand> operands, int number) {
        int labelNumber = 0;
        int size = 1;
        boolean isLabel = false;
        if (operands.size() == 1) {
            if (operands.get(index).equalOperandType(TokenType.Identifier)) {
                for (Label lbl : IdentifierStore.getInstance().getLabelList()) {
                    isLabel = lbl.getLabel().equals(operands.get(index).getTokens().get(index).getStringToken());
                    if (isLabel) {
                        labelNumber = lbl.getIndex();
                        break;
                    }
                }
                if (isLabel) {
                    if (labelNumber > number) return size + 5;
                    else return size + 1;
                } else return error;
            }
        }
        return error;
    }

    private int dbDir(List<Operand> operands) {
        int size = 1;
        if (operands.size() == 1) {
            if (operands.get(index).equalOperandType(TokenType.Imm) || operands.get(index).equalOperandType(TokenType.Text)) {
                if (operands.get(index).equalOperandType(TokenType.Text))
                    size *= operands.get(index).getTokens().get(index).stringToken.length();
                return size;
            }
        }
        return error;
    }

    private int ddDir(List<Operand> operands) {
        int size = 4;
        if (operands.size() == 1) {
            if (operands.get(index).equalOperandType(TokenType.Imm)) {
                return size;
            }
        }
        return error;
    }

    private int dwDir(List<Operand> operands) {
        int size = 2;
        if (operands.size() == 1) {
            if (operands.get(index).equalOperandType(TokenType.Imm)) {
                return size;
            }
        }
        return error;
    }

    private boolean isTypeMemRegGood(Operand mem, Operand reg) {
        return ((mem.equalOperandType(TokenType.DwIdentifier) ||
                mem.equalOperandType(TokenType.Mem) ||
                mem.equalOperandType(TokenType.Mem16)) && reg.equalOperandType(TokenType.Reg16)) ||
                ((mem.equalOperandType(TokenType.DdIdentifier) ||
                        mem.equalOperandType(TokenType.Mem) ||
                        mem.equalOperandType(TokenType.Mem16)) && reg.equalOperandType(TokenType.Reg32));
    }

    private int assume(List<Operand> operands) {
        boolean isError = true;
        for (Operand operand : operands) {
            isError = true;
            if (operand.getTokens().size() != 2 ||
                    !(operand.getTokens().get(index).equals(TokenType.SegmentRegister) && operand.getTokens().get(index + next).equals(TokenType.Identifier)))
                return error;
            for (Segment segment : IdentifierStore.getInstance().getSegmentList()) {
                if (segment.equalSegment(operand.getTokens().get(index + next).getStringToken())) {
                    AssumeRegisterStorage.getInstance().changeRegister(operand);
                    isError = false;
                }
            }
            if (isError) return error;
        }
        if (!isError) return 0;
        return error;
    }

    private int mem(Operand operand) {
        int size = 0;
        int segIndex = index;
        boolean isConstant = false;
        if (operand.equalOperandType(TokenType.Mem) ||
                operand.equalOperandType(TokenType.Mem8) ||
                operand.equalOperandType(TokenType.Mem16) ||
                operand.equalOperandType(TokenType.Mem32) ||
                operand.equalOperandType(TokenType.DbIdentifier) ||
                operand.equalOperandType(TokenType.DdIdentifier) ||
                operand.equalOperandType(TokenType.DwIdentifier)) {

            if (operand.isSegmentReg()) {
                if (!operand.getTokens().get(segIndex).getStringToken().equals("DS")) {
                    size++;
                }
                segIndex = index + next;

            }


            if (operand.equalOperandType(TokenType.DbIdentifier) ||
                    operand.equalOperandType(TokenType.DdIdentifier) ||
                    operand.equalOperandType(TokenType.DwIdentifier)) {
                for (Constant cst : IdentifierStore.getInstance().getConstantList()) {
                    isConstant = cst.getConstant().equals(operand.getTokens().get(segIndex).getStringToken());
                    if (isConstant) break;
                }
                if (isConstant) {
                    size += 4;
                    if (operand.isSibDisp()) {
                        size++;
                        if (operand.isSibRegIs16()) {
                            size--;
                        }
                    }
                } else return error;

            } else {
                if (operand.isSibDisp()) {
                    size += 2;
                    if (operand.isSibRegIs16()) size++;
                } else return error;
            }
            return size;

        }


        return error;
    }

    private int imm(Operand operand) {
        int size = 0;
        if (operand.equalOperandType(TokenType.Imm)) return size + 1;
        return error;
    }
}
