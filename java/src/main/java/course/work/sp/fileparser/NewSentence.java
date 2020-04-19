package course.work.sp;

import course.work.sp.tokenizer.Token;

import java.util.List;

public class NewSentence {

    private int number;
    private Token identifier;
    private Token instruction;
    private List<Operand> operands;
    private String line;
    private boolean error = true;

    public boolean isError() {
        return error;
    }

    public void setError(boolean error) {
        this.error = error;
    }

    public NewSentence(int number, Token identifier, Token instruction, List<Operand> operands, String line) {
        this.number = number;
        this.identifier = identifier;
        this.instruction = instruction;
        this.operands = operands;
        this.line = line;
    }

    public Token getIdentifier() {
        return identifier;
    }

    public void setIdentifier(Token identifier) {
        this.identifier = identifier;
    }

    public Token getInstruction() {
        return instruction;
    }

    public void setInstruction(Token instruction) {
        this.instruction = instruction;
    }

    public List<Operand> getOperands() {
        return operands;
    }

    public void setOperands(List<Operand> operands) {
        this.operands = operands;
    }

    public String getLine() {
        return line;
    }

    public void setLine(String line) {
        this.line = line;
    }

    public int getNumber() {
        return number;
    }

    public void setNumber(int number) {
        this.number = number;
    }
}
