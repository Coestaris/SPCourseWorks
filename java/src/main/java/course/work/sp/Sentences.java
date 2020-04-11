package course.work.sp;

import java.net.BindException;
import java.util.List;

public class Sentences {
    public int indexOfString;
    public int offset = 0;
    public String thisSentences;
    public boolean Error = false;
    public boolean segmentStart = false;
    public boolean segmentEnd = false;
    List<Token> tokens;

    public void setOffset(int offset) {
        this.offset = offset;
    }

    public Sentences(List<Token> sentences, int index, String line) {
        tokens = sentences;
        indexOfString = index;
        thisSentences = line;
        if (sentences.size() != 0) {
            if (sentences.get(0).type.equals(TokenType.Instruction) ||
                    sentences.get(0).type.equals(TokenType.JmpWord) ||
                    sentences.get(0).type.equals(TokenType.JncWord)) {
                Instruction instruction = new Instruction(index, sentences);
                offset = instruction.offset;
                Error = instruction.Error;
            }

            if (sentences.get(0).type.equals(TokenType.Identifier)) {
                Identifier identifier = new Identifier(index, sentences);
                offset = identifier.offset;
                Error = identifier.Error;
                if (identifier.identifier == TokenType.Identifier) segmentStart = true;
                if (identifier.identifier == TokenType.IdentifierEnd) segmentEnd = true;
            }

            if (sentences.get(0).type.equals(TokenType.EndWord)) {
                offset = 0;
            }

            if (sentences.get(0).type.equals(TokenType.Unknown)){
                offset = 0;
                Error = true;
            }
        }
    }

        public Sentences(int offset, List<Token> sentences, int index, String line){
            tokens = sentences;
            indexOfString = index;
            thisSentences = line;
            this.offset = offset;
            if(sentences.size() != 0) {
                if (sentences.get(0).type.equals(TokenType.Instruction) ||
                        sentences.get(0).type.equals(TokenType.JmpWord) ||
                        sentences.get(0).type.equals(TokenType.JncWord)) {
                    Instruction instruction = new Instruction(index, sentences);
                    Error = instruction.Error;
                }

                if (sentences.get(0).type.equals(TokenType.Identifier)) {
                    Identifier identifier = new Identifier(index, sentences);
                    Error = identifier.Error;
                    if(identifier.identifier == TokenType.Identifier) segmentStart = true;
                    if(identifier.identifier == TokenType.IdentifierEnd) segmentEnd = true;
                }

                if (sentences.get(0).type.equals(TokenType.EndWord)) {
                    offset = -1;
                }
            }
        }

    public int getOffset() {
        return offset;
    }

    @Override
    public String toString() {
        return "Offset " + String.format("%08X", offset) + " |Index " + String.format("%02d", indexOfString) + " |sentences " + thisSentences  + "\n";
    }
}
