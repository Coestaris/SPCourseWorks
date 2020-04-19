package course.work.sp;

import course.work.sp.sourcefile.DownloadFile;
import course.work.sp.tokenizer.Token;
import course.work.sp.tokenizer.TokenType;

import java.util.ArrayList;
import java.util.List;

public class NewFileParser {

    public static List<NewSentence> newFileParser(String filePath) {
        List<Token> arrayToken;
        List<NewSentence> newSentenceList = new ArrayList<>();
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
                    if(c == ':') token.append(c);
                    createToken(arrayToken, token.toString());
                    createToken(arrayToken, c + "");
                    token = new StringBuilder();
                }
            }
            createToken(arrayToken, token.toString());
            newSentenceList.add(createSentence(arrayToken, lines[index], index));
            index++;
            //fileToken.add(arrayToken);
        }

        return newSentenceList;
    }

    private static boolean contains(char ch) {
        return ch == ' ' || ch == '[' || ch == ']' || ch == '+' || ch == ',' || ch == ':';
    }

    private static void createToken(List<Token> arrayToken, String string) {
        string = string.trim();
        if (!string.isEmpty()) {
            arrayToken.add(new Token(string));
        }
    }

    private static NewSentence createSentence(List<Token> arrayToken, String string, int number){
        Token identifier;
        Token instruction;
        List<Operand> operands;
        boolean error = true;
        for (int index = 0; index < arrayToken.size(); index++){
            if(index == 0 && arrayToken.get(index).equals(TokenType.Label) ||
                             arrayToken.get(index).equals(TokenType.Identifier)){
                identifier = arrayToken.get(index);
                if(arrayToken.get(index).equals(TokenType.Label)) index++;
            }else {
                
            }
        }

        return null;
    }
}
