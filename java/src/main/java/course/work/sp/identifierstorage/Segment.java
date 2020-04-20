package course.work.sp.identifierstorage;

import course.work.sp.tokenizer.Token;
import course.work.sp.tokenizer.TokenType;

public class Segment {
    private String segment;
    private TokenType type;
    private int indexStart;
    private int indexFinish;
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

    public boolean isError() {
        return error;
    }

    public boolean equalSegment(String string){
        return string.equals(segment);
    }

    @Override
    public String toString() {
        StringBuilder pass = new StringBuilder();
        pass.append(error).append(" ").append(indexStart).append("-").append(indexFinish).append(" ").append(type).append(" ").append(segment).append("\n");
        return pass.toString();
    }
}
