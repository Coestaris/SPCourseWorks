package course.work.sp.secondpass;

import course.work.sp.fileparser.Operand;
import course.work.sp.firstpass.FirstPassSentence;
import course.work.sp.identifierstorage.AssumeRegisterStorage;
import course.work.sp.identifierstorage.Constant;
import course.work.sp.identifierstorage.IdentifierStore;
import course.work.sp.tokenizer.Token;
import course.work.sp.tokenizer.TokenType;

import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;

public class SetMachineCode {
    private static final int next = 1;
    private RegisterInfoStorage registerInfoStorage = RegisterInfoStorage.getInstance();

    public List<Byte> setMachineCode(FirstPassSentence firstPassSentence) {
        List<Byte> machineCode = new ArrayList<>();
        if (!(firstPassSentence.getNewSentence().getInstruction().getType().equals(TokenType.Empty) ||
                (firstPassSentence.getNewSentence().getInstruction().getType().equals(TokenType.Unknown)))) {
            switch (firstPassSentence.getNewSentence().getInstruction().getStringToken()) {
                case ("STC"):
                    return stc();
                case ("PUSH"):
                    return push(firstPassSentence.getNewSentence().getOperands());
                case ("MOV"):
                    return mov(firstPassSentence.getNewSentence().getOperands());
                case ("SUB"):
                    return sub(firstPassSentence.getNewSentence().getOperands());
                case ("SBB"):
                    return sbb(firstPassSentence.getNewSentence().getOperands());
                case ("MUL"):
                    return mul(firstPassSentence.getNewSentence().getOperands());
                case ("XOR"):
                    return xor(firstPassSentence.getNewSentence().getOperands());
                case ("OR"):
                    return or(firstPassSentence.getNewSentence().getOperands());
                case ("BTR"):
                    return btr(firstPassSentence.getNewSentence().getOperands());
                case ("ADC"):
                    return adc(firstPassSentence.getNewSentence().getOperands());
                case ("DB"):
                    return db(firstPassSentence.getNewSentence().getOperands());
                case ("DD"):
                    return dd(firstPassSentence.getNewSentence().getOperands());
                case ("DW"):
                    return dw(firstPassSentence.getNewSentence().getOperands());
                case ("JMP"):
                    return jmp(firstPassSentence);
                case ("JNC"):
                    return jnc(firstPassSentence);
                default:
                    break;
            }
        }
        return machineCode;
    }

    private List<Byte> stc() {
        List<Byte> machineCode = new ArrayList<>();
        machineCode.add((byte) 0xF9);
        return machineCode;
    }

    private List<Byte> push(List<Operand> operands) {
        int indexOperand = 0;
        int indexToken = 0;
        List<Byte> machineCode = new ArrayList<>();
        if (operands.get(indexOperand).equalOperandType(TokenType.Reg16)) machineCode.add((byte) 0x66);
        machineCode.add((byte) (0x50 + registerInfoStorage.getByteReg(operands.get(indexOperand).getStringTokenByIndex(indexToken))));
        return machineCode;
    }

    private List<Byte> mov(List<Operand> operands) {
        int indexOperand = 0;
        int indexToken = 0;
        List<Byte> machineCode = new ArrayList<>();
        if (operands.get(indexOperand).equalOperandType(TokenType.Reg16)) machineCode.add((byte) 0x66);
        machineCode.add((byte) 0x8b);
        machineCode.add((byte) (0b11000000 ^
                (registerInfoStorage.getByteReg(operands.get(indexOperand).getStringTokenByIndex(indexToken)) << 3) ^
                (registerInfoStorage.getByteReg(operands.get(indexOperand + next).getStringTokenByIndex(indexToken)))));
        return machineCode;
    }

    private List<Byte> mul(List<Operand> operands) {
        int indexOperand = 0;
        int indexToken = 0;
        List<Byte> machineCode = new ArrayList<>();
        if (operands.get(indexOperand).equalOperandType(TokenType.Reg8) ||
                operands.get(indexOperand).equalOperandType(TokenType.Reg16) ||
                operands.get(indexOperand).equalOperandType(TokenType.Reg32)) {
            if (operands.get(indexOperand).equalOperandType(TokenType.Reg8)) {
                machineCode.add((byte) 0xF6);
                machineCode.add((byte) (0b11100000 ^ registerInfoStorage.getByteReg(operands.get(indexOperand).getStringTokenByIndex(indexToken))));
            } else {
                if (operands.get(indexOperand).equalOperandType(TokenType.Reg16)) machineCode.add((byte) 0x66);
                machineCode.add((byte) 0xF7);
                machineCode.add((byte) (0b11100000 ^ registerInfoStorage.getByteReg(operands.get(indexOperand).getStringTokenByIndex(indexToken))));
            }

            return machineCode;
        }
        List<Token> operandTokens = operands.get(indexOperand).getTokens();
        if (operands.get(indexOperand).equalOperandType(TokenType.Mem16)) machineCode.add((byte) 0x66);
        if (operands.get(indexOperand).isSegmentReg()) {
            if (!operandTokens.get(indexToken).getStringToken().equals("DS"))
                machineCode.add(AssumeRegisterStorage.getInstance().getSegmentRegisterByte(operandTokens.get(indexToken).getStringToken()));
            indexToken++;
        }
        if (operands.get(indexOperand).isSibRegIs16()) {
            indexToken++;
            machineCode.add((byte) 0x67);//prefix
            if (operands.get(indexOperand).equalOperandType(TokenType.Mem8)) machineCode.add((byte) 0xF6);//opcode
            else machineCode.add((byte) 0xF7);//opcode

            if (operandTokens.get(indexToken).getStringToken().equals("BX")) {//modrm
                if (operandTokens.get(indexToken + next).getStringToken().equals("SI"))
                    machineCode.add((byte) 0b01100000);
                else machineCode.add((byte) 0b01100001);
            } else {
                if (operandTokens.get(indexToken + next).getStringToken().equals("SI"))
                    machineCode.add((byte) 0b01100010);
                else machineCode.add((byte) 0b01100011);
            }
            indexToken += 2;
            machineCode.add((byte) Integer.parseInt(operandTokens.get(indexToken).getStringToken()));//Imm
        } else {
            indexToken++;
            if (operands.get(indexOperand).equalOperandType(TokenType.Mem8)) machineCode.add((byte) 0xF6);//opcode
            else machineCode.add((byte) 0xF7);//opcode

            machineCode.add((byte) 0b01100100);//modrm
            machineCode.add((byte) (registerInfoStorage.getByteReg(operands.get(indexOperand).getStringTokenByIndex(indexToken)) ^
                    (registerInfoStorage.getByteReg(operands.get(indexOperand).getStringTokenByIndex(indexToken + next)) << 3)));//SIB

            indexToken += 2;
            machineCode.add((byte) Integer.parseInt(operandTokens.get(indexToken).getStringToken()));//Imm
        }


        return machineCode;
    }

    private List<Byte> xor(List<Operand> operands) {
        int indexToken = 0;
        int indexOperand = 0;
        Operand operand1 = operands.get(indexOperand);
        Operand mem = operands.get(indexOperand + next);
        List<Byte> machineCode = new ArrayList<>();
        List<Byte> opcode = new ArrayList<>();
        opcode.add((byte) 0x33);
        byte reg = RegisterInfoStorage.getInstance().getByteReg(operand1.getStringTokenByIndex(indexToken));
        if (operand1.equalOperandType(TokenType.Reg16)) machineCode.add((byte) 0x66);
        machineCode.addAll(mem(mem, opcode, reg, (byte) 0b10000000));
        return machineCode;
    }

    private List<Byte> or(List<Operand> operands) {
        int indexToken = 0;
        int indexOperand = 0;
        Operand operand1 = operands.get(indexOperand + next);
        Operand mem = operands.get(indexOperand);
        List<Byte> machineCode = new ArrayList<>();
        List<Byte> opcode = new ArrayList<>();
        opcode.add((byte) 0x09);
        byte reg = RegisterInfoStorage.getInstance().getByteReg(operand1.getStringTokenByIndex(indexToken));
        if (operand1.equalOperandType(TokenType.Reg16)) machineCode.add((byte) 0x66);
        machineCode.addAll(mem(mem, opcode, reg, (byte) 0b01000000));
        return machineCode;
    }

    private List<Byte> btr(List<Operand> operands) {
        int indexToken = 0;
        int indexOperand = 0;
        Operand mem = operands.get(indexOperand);
        Operand operand1 = operands.get(indexOperand + next);
        List<Byte> machineCode = new ArrayList<>();
        List<Byte> opcode = new ArrayList<>();
        opcode.add((byte) 0x0f);
        opcode.add((byte) 0xb3);
        byte mod = (byte) 0b10000000;
        byte reg = RegisterInfoStorage.getInstance().getByteReg(operand1.getStringTokenByIndex(indexToken));
        if (operand1.equalOperandType(TokenType.Reg16)) machineCode.add((byte) 0x66);
        machineCode.addAll(mem(mem, opcode, reg, mod));
        return machineCode;
    }

    private List<Byte> adc(List<Operand> operands) {
        int indexToken = 0;
        int indexOperand = 0;
        Operand mem = operands.get(indexOperand);
        Operand operand1 = operands.get(indexOperand + next);
        List<Byte> opcode = new ArrayList<>();
        if (mem.equalOperandType(TokenType.Mem8) || mem.equalOperandType(TokenType.DbIdentifier))
            opcode.add((byte) 0x80);
        else opcode.add((byte) 0x83);

        byte mod;
        if (mem.equalOperandType(TokenType.Mem8)) mod = (byte) 0b01000000;
        else mod = (byte) 0b10000000;
        byte reg = (byte) 0b010;
        List<Byte> machineCode = new ArrayList<>(mem(mem, opcode, reg, mod));
        machineCode.add((byte) Integer.parseInt(operand1.getStringTokenByIndex(indexToken)));
        return machineCode;
    }

    private List<Byte> sub(List<Operand> operands) {
        int indexOperand = 0;
        int indexToken = 0;
        List<Byte> machineCode = new ArrayList<>();
        if (operands.get(indexOperand).equalOperandType(TokenType.Reg16)) machineCode.add((byte) 0x66);
        machineCode.add((byte) 0x83);
        machineCode.add((byte) (0b11101000 ^ RegisterInfoStorage.getInstance().getByteReg(operands.get(indexOperand).getStringTokenByIndex(indexToken))));
        machineCode.add((byte) Integer.parseInt(operands.get(indexOperand + next).getStringTokenByIndex(indexToken)));
        return machineCode;
    }

    private List<Byte> sbb(List<Operand> operands) {
        int indexOperand = 0;
        int indexToken = 0;
        List<Byte> machineCode = new ArrayList<>();
        if (operands.get(indexOperand).equalOperandType(TokenType.Reg16)) machineCode.add((byte) 0x66);
        machineCode.add((byte) 0x83);
        machineCode.add((byte) (0b11011000 ^ RegisterInfoStorage.getInstance().getByteReg(operands.get(indexOperand).getStringTokenByIndex(indexToken))));
        machineCode.add((byte) Integer.parseInt(operands.get(indexOperand + next).getStringTokenByIndex(indexToken)));
        return machineCode;
    }

    private List<Byte> db(List<Operand> operands) {
        int indexOperand = 0;
        int indexToken = 0;
        List<Byte> machineCode = new ArrayList<>();
        if (operands.get(indexOperand).equalOperandType(TokenType.Imm)) {
            if (operands.get(indexOperand).getTokens().get(indexToken).equals(TokenType.DecNumber))
                machineCode.add((byte) Integer.parseInt(operands.get(indexOperand).getStringTokenByIndex(indexToken)));
            if (operands.get(indexOperand).getTokens().get(indexToken).equals(TokenType.HexNumber)) {
                String hexNumber = operands.get(indexOperand).getStringTokenByIndex(indexToken);
                machineCode.add((byte) Integer.parseInt(hexNumber.substring(0, hexNumber.length() - 1), 16));
            }
            if (operands.get(indexOperand).getTokens().get(indexToken).equals(TokenType.BinNumber)) {
                String binNumber = operands.get(indexOperand).getStringTokenByIndex(indexToken);
                machineCode.add((byte) Integer.parseInt(binNumber.substring(0, binNumber.length() - 1), 2));
            }
        } else {
            machineCode.addAll(toHexadecimal(operands.get(indexOperand).getStringTokenByIndex(indexToken)));
        }
        return machineCode;
    }

    private List<Byte> dd(List<Operand> operands) {
        int indexOperand = 0;
        int indexToken = 0;
        List<Byte> machineCode = new ArrayList<>();
        machineCode.add((byte) 0x0);
        machineCode.add((byte) 0x0);
        machineCode.add((byte) 0x0);
        if (operands.get(indexOperand).getTokens().get(indexToken).equals(TokenType.DecNumber))
            machineCode.add((byte) Integer.parseInt(operands.get(indexOperand).getStringTokenByIndex(indexToken)));
        if (operands.get(indexOperand).getTokens().get(indexToken).equals(TokenType.HexNumber)) {
            String hexNumber = operands.get(indexOperand).getStringTokenByIndex(indexToken);
            machineCode.add((byte) Integer.parseInt(hexNumber.substring(0, hexNumber.length() - 1), 16));
        }
        if (operands.get(indexOperand).getTokens().get(indexToken).equals(TokenType.BinNumber)) {
            String binNumber = operands.get(indexOperand).getStringTokenByIndex(indexToken);
            machineCode.add((byte) Integer.parseInt(binNumber.substring(0, binNumber.length() - 1), 2));
        }
        return machineCode;
    }

    private List<Byte> dw(List<Operand> operands) {
        int indexOperand = 0;
        int indexToken = 0;
        List<Byte> machineCode = new ArrayList<>();
        machineCode.add((byte) 0x0);
        if (operands.get(indexOperand).getTokens().get(indexToken).equals(TokenType.DecNumber))
            machineCode.add((byte) Integer.parseInt(operands.get(indexOperand).getStringTokenByIndex(indexToken)));
        if (operands.get(indexOperand).getTokens().get(indexToken).equals(TokenType.HexNumber)) {
            String hexNumber = operands.get(indexOperand).getStringTokenByIndex(indexToken);
            machineCode.add((byte) Integer.parseInt(hexNumber.substring(0, hexNumber.length() - 1), 16));
        }
        if (operands.get(indexOperand).getTokens().get(indexToken).equals(TokenType.BinNumber)) {
            String binNumber = operands.get(indexOperand).getStringTokenByIndex(indexToken);
            machineCode.add((byte) Integer.parseInt(binNumber.substring(0, binNumber.length() - 1), 2));
        }
        return machineCode;
    }

    private List<Byte> jmp(FirstPassSentence firstPassSentence) {
        int indexOperand = 0;
        int indexToken = 0;
        List<Byte> machineCode = new ArrayList<>();
        machineCode.add((byte) 0xEB);
        if (firstPassSentence.getSize() == 5) {
            machineCode.add((byte) (
                    IdentifierStore.getInstance().getLabelOffsetByString(
                            firstPassSentence.getNewSentence().getOperands().get(indexOperand).getTokens().get(indexToken).getStringToken()) -
                            firstPassSentence.getOffset() - 2));
            machineCode.add((byte) 0x90);
            machineCode.add((byte) 0x90);
            machineCode.add((byte) 0x90);
        } else {
            machineCode.add((byte) (
                    IdentifierStore.getInstance().getLabelOffsetByString(
                            firstPassSentence.getNewSentence().getOperands().get(indexOperand).getTokens().get(indexToken).getStringToken()) -
                            firstPassSentence.getOffset() - 2));
        }
        return machineCode;
    }

    private List<Byte> jnc(FirstPassSentence firstPassSentence) {
        int indexOperand = 0;
        int indexToken = 0;
        List<Byte> machineCode = new ArrayList<>();
        machineCode.add((byte) 0x73);
        if (firstPassSentence.getSize() == 6) {
            machineCode.add((byte) (
                    IdentifierStore.getInstance().getLabelOffsetByString(
                            firstPassSentence.getNewSentence().getOperands().get(indexOperand).getTokens().get(indexToken).getStringToken()) -
                            firstPassSentence.getOffset() - 2));
            machineCode.add((byte) 0x90);
            machineCode.add((byte) 0x90);
            machineCode.add((byte) 0x90);
            machineCode.add((byte) 0x90);
        } else {
            machineCode.add((byte) (
                    IdentifierStore.getInstance().getLabelOffsetByString(
                            firstPassSentence.getNewSentence().getOperands().get(indexOperand).getTokens().get(indexToken).getStringToken()) -
                            firstPassSentence.getOffset() - 2));
        }
        return machineCode;
    }

    private List<Byte> toHexadecimal(String text) {
        List<Byte> machineCode = new ArrayList<>();
        byte[] myBytes = text.getBytes(StandardCharsets.UTF_8);
        for (byte myByte : myBytes) {
            machineCode.add((byte) (myByte + 0x20));
        }
        return machineCode;
    }

    private List<Byte> mem(Operand mem, List<Byte> opCode, byte reg, byte mod) {
        int indexToken = 0;
        boolean isIdentifier = false;
        byte offset = 0;
        List<Token> operandTokens = mem.getTokens();
        List<Byte> machineCode = new ArrayList<>();
        if (mem.isSegmentReg()) {
            if (!operandTokens.get(indexToken).getStringToken().equals("DS"))
                machineCode.add(AssumeRegisterStorage.getInstance().getSegmentRegisterByte(operandTokens.get(indexToken).getStringToken()));
            indexToken++;
        }
        if (mem.equalOperandType(TokenType.DwIdentifier) ||
                mem.equalOperandType(TokenType.DdIdentifier) ||
                mem.equalOperandType(TokenType.DbIdentifier)) {
            isIdentifier = true;
            for (Constant constant : IdentifierStore.getInstance().getConstantList()) {
                if (mem.getStringTokenByIndex(indexToken).equals(constant.getConstant()))
                    offset = (byte) constant.getOffset();
            }
        }
        if (mem.isSibRegIs16()) {
            indexToken++;
            if(mem.equalOperandType(TokenType.Mem)) indexToken--;
            machineCode.add((byte) 0x67);//prefix
            machineCode.addAll(opCode);//opcode
            if (operandTokens.get(indexToken).getStringToken().equals("BX")) {//modrm
                if (operandTokens.get(indexToken + next).getStringToken().equals("SI"))
                    machineCode.add((byte) (0b00000000 ^ mod ^ (reg << 3)));
                else machineCode.add((byte) (0b00000001 ^ mod ^ (reg << 3)));
            } else {
                if (operandTokens.get(indexToken + next).getStringToken().equals("SI"))
                    machineCode.add((byte) (0b00000010 ^ mod ^ (reg << 3)));
                else machineCode.add((byte) (0b00000011 ^ mod ^ (reg << 3)));
            }

            indexToken += 2;
            if (isIdentifier) {
                machineCode.add((byte) 0x0);
                machineCode.add((byte) (offset + Integer.parseInt(operandTokens.get(indexToken).getStringToken())));

            } else machineCode.add((byte) Integer.parseInt(operandTokens.get(indexToken).getStringToken()));//Imm
        } else if (mem.isSibDisp()) {

            indexToken++;
            if(mem.equalOperandType(TokenType.Mem)) indexToken--;
            machineCode.addAll(opCode);//opcode
            machineCode.add((byte) (0b00000100 ^ mod ^ (reg << 3)));//modrm
            machineCode.add((byte) (registerInfoStorage.getByteReg(mem.getStringTokenByIndex(indexToken)) ^
                    (registerInfoStorage.getByteReg(mem.getStringTokenByIndex(indexToken + next)) << 3)));//SIB

            indexToken += 2;
            if (isIdentifier) {

                machineCode.add((byte) 0x0);
                machineCode.add((byte) 0x0);
                machineCode.add((byte) 0x0);
                machineCode.add((byte) (offset + Integer.parseInt(operandTokens.get(indexToken).getStringToken())));

            } else machineCode.add((byte) Integer.parseInt(operandTokens.get(indexToken).getStringToken()));//Imm
        } else {
            machineCode.addAll(opCode);
            machineCode.add((byte) (0b00000101 ^ (reg << 3)));
            machineCode.add((byte) 0x0);
            machineCode.add((byte) 0x0);
            machineCode.add((byte) 0x0);
            machineCode.add(offset);
        }
        return machineCode;

    }

}
