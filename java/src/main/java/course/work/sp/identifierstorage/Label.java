package course.work.sp.identifierstorage;

import course.work.sp.tokenizer.Token;
import course.work.sp.tokenizer.TokenType;

public class Label {
    private int index;
    private String label;
    private int offset;
    private String segment;

    public Label(int index, String label) {
        this.index = index;
        this.label = label;
    }

    public int getIndex() {
        return index;
    }

    public void setIndex(int index) {
        this.index = index;
    }

    public String getLabel() {
        return label;
    }

    public void setLabel(String label) {
        this.label = label;
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
        pass.append(label).append("| ").append(TokenType.Label).append("   | ").append(segment).append(String.format(" | %08X ", offset)).append("\n");
        return pass.toString();
    }
}
