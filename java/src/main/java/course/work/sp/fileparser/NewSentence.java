package course.work.sp.fileparser;

import course.work.sp.tokenizer.Token;
import course.work.sp.tokenizer.TokenType;

import java.util.List;

public class NewSentence {

    private int number;
    private Token identifier;
    private Token instruction;
    private List<Operand> operands;
    private String line;
    private boolean error = false;

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
        if(identifier.equals(TokenType.Unknown) && instruction.equals(TokenType.Unknown) && operands.size() != 0)
            error = true;

        if(identifier.equals(TokenType.Identifier) && (instruction.equals(TokenType.Instruction) ||
                instruction.equals(TokenType.KeyWord)||
                instruction.equals(TokenType.JncWord)||
                instruction.equals(TokenType.JmpWord)))
            error = true;

        if(operands != null)
            for (Operand op: this.operands){
                if(op.equalOperandType(TokenType.Unknown)) error = true;
            }
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

    @Override
    public String toString() {
        StringBuilder pass = new StringBuilder();
        pass.append(line).append("\n");
        pass.append(number).append("  ").append(identifier.getType()).append("  ").append(instruction.getType()).append("\n");
        for (Operand op: operands)
            pass.append(op.toString()).append("\n");
        pass.append(error).append("\n");
        return  pass.toString();
    }
}
