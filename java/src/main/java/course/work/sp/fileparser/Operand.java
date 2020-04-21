package course.work.sp.fileparser;

import course.work.sp.tokenizer.Token;
import course.work.sp.tokenizer.TokenType;

import java.util.List;

public class Operand {
    public static final int next = 1;
    private TokenType operandType;
    private boolean segmentReg = false;
    private boolean sibRegIs16 = false;
    private boolean isSibDisp = false;
    private List<Token> tokens;

    public Operand(List<Token> tokens) {
        this.tokens = tokens;
        boolean error = false;
        for (int index = 0; index < tokens.size(); index++) {
            if (tokens.get(index).equals(TokenType.Unknown)) {
                error = true;
                break;
            }
            if (tokens.get(index).equals(TokenType.BytePtr) || tokens.get(index).equals(TokenType.WordPtr) || tokens.get(index).equals(TokenType.DwordPtr)) {
                if (tokens.get(++index).equals(TokenType.Ptr)) {
                    this.tokens.remove(index);
                } else {
                    error = true;
                    break;
                }
            }
            if (tokens.get(index).equals(TokenType.Symbol)) {
                this.tokens.remove(index);
                index--;
            }
        }
        if(!error){
            operandType = createOperandType();

        }else {
            operandType = TokenType.Unknown;
        }

    }


    private TokenType createOperandType() {
        int index = 0;
        if (tokens.size() == 1 && !tokens.get(index).equals(TokenType.Identifier)) {
            if (tokens.get(index).equals(TokenType.Reg8)) return TokenType.Reg8;
            if (tokens.get(index).equals(TokenType.Reg16)) return TokenType.Reg16;
            if (tokens.get(index).equals(TokenType.Reg32)) return TokenType.Reg32;
            if (tokens.get(index).equals(TokenType.Text)) return TokenType.Text;
            if (tokens.get(index).equals(TokenType.DecNumber)) return TokenType.Imm;
            if (tokens.get(index).equals(TokenType.HexNumber)) return TokenType.Imm;
            if (tokens.get(index).equals(TokenType.BinNumber)) return TokenType.Imm;
        }else{
            if(tokens.get(index).equals(TokenType.SegmentRegister)){
                segmentReg = true;
                index++;
            }
            if(sibPlusDisp(index)){
                sibRegIs16 = sibRegTypeIs16(index);
                isSibDisp = true;
                return TokenType.Mem;
            }else if (tokens.get(index).equals(TokenType.DwordPtr) || tokens.get(index).equals(TokenType.WordPtr) || tokens.get(index).equals(TokenType.BytePtr)){
                if(tokens.size() > 3){
                    if(sibPlusDisp(index + next)) {
                        sibRegIs16 = sibRegTypeIs16(index + next);
                        isSibDisp = true;
                        if(tokens.get(index).equals(TokenType.DwordPtr)) return TokenType.Mem32;
                        if(tokens.get(index).equals(TokenType.WordPtr)) return TokenType.Mem16;
                        if(tokens.get(index).equals(TokenType.BytePtr)) return TokenType.Mem8;
                    }else{
                        return TokenType.Unknown;
                    }
                }
            }else if(tokens.get(index).equals(TokenType.Identifier)){
                if(tokens.size() > 3) {
                    if (sibPlusDisp(index + next)) {
                        sibRegIs16 = sibRegTypeIs16(index + next);
                        isSibDisp = true;
                        return TokenType.Identifier;
                    } else return TokenType.Identifier;
                }else return TokenType.Identifier;
            }
        }
        return TokenType.Unknown;
    }

    private boolean sibPlusDisp(int index){
        return (
                (tokens.get(index).equals(TokenType.Reg16) && tokens.get(index + next).equals(TokenType.Reg16)) ||
                (tokens.get(index).equals(TokenType.Reg32) && tokens.get(index + next).equals(TokenType.Reg32))
                                                    ) && (tokens.get(index + next + next).equals(TokenType.DecNumber) ||
                                                         tokens.get(index + next + next).equals(TokenType.HexNumber) ||
                                                         tokens.get(index + next + next).equals(TokenType.BinNumber));
    }

    private boolean sibRegTypeIs16(int index){
        return tokens.get(index).equals(TokenType.Reg16);
    }

    public boolean isSegmentReg() {
        return segmentReg;
    }

    public boolean isSibRegIs16() {
        return sibRegIs16;
    }

    public boolean isSibDisp() {
        return isSibDisp;
    }


    public TokenType getOperandType() {
        return operandType;
    }

    public void setOperandType(TokenType operandType) {
        this.operandType = operandType;
    }

    public boolean equalOperandType(TokenType operandType) {
       return operandType == this.operandType;
    }

    public List<Token> getTokens() {
        return tokens;
    }

    @Override
    public String toString() {
        StringBuilder pass = new StringBuilder();
        if(segmentReg) pass.append("SEGREG ( ").append(segmentReg).append(" )");
        if(isSibDisp) pass.append(" isSib ( ").append(isSibDisp).append(" ) is16Sib (").append(sibRegIs16).append(" ) ");
        pass.append(operandType).append(": ");
        for (Token tk: tokens)
        pass.append(tk.getType()).append(" | ");
        return pass.toString();
    }
}
