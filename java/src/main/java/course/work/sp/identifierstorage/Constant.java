package course.work.sp.identifierstorage;

import course.work.sp.tokenizer.Token;
import course.work.sp.tokenizer.TokenType;

public class Constant {

    private int index;
    private String constant;
    private TokenType type;
    private int offset;
    private String segment;



    public Constant(int index, String constant, TokenType type) {
        this.index = index;
        this.constant = constant;
        this.type = type;
    }

    public int getIndex() {
        return index;
    }

    public String getConstant() {
        return constant;
    }

    public TokenType getType() {
        return type;
    }

    public int getOffset() {
        return offset;
    }

    public void setOffset(int offset) {
        this.offset = offset;
    }

    public String getSegment() {
        return segment;
    }

    public void setSegment(String segment) {
        this.segment = segment;
    }

    @Override
    public String toString() {
        StringBuilder pass = new StringBuilder();
        pass.append(constant).append("  | ").append(type).append(" | ").append(segment).append(String.format(" | %08X ", offset)).append("\n");
        return pass.toString();
    }

    //TODO

}
