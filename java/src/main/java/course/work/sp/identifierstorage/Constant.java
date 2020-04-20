package course.work.sp.identifierstorage;

import course.work.sp.tokenizer.Token;
import course.work.sp.tokenizer.TokenType;

public class Constant {

    private int index;
    private String constant;
    private TokenType type;

    public Constant(int index, String constant, TokenType type) {
        this.index = index;
        this.constant = constant;
        this.type = type;
    }

    public int getIndex() {
        return index;
    }

    public void setIndex(int index) {
        this.index = index;
    }

    public String getConstant() {
        return constant;
    }

    public void setConstant(String constant) {
        this.constant = constant;
    }

    @Override
    public String toString() {
        StringBuilder pass = new StringBuilder();
        pass.append(index).append(" ").append(type).append(" ").append(constant).append("\n");
        return pass.toString();
    }

    //TODO

}
