package course.work.sp.fileparser;

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

    @Override
    public String toString() {
        StringBuilder pass = new StringBuilder();
        for (Token tk: tokens)
        pass.append(tk.getType()).append(" | ");
        return pass.toString();
    }
}
