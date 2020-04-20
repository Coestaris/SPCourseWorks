package course.work.sp.identifierstorage;

import course.work.sp.tokenizer.Token;
import course.work.sp.tokenizer.TokenType;

public class Label {
    private int index;
    private String label;

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

    @Override
    public String toString() {
        StringBuilder pass = new StringBuilder();
        pass.append(index).append(" ").append(TokenType.Label).append(" ").append(label).append("\n");
        return pass.toString();
    }
}
