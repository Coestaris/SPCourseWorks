package course.work.sp.fileparser;

import course.work.sp.identifierstorage.IdentifierStore;
import course.work.sp.sourcefile.DownloadFile;
import course.work.sp.tokenizer.Token;
import course.work.sp.tokenizer.TokenType;

import java.util.ArrayList;
import java.util.List;

public class NewFileParser {

    private List<NewSentence> newSentenceList;

    public NewFileParser(String filePath) {
        parsingFile(filePath);
    }

    private void parsingFile(String filePath) {
        newSentenceList = new ArrayList<>();
        List<Token> arrayToken;
        String file = DownloadFile.downloadFile(filePath);
        String[] lines = file.split("\n");
        int index = 0;
        for (String line : lines) {
            line = line.toUpperCase();
            arrayToken = new ArrayList<>();
            char[] charLine = line.toCharArray();
            StringBuilder token = new StringBuilder();
            for (char c : charLine) {
                if (!contains(c)) {
                    token.append(c);
                } else {
                    if (c == ':') token.append(c);
                    createToken(arrayToken, token.toString());
                    createToken(arrayToken, c + "");
                    token = new StringBuilder();
                }
            }
            createToken(arrayToken, token.toString());
            newSentenceList.add(createSentence(arrayToken, lines[index], index));
            index++;
        }
    }

    public List<NewSentence> getNewSentenceList() {
        return newSentenceList;
    }

    private boolean contains(char ch) {
        return ch == ' ' || ch == '[' || ch == ']' || ch == '+' || ch == ',' || ch == ':';
    }

    private void createToken(List<Token> arrayToken, String string) {
        string = string.trim();
        if (!string.isEmpty()) {
            arrayToken.add(new Token(string));
        }
    }

    private NewSentence createSentence(List<Token> arrayToken, String string, int number) {

        Token identifier = new Token();
        Token instruction = new Token();
        List<Token> operand = new ArrayList<>();
        List<Operand> operands = new ArrayList<>();
        if (arrayToken.size() > 0) {
            for (int index = 0; index < arrayToken.size(); index++) {
                if (index == 0 && (arrayToken.get(index).equals(TokenType.Label) ||
                        arrayToken.get(index).equals(TokenType.Identifier))) {
                    identifier = arrayToken.get(index);
                    identifier.setType(IdentifierStore.getInstance().addIdentifierStore(arrayToken, number));
                    if (identifier.equals(TokenType.Unknown))
                        return new NewSentence(number, identifier, instruction, operands, string);
                    if (arrayToken.get(index).equals(TokenType.Label)) index++;
                } else {
                    if (arrayToken.get(index).equals(TokenType.Instruction) ||
                            arrayToken.get(index).equals(TokenType.SegmentWord) ||
                            arrayToken.get(index).equals(TokenType.DbDir) ||
                            arrayToken.get(index).equals(TokenType.DwDir) ||
                            arrayToken.get(index).equals(TokenType.DdDir) ||
                            arrayToken.get(index).equals(TokenType.KeyWord) ||
                            arrayToken.get(index).equals(TokenType.EndWord) ||
                            arrayToken.get(index).equals(TokenType.EndsWord) ||
                            arrayToken.get(index).equals(TokenType.JmpWord) ||
                            arrayToken.get(index).equals(TokenType.JncWord)) {
                        instruction = arrayToken.get(index);
                    } else {
                        if (arrayToken.get(index).equals(TokenType.Comma) || index == arrayToken.size() - 1) {
                            if (index == arrayToken.size() - 1) operand.add(arrayToken.get(index));
                            operands.add(new Operand(operand));
                            operand = new ArrayList<>();
                        } else
                            operand.add(arrayToken.get(index));
                    }
                }
            }
        } else {
            return new NewSentence(number, new Token(true), new Token(true), operands, string);
        }
        return new NewSentence(number, identifier, instruction, operands, string);
    }
}
