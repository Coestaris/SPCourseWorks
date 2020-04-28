package course.work.sp.secondpass;

import course.work.sp.fileparser.Operand;
import course.work.sp.firstpass.FirstPassSentence;
import course.work.sp.identifierstorage.AssumeRegister;
import course.work.sp.identifierstorage.AssumeRegisterStorage;
import course.work.sp.identifierstorage.Constant;
import course.work.sp.identifierstorage.IdentifierStore;
import course.work.sp.tokenizer.Token;
import course.work.sp.tokenizer.TokenType;

import java.util.ArrayList;
import java.util.List;

public class SetMachineCode {
    private static final int next = 1;
    private RegisterInfoStorage registerInfoStorage= RegisterInfoStorage.getInstance();

    public List<Byte> setMachineCode(FirstPassSentence firstPassSentence){
        List<Byte> machineCode = new ArrayList<>();
        if(!(firstPassSentence.getNewSentence().getInstruction().getType().equals(TokenType.Empty) ||
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
                case ("MUL"):
                    return mul(firstPassSentence.getNewSentence().getOperands());
                case ("XOR"):
                    return xor(firstPassSentence.getNewSentence().getOperands());
                case ("BTR"):
                    return btr(firstPassSentence.getNewSentence().getOperands());
                case ("ADC"):
                    return adc(firstPassSentence.getNewSentence().getOperands());
//                case ("DB"):
//                    return dbDir(operands);
//                case ("DD"):
//                    return ddDir(operands);
//                case ("DW"):
//                    return dwDir(operands);
//                case ("JMP"):
//                    return jmp(operands, number);
//                case ("JNC"):
//                    return jnc(operands, number);
                default:
                    break;
            }
        }
        return machineCode;
    }

    private List<Byte> stc(){
        List<Byte> machineCode = new ArrayList<>();
        machineCode.add((byte)0xF9);
        return machineCode;
    }

    private List<Byte> push(List<Operand> operands){
        int indexOperand = 0;
        int indexToken = 0;
        List<Byte> machineCode = new ArrayList<>();
        if(operands.get(indexOperand).equalOperandType(TokenType.Reg16)) machineCode.add((byte)0x66);
        machineCode.add((byte) (0x50 + registerInfoStorage.getByteReg(operands.get(indexOperand).getStringTokenByIndex(indexToken))));
        return machineCode;
    }

    private List<Byte> mov(List<Operand> operands){
        int indexOperand = 0;
        int indexToken = 0;
        List<Byte> machineCode = new ArrayList<>();
        if(operands.get(indexOperand).equalOperandType(TokenType.Reg16)) machineCode.add((byte)0x66);
        machineCode.add((byte) 0x8b);
        machineCode.add((byte) (0b11000000 ^
                (registerInfoStorage.getByteReg(operands.get(indexOperand).getStringTokenByIndex(indexToken)) << 3) ^
                (registerInfoStorage.getByteReg(operands.get(indexOperand + next).getStringTokenByIndex(indexToken)))));
        return machineCode;
    }

    private List<Byte> mul(List<Operand> operands){
        int indexOperand = 0;
        int indexToken = 0;
        List<Token> operandTokens= operands.get(indexOperand).getTokens();
        List<Byte> machineCode = new ArrayList<>();
        if(operands.get(indexOperand).isSegmentReg()){
            if (!operandTokens.get(indexToken).getStringToken().equals("DS"))
            machineCode.add( AssumeRegisterStorage.getInstance().getSegmentRegisterByte(operandTokens.get(indexToken).getStringToken()));
            indexToken++;
        }
        if(operands.get(indexOperand).isSibRegIs16()) {
            indexToken++;
            machineCode.add((byte) 0x67);//prefix
            if(operands.get(indexOperand).equalOperandType(TokenType.Mem8)) machineCode.add((byte) 0xF6);//opcode
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
        }else{
            indexToken++;
            if(operands.get(indexOperand).equalOperandType(TokenType.Mem8)) machineCode.add((byte) 0xF6);//opcode
            else machineCode.add((byte) 0xF7);//opcode

            machineCode.add((byte) 0b01100100);//modrm
            machineCode.add((byte) (registerInfoStorage.getByteReg(operands.get(indexOperand).getStringTokenByIndex(indexToken)) ^
                    (registerInfoStorage.getByteReg(operands.get(indexOperand).getStringTokenByIndex(indexToken + next)) << 3)));//SIB

            indexToken += 2;
            machineCode.add((byte) Integer.parseInt(operandTokens.get(indexToken).getStringToken()));//Imm
        }


        return machineCode;
    }

    private List<Byte> xor(List<Operand> operands){
        int indexToken = 0;
        int indexOperand = 0;
        Operand operand1 = operands.get(indexOperand);
        Operand mem = operands.get(indexOperand + next);
        List<Byte> machineCode = new ArrayList<>();
        List<Byte> opcode  = new ArrayList<>();
        opcode.add((byte) 0x33);
        byte reg = RegisterInfoStorage.getInstance().getByteReg(operand1.getStringTokenByIndex(indexToken));
        if(operand1.equalOperandType(TokenType.Reg16)) machineCode.add((byte) 0x66);
        machineCode.addAll(mem(mem, opcode, reg));
        return  machineCode;
    }

    private List<Byte> btr(List<Operand> operands){
        int indexToken = 0;
        int indexOperand = 0;
        Operand mem = operands.get(indexOperand);
        Operand operand1 = operands.get(indexOperand + next);
        List<Byte> machineCode = new ArrayList<>();
        List<Byte> opcode  = new ArrayList<>();
        opcode.add((byte) 0x0f);
        opcode.add((byte) 0xb3);
        byte reg = RegisterInfoStorage.getInstance().getByteReg(operand1.getStringTokenByIndex(indexToken));
        if(operand1.equalOperandType(TokenType.Reg16)) machineCode.add((byte) 0x66);
        machineCode.addAll(mem(mem, opcode, reg));
        return  machineCode;
    }

    private List<Byte> adc(List<Operand> operands){
        int indexToken = 0;
        int indexOperand = 0;
        Operand mem = operands.get(indexOperand);
        Operand operand1 = operands.get(indexOperand + next);
        List<Byte> opcode  = new ArrayList<>();
        opcode.add((byte) 0x83);
        byte reg = (byte) 0b010;
        List<Byte> machineCode = new ArrayList<>(mem(mem, opcode, reg));
        machineCode.add((byte) Integer.parseInt(operand1.getStringTokenByIndex(indexToken)));
        return  machineCode;
    }

    private List<Byte> sub(List<Operand> operands){
        int indexOperand = 0;
        int indexToken = 0;
        List<Byte> machineCode = new ArrayList<>();
        if(operands.get(indexOperand).equalOperandType(TokenType.Reg16)) machineCode.add((byte)0x66);
        machineCode.add((byte) 0x83);
        machineCode.add((byte) (0b11101000 ^ RegisterInfoStorage.getInstance().getByteReg(operands.get(indexOperand).getStringTokenByIndex(indexToken))));
        machineCode.add((byte) Integer.parseInt(operands.get(indexOperand + next).getStringTokenByIndex(indexToken)));
        return machineCode;
    }



    private List<Byte> mem(Operand mem, List<Byte> opCode, byte reg){
        int indexToken = 0;
        boolean isIdentifier = false;
        byte offset = 0;
        List<Token> operandTokens= mem.getTokens();
        List<Byte> machineCode = new ArrayList<>();
        if(mem.isSegmentReg()){
            if (!operandTokens.get(indexToken).getStringToken().equals("DS"))
                machineCode.add( AssumeRegisterStorage.getInstance().getSegmentRegisterByte(operandTokens.get(indexToken).getStringToken()));
            indexToken++;
        }
        if(mem.equalOperandType(TokenType.DwIdentifier) ||
                mem.equalOperandType(TokenType.DdIdentifier) ||
                mem.equalOperandType(TokenType.DbIdentifier)){
            isIdentifier = true;
            for (Constant constant:IdentifierStore.getInstance().getConstantList()){
                if(mem.getStringTokenByIndex(indexToken).equals(constant.getConstant()))
                    offset = (byte)constant.getOffset();
            }
        }
        if(mem.isSibRegIs16()) {
            indexToken++;
            machineCode.add((byte) 0x67);//prefix
            machineCode.addAll(opCode);//opcode
            if (operandTokens.get(indexToken).getStringToken().equals("BX")) {//modrm
                if (operandTokens.get(indexToken + next).getStringToken().equals("SI"))
                    machineCode.add((byte) (0b10000000 ^ (reg << 3)));
                else machineCode.add((byte) (0b10000001 ^ (reg << 3)));
            } else {
                if (operandTokens.get(indexToken + next).getStringToken().equals("SI"))
                    machineCode.add((byte) (0b10000010 ^ (reg << 3)));
                else machineCode.add((byte) (0b10000011 ^ (reg << 3)));
            }

            indexToken += 2;
            if(isIdentifier){
                machineCode.add((byte) 0x0);
                machineCode.add((byte) (offset + Integer.parseInt(operandTokens.get(indexToken).getStringToken())));

            }else machineCode.add((byte) Integer.parseInt(operandTokens.get(indexToken).getStringToken()));//Imm
        }else if(mem.isSibDisp()){

            indexToken++;
            machineCode.addAll(opCode);//opcode
            machineCode.add((byte) (0b10000100 ^ (reg << 3)));//modrm
            machineCode.add((byte) (registerInfoStorage.getByteReg(mem.getStringTokenByIndex(indexToken)) ^
                    (registerInfoStorage.getByteReg(mem.getStringTokenByIndex(indexToken + next)) << 3)));//SIB

            indexToken += 2;
            if(isIdentifier){
                machineCode.add((byte) 0x0);
                machineCode.add((byte) 0x0);
                machineCode.add((byte) 0x0);
                machineCode.add((byte) (offset + Integer.parseInt(operandTokens.get(indexToken).getStringToken())));

            }else machineCode.add((byte) Integer.parseInt(operandTokens.get(indexToken).getStringToken()));//Imm
        }else {
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
