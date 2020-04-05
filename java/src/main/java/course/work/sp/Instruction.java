package course.work.sp;

import java.util.ArrayList;
import java.util.List;

public class Instruction {
    public int index;
    public int offset = 0;
    public boolean Error = false;
    public String token;
    public Instruction(int index, List<Token> arrayTokens){
        this.index = index;
        List<List<Token>> operands = new ArrayList<>();
        List<Token> operand = new ArrayList<>();
        for(int indexOfToken = 0; indexOfToken < arrayTokens.size(); indexOfToken++){

            if(arrayTokens.get(indexOfToken).type != TokenType.Instruction &&
                    arrayTokens.get(indexOfToken).type != TokenType.JmpWord &&
                    arrayTokens.get(indexOfToken).type != TokenType.JncWord ){
                    if(arrayTokens.get(indexOfToken).type == TokenType.Comma){
                        operands.add(operand);
                        operand = new ArrayList<>();
                    }else {
                        operand.add(arrayTokens.get(indexOfToken));
                        if(indexOfToken == arrayTokens.size() - 1) {
                            operands.add(operand);
                            operand = new ArrayList<>();
                        }
                    }
            }else {
                    token = arrayTokens.get(indexOfToken).stringToken;
                }
        }

       if(token != null)
        switch (token){
            case ( "STC"):
                stc(operands);
                break;
            case ( "PUSH"):
                push(operands);
                break;
            case ( "MOV"):
                mov(operands);
                break;
            case ( "SUB"):
                sub(operands);
                break;
            case ( "MUL"):
                mul(operands);
                break;
            case ( "XOR"):
                xor(operands);
                break;
            case ( "BTR"):
                btr(operands);
                break;
            case ( "ADC"):
                adc(operands);
                break;
            case ( "JMP"):
                jmp(operands);
                break;
            case ( "JNC"):
                jnc(operands);
                break;
            default:
                break;
        }
    }



    public void stc(List<List<Token>> operands){
        if(operands.size() == 0) {
            offset += 1;
        }else {
            Error = true;
        }
    }

    public void push(List<List<Token>> operands){
        if(operands.size() == 1 && operands.get(0).size() == 1 &&
                (operands.get(0).get(0).type.equals(TokenType.Reg16) || operands.get(0).get(0).type.equals(TokenType.Reg32))){
            offset += 1;
        }else {
            Error = true;
        }
    }


    public void mov(List<List<Token>> operands){
        if(operands.size() == 2 && operands.get(0).size() == 1 && operands.get(1).size() == 1 &&
                (operands.get(0).get(0).type.equals(TokenType.Reg16) && operands.get(1).get(0).type.equals(TokenType.Reg16)) ||
                        (operands.get(0).get(0).type.equals(TokenType.Reg32) && operands.get(1).get(0).type.equals(TokenType.Reg32))){
                    if(operands.get(0).get(0).type.equals(TokenType.Reg16)) offset += 3;
                    else offset += 2;
        }else{
            Error = true;
        }

    }

    public void sub(List<List<Token>> operands){
        if(operands.size() == 2 && operands.get(0).size() == 1 && operands.get(1).size() == 1 &&
                (operands.get(0).get(0).type.equals(TokenType.Reg16) && (operands.get(1).get(0).type.equals(TokenType.DecNumber) ||
                         operands.get(1).get(0).type.equals(TokenType.HexNumber)) || operands.get(1).get(0).type.equals(TokenType.BinNumber)) ||
                (operands.get(0).get(0).type.equals(TokenType.Reg32) && (operands.get(1).get(0).type.equals(TokenType.DecNumber) ||
                        operands.get(1).get(0).type.equals(TokenType.HexNumber)) || operands.get(1).get(0).type.equals(TokenType.BinNumber))){
            if(operands.get(0).get(0).type.equals(TokenType.Reg16)) offset += 4;
            else offset += 3;
        }else{
            Error = true;
        }

    }

    public void mul(List<List<Token>> operands) {
        int isSegmentRegister = 0;
        if (operands.size() != 1) {
            Error = true;
        }
        if (operands.get(0).get(0).type.equals(TokenType.Reg16) ||
                operands.get(0).get(0).type.equals(TokenType.Reg32) ||
                operands.get(0).get(0).type.equals(TokenType.DecNumber) ||
                operands.get(0).get(0).type.equals(TokenType.HexNumber) ||
                operands.get(0).get(0).type.equals(TokenType.BinNumber)) {
            Error = true;
        }
        if (operands.get(0).get(0).type.equals(TokenType.SegmentRegister) && !operands.get(0).get(0).stringToken.equals("DS")) {
            offset += 1;
            isSegmentRegister += 2;
        }else if(operands.get(0).get(0).stringToken.equals("DS")) isSegmentRegister += 2;
        if ((operands.get(0).get(isSegmentRegister).type.equals(TokenType.DwordPtr) ||
                operands.get(0).get(isSegmentRegister).type.equals(TokenType.WordPtr) ||
                operands.get(0).get(isSegmentRegister).type.equals(TokenType.BytePtr)) &&
                operands.get(0).get(1 + isSegmentRegister).type.equals(TokenType.Ptr)) {
            if (operands.get(0).get(2 + isSegmentRegister).type.equals(TokenType.Symbol) &&
                    (operands.get(0).get(3 + isSegmentRegister).type.equals(TokenType.Reg32) ||
                            operands.get(0).get(3 + isSegmentRegister).type.equals(TokenType.Reg16)) &&
                    operands.get(0).get(4 + isSegmentRegister).type.equals(TokenType.Symbol) &&
                    (operands.get(0).get(5 + isSegmentRegister).type.equals(TokenType.Reg32) ||
                            operands.get(0).get(5 + isSegmentRegister).type.equals(TokenType.Reg16)) &&
                    operands.get(0).get(6 + isSegmentRegister).type.equals(TokenType.Symbol) &&
                    (operands.get(0).get(7 + isSegmentRegister).type.equals(TokenType.DecNumber) ||
                            operands.get(0).get(7 + isSegmentRegister).type.equals(TokenType.HexNumber) ||
                            operands.get(0).get(7 + isSegmentRegister).type.equals(TokenType.BinNumber)) &&
                    operands.get(0).get(8 + isSegmentRegister).type.equals(TokenType.Symbol)) {

                if (operands.get(0).get(3 + isSegmentRegister).type.equals(TokenType.Reg16)) offset += 5;
                else offset += 4;
            }
        } else if (operands.get(0).get(isSegmentRegister).type.equals(TokenType.Identifier) && operands.get(0).size() != isSegmentRegister + 1) {
            if (operands.get(0).get(1 + isSegmentRegister).type.equals(TokenType.Symbol) &&
                    (operands.get(0).get(2 + isSegmentRegister).type.equals(TokenType.Reg32) ||
                            operands.get(0).get(2 + isSegmentRegister).type.equals(TokenType.Reg16)) &&
                    operands.get(0).get(3 + isSegmentRegister).type.equals(TokenType.Symbol) &&
                    (operands.get(0).get(4 + isSegmentRegister).type.equals(TokenType.Reg32) ||
                            operands.get(0).get(4 + isSegmentRegister).type.equals(TokenType.Reg16)) &&
                    operands.get(0).get(5 + isSegmentRegister).type.equals(TokenType.Symbol) &&
                    (operands.get(0).get(6 + isSegmentRegister).type.equals(TokenType.DecNumber) ||
                            operands.get(0).get(6 + isSegmentRegister).type.equals(TokenType.HexNumber) ||
                            operands.get(0).get(6 + isSegmentRegister).type.equals(TokenType.BinNumber)) &&
                    operands.get(0).get(7 + isSegmentRegister).type.equals(TokenType.Symbol)) {

                if (operands.get(0).get(3).type.equals(TokenType.Reg16)) offset += 4;
                else offset += 5;
            }

        } else if (operands.get(0).size() == isSegmentRegister + 1 && operands.get(0).get(isSegmentRegister).type.equals(TokenType.Identifier)) offset += 4;
        else Error = true;
        if (Error) offset = 0;
    }

    public void xor(List<List<Token>> operands){
        if(operands.size() == 2 && operands.get(0).size() == 1 &&
                (operands.get(0).get(0).type.equals(TokenType.Reg16) || operands.get(0).get(0).type.equals(TokenType.Reg32))){
        }else {
            Error = true;
        }

        int isSegmentRegister = 0;
        if (operands.get(1).get(0).type.equals(TokenType.Reg16) ||
                operands.get(1).get(0).type.equals(TokenType.Reg32) ||
                operands.get(1).get(0).type.equals(TokenType.DecNumber) ||
                operands.get(1).get(0).type.equals(TokenType.HexNumber) ||
                operands.get(1).get(0).type.equals(TokenType.BinNumber)) {
            Error = true;
        }
        if (operands.get(1).get(0).type.equals(TokenType.SegmentRegister) && !operands.get(1).get(0).stringToken.equals("DS")) {
            offset += 1;
            isSegmentRegister += 2;
        }else if(operands.get(1).get(0).stringToken.equals("DS")) isSegmentRegister += 2;
        if ((operands.get(1).get(isSegmentRegister).type.equals(TokenType.DwordPtr) ||
                operands.get(1).get(isSegmentRegister).type.equals(TokenType.WordPtr) ||
                operands.get(1).get(isSegmentRegister).type.equals(TokenType.BytePtr)) &&
                operands.get(1).get(1 + isSegmentRegister).type.equals(TokenType.Ptr)) {
            if (operands.get(1).get(2 + isSegmentRegister).type.equals(TokenType.Symbol) &&
                    (operands.get(1).get(3 + isSegmentRegister).type.equals(TokenType.Reg32) ||
                            operands.get(1).get(3 + isSegmentRegister).type.equals(TokenType.Reg16)) &&
                    operands.get(1).get(4 + isSegmentRegister).type.equals(TokenType.Symbol) &&
                    (operands.get(1).get(5 + isSegmentRegister).type.equals(TokenType.Reg32) ||
                            operands.get(1).get(5 + isSegmentRegister).type.equals(TokenType.Reg16)) &&
                    operands.get(1).get(6 + isSegmentRegister).type.equals(TokenType.Symbol) &&
                    (operands.get(1).get(7 + isSegmentRegister).type.equals(TokenType.DecNumber) ||
                            operands.get(1).get(7 + isSegmentRegister).type.equals(TokenType.HexNumber) ||
                            operands.get(1).get(7 + isSegmentRegister).type.equals(TokenType.BinNumber)) &&
                    operands.get(1).get(8 + isSegmentRegister).type.equals(TokenType.Symbol)) {

                if (operands.get(1).get(3 + isSegmentRegister).type.equals(TokenType.Reg16)) offset += 5;
                else offset += 4;
            }
        } else if (operands.get(1).get(isSegmentRegister).type.equals(TokenType.Identifier) && operands.get(1).size() != isSegmentRegister + 1) {
            if (operands.get(1).get(1 + isSegmentRegister).type.equals(TokenType.Symbol) &&
                    (operands.get(1).get(2 + isSegmentRegister).type.equals(TokenType.Reg32) ||
                            operands.get(1).get(2 + isSegmentRegister).type.equals(TokenType.Reg16)) &&
                    operands.get(1).get(3 + isSegmentRegister).type.equals(TokenType.Symbol) &&
                    (operands.get(1).get(4 + isSegmentRegister).type.equals(TokenType.Reg32) ||
                            operands.get(1).get(4 + isSegmentRegister).type.equals(TokenType.Reg16)) &&
                    operands.get(1).get(5 + isSegmentRegister).type.equals(TokenType.Symbol) &&
                    (operands.get(1).get(6 + isSegmentRegister).type.equals(TokenType.DecNumber) ||
                            operands.get(1).get(6 + isSegmentRegister).type.equals(TokenType.HexNumber) ||
                            operands.get(1).get(6 + isSegmentRegister).type.equals(TokenType.BinNumber)) &&
                    operands.get(1).get(7 + isSegmentRegister).type.equals(TokenType.Symbol)) {

                if (operands.get(1).get(3).type.equals(TokenType.Reg16)) offset += 6;
                else offset += 7;
            }

        } else if (operands.get(1).size() == isSegmentRegister + 1 && operands.get(1).get(isSegmentRegister).type.equals(TokenType.Identifier)) offset += 6;
            else Error = true;
        if (Error) offset = 0;

    }

    public void btr(List<List<Token>> operands) {

        if(operands.size() == 2 && operands.get(1).size() == 1 &&
                (operands.get(1).get(0).type.equals(TokenType.Reg16) || operands.get(1).get(0).type.equals(TokenType.Reg32))){
            if (operands.get(1).get(0).type.equals(TokenType.Reg32)) offset += 1;
            else offset += 2;
        }else {
            Error = true;
        }

        int isSegmentRegister = 0;

        if (operands.get(0).get(0).type.equals(TokenType.Reg16) ||
                operands.get(0).get(0).type.equals(TokenType.Reg32) ||
                operands.get(0).get(0).type.equals(TokenType.DecNumber) ||
                operands.get(0).get(0).type.equals(TokenType.HexNumber) ||
                operands.get(0).get(0).type.equals(TokenType.BinNumber)) {
            Error = true;
        }
        if (operands.get(0).get(0).type.equals(TokenType.SegmentRegister) && !operands.get(0).get(0).stringToken.equals("DS")) {
            offset += 1;
            isSegmentRegister += 2;
        }else if(operands.get(0).get(0).stringToken.equals("DS")) isSegmentRegister += 2;
        if ((operands.get(0).get(isSegmentRegister).type.equals(TokenType.DwordPtr) ||
                operands.get(0).get(isSegmentRegister).type.equals(TokenType.WordPtr) ||
                operands.get(0).get(isSegmentRegister).type.equals(TokenType.BytePtr)) &&
                operands.get(0).get(1 + isSegmentRegister).type.equals(TokenType.Ptr)) {
            if (operands.get(0).get(2 + isSegmentRegister).type.equals(TokenType.Symbol) &&
                    (operands.get(0).get(3 + isSegmentRegister).type.equals(TokenType.Reg32) ||
                            operands.get(0).get(3 + isSegmentRegister).type.equals(TokenType.Reg16)) &&
                    operands.get(0).get(4 + isSegmentRegister).type.equals(TokenType.Symbol) &&
                    (operands.get(0).get(5 + isSegmentRegister).type.equals(TokenType.Reg32) ||
                            operands.get(0).get(5 + isSegmentRegister).type.equals(TokenType.Reg16)) &&
                    operands.get(0).get(6 + isSegmentRegister).type.equals(TokenType.Symbol) &&
                    (operands.get(0).get(7 + isSegmentRegister).type.equals(TokenType.DecNumber) ||
                            operands.get(0).get(7 + isSegmentRegister).type.equals(TokenType.HexNumber) ||
                            operands.get(0).get(7 + isSegmentRegister).type.equals(TokenType.BinNumber)) &&
                    operands.get(0).get(8 + isSegmentRegister).type.equals(TokenType.Symbol)) {

                if (operands.get(0).get(3 + isSegmentRegister).type.equals(TokenType.Reg16)) offset += 5;
                else offset += 4;
            }
        } else if (operands.get(0).get(isSegmentRegister).type.equals(TokenType.Identifier) && operands.get(0).size() != isSegmentRegister + 1) {
            if (operands.get(0).get(1 + isSegmentRegister).type.equals(TokenType.Symbol) &&
                    (operands.get(0).get(2 + isSegmentRegister).type.equals(TokenType.Reg32) ||
                            operands.get(0).get(2 + isSegmentRegister).type.equals(TokenType.Reg16)) &&
                    operands.get(0).get(3 + isSegmentRegister).type.equals(TokenType.Symbol) &&
                    (operands.get(0).get(4 + isSegmentRegister).type.equals(TokenType.Reg32) ||
                            operands.get(0).get(4 + isSegmentRegister).type.equals(TokenType.Reg16)) &&
                    operands.get(0).get(5 + isSegmentRegister).type.equals(TokenType.Symbol) &&
                    (operands.get(0).get(6 + isSegmentRegister).type.equals(TokenType.DecNumber) ||
                            operands.get(0).get(6 + isSegmentRegister).type.equals(TokenType.HexNumber) ||
                            operands.get(0).get(6 + isSegmentRegister).type.equals(TokenType.BinNumber)) &&
                    operands.get(0).get(7 + isSegmentRegister).type.equals(TokenType.Symbol)) {

                if (operands.get(0).get(3).type.equals(TokenType.Reg16)) offset += 4;
                else offset += 5;
            }

        } else if (operands.get(0).size() == isSegmentRegister + 1 && operands.get(0).get(isSegmentRegister).type.equals(TokenType.Identifier)) offset += 4;
        else Error = true;
        if (Error) offset = 0;

    }

    public void adc(List<List<Token>> operands) {
       if (operands.size() == 2 && (operands.get(1).get(0).type.equals(TokenType.DecNumber) ||
                operands.get(1).get(0).type.equals(TokenType.HexNumber) || operands.get(1).get(0).type.equals(TokenType.BinNumber))){
           offset += 1;
       }else Error = true;
        int isSegmentRegister = 0;

        if (operands.get(0).get(0).type.equals(TokenType.Reg16) ||
                operands.get(0).get(0).type.equals(TokenType.Reg32) ||
                operands.get(0).get(0).type.equals(TokenType.DecNumber) ||
                operands.get(0).get(0).type.equals(TokenType.HexNumber) ||
                operands.get(0).get(0).type.equals(TokenType.BinNumber)) {
            Error = true;
        }
        if (operands.get(0).get(0).type.equals(TokenType.SegmentRegister) && !operands.get(0).get(0).stringToken.equals("DS")) {
            offset += 1;
            isSegmentRegister += 2;
        }else if(operands.get(0).get(0).stringToken.equals("DS")) isSegmentRegister += 2;
        if ((operands.get(0).get(isSegmentRegister).type.equals(TokenType.DwordPtr) ||
                operands.get(0).get(isSegmentRegister).type.equals(TokenType.WordPtr) ||
                operands.get(0).get(isSegmentRegister).type.equals(TokenType.BytePtr)) &&
                operands.get(0).get(1 + isSegmentRegister).type.equals(TokenType.Ptr)) {
            if (operands.get(0).get(2 + isSegmentRegister).type.equals(TokenType.Symbol) &&
                    (operands.get(0).get(3 + isSegmentRegister).type.equals(TokenType.Reg32) ||
                            operands.get(0).get(3 + isSegmentRegister).type.equals(TokenType.Reg16)) &&
                    operands.get(0).get(4 + isSegmentRegister).type.equals(TokenType.Symbol) &&
                    (operands.get(0).get(5 + isSegmentRegister).type.equals(TokenType.Reg32) ||
                            operands.get(0).get(5 + isSegmentRegister).type.equals(TokenType.Reg16)) &&
                    operands.get(0).get(6 + isSegmentRegister).type.equals(TokenType.Symbol) &&
                    (operands.get(0).get(7 + isSegmentRegister).type.equals(TokenType.DecNumber) ||
                            operands.get(0).get(7 + isSegmentRegister).type.equals(TokenType.HexNumber) ||
                            operands.get(0).get(7 + isSegmentRegister).type.equals(TokenType.BinNumber)) &&
                    operands.get(0).get(8 + isSegmentRegister).type.equals(TokenType.Symbol)) {

                if (operands.get(0).get(3 + isSegmentRegister).type.equals(TokenType.Reg16)) offset += 4s;
                else offset += 3;
            }
        } else if (operands.get(0).get(isSegmentRegister).type.equals(TokenType.Identifier) && operands.get(0).size() != isSegmentRegister + 1) {
            if (operands.get(0).get(1 + isSegmentRegister).type.equals(TokenType.Symbol) &&
                    (operands.get(0).get(2 + isSegmentRegister).type.equals(TokenType.Reg32) ||
                            operands.get(0).get(2 + isSegmentRegister).type.equals(TokenType.Reg16)) &&
                    operands.get(0).get(3 + isSegmentRegister).type.equals(TokenType.Symbol) &&
                    (operands.get(0).get(4 + isSegmentRegister).type.equals(TokenType.Reg32) ||
                            operands.get(0).get(4 + isSegmentRegister).type.equals(TokenType.Reg16)) &&
                    operands.get(0).get(5 + isSegmentRegister).type.equals(TokenType.Symbol) &&
                    (operands.get(0).get(6 + isSegmentRegister).type.equals(TokenType.DecNumber) ||
                            operands.get(0).get(6 + isSegmentRegister).type.equals(TokenType.HexNumber) ||
                            operands.get(0).get(6 + isSegmentRegister).type.equals(TokenType.BinNumber)) &&
                    operands.get(0).get(7 + isSegmentRegister).type.equals(TokenType.Symbol)) {

                if (operands.get(0).get(3).type.equals(TokenType.Reg16)) offset += 4;
                else offset += 5;
            }

        } else if (operands.get(0).size() == isSegmentRegister + 1 && operands.get(0).get(isSegmentRegister).type.equals(TokenType.Identifier)) offset += 4;
        else Error = true;
        if (Error) offset = 0;

    }

    public void jmp(List<List<Token>> operands){
        boolean isFind = false;
        if(operands.size() == 1 && operands.get(0).size() == 1 && operands.get(0).get(0).type.equals(TokenType.Identifier)){
            List<Identifier> tableIdentifier = FirstPass.TableIdentifier(FileParser.fileParser(Application.getFilepath()));
            for (Identifier identifier : tableIdentifier)
                if (identifier.identifier == TokenType.Label && identifier.token.equals(operands.get(0).get(0).stringToken)) {
                    if (identifier.index > this.index) {
                        offset += 5;
                        isFind = true;
                    } else {
                        isFind = true;
                        offset += 2;
                    }
                }
            if(!isFind) Error = false;
        }else {
            Error = false;
        }
    }

    public void jnc(List<List<Token>> operands){
        boolean isFind = false;
        if(operands.size() == 1 && operands.get(0).size() == 1 && operands.get(0).get(0).type.equals(TokenType.Identifier)){
            List<Identifier> tableIdentifier = FirstPass.TableIdentifier(FileParser.fileParser(Application.getFilepath()));
            for (Identifier identifier : tableIdentifier)
                if (identifier.identifier == TokenType.Label && identifier.token.equals(operands.get(0).get(0).stringToken)) {
                    if (identifier.index > this.index) {
                        offset += 6;
                        isFind = true;
                    } else {
                        isFind = true;
                        offset += 2;
                    }
                }
            if(!isFind) Error = false;
        }else {
            Error = false;
        }
    }


    public String toString() {
        return "Offset " + String.format("%08X", offset) + " |Index " + String.format("%02d", index) + " |Name " + token  + "\n";
    }
}
