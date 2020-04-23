package course.work.sp.identifierstorage;

import course.work.sp.tokenizer.Token;
import course.work.sp.tokenizer.TokenType;

public class Segment {
    private String segment;
    private TokenType type;
    private int indexStart;
    private int indexFinish;
    private int offSet;
    private boolean error = true;

    public Segment(String segment, TokenType type, int indexStart) {
        this.segment = segment;
        this.type = type;
        this.indexStart = indexStart;
    }

    public String getSegment() {
        return segment;
    }

    public void setSegment(String segment) {
        this.segment = segment;
    }

    public int getIndexStart() {
        return indexStart;
    }

    public int getIndexFinish() {
        return indexFinish;
    }

    public void setIndexFinish(int indexFinish) {
        this.indexFinish = indexFinish;
        error = false;
    }

    public int getOffSet() {
        return offSet;
    }

    public void setOffSet(int offSet) {
        this.offSet = offSet;
    }

    public boolean isError() {
        return error;
    }

    public boolean equalSegment(String string){
        return string.equals(segment);
    }

    @Override
    public String toString() {
        StringBuilder pass = new StringBuilder();
        pass.append(segment).append(" |          32   ").append(String.format("  | %08X  ", offSet)).append("\n");
        return pass.toString();
    }
}
