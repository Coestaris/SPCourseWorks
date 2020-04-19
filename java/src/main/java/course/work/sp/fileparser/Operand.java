package course.work.sp;

import course.work.sp.tokenizer.Token;
import course.work.sp.tokenizer.TokenType;

import java.util.List;

public class Operand {

    private TokenType operandType;
    private List<Token> tokens;

    public Operand(List<Token> tokens) {
        this.tokens = tokens;
    }

    public List<Token> getTokens() {
        return tokens;
    }

    public TokenType getOperandType() {
        return operandType;
    }

    public void setOperandType(TokenType operandType) {
        this.operandType = operandType;
    }
}
