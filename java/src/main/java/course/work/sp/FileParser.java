package course.work.sp;

import java.util.ArrayList;
import java.util.List;

public class FileParser {

    private static void createToken(ArrayList<Token> arrayToken, String string) {
        string = string.trim();
        if (!string.isEmpty()) {
            arrayToken.add(new Token(string));
        }
    }

    public static void fileParser() {
        String file = DownloadFile.downloadFile("D:\\Repos\\sp_kurs\\java\\src\\main\\resources\\testFile.asm");
        file = file.toUpperCase();
        String[] lines = file.split("\n");
        for (String line : lines) {
            ArrayList<Token> arrayToken = new ArrayList<Token>();
            char[] charLine = line.toCharArray();
            StringBuilder token = new StringBuilder();
            for (char c : charLine) {
                if (!contains(c)) {
                    token.append(c);
                } else {
                    createToken(arrayToken, token.toString());
                    createToken(arrayToken, c + "");
                    token = new StringBuilder();
                }
            }
            createToken(arrayToken, token.toString());
            System.out.println();
            print(arrayToken, line);
            arrayToken.clear();
        }
    }

    public static boolean contains(char ch) {
        return ch == ' ' || ch == '[' || ch == ']' || ch == '+' || ch == ',' || ch == ':';
    }

    public static void print(List<Token> arrayToken, String string) {
        if (!arrayToken.isEmpty()) {
            int i = 0;
            System.out.println(string);
            for (Token tokenArrayList : arrayToken) {
                System.out.println((i++) + " | \"" + tokenArrayList.stringToken + "\" " + " : " + tokenArrayList.type + " : " + tokenArrayList.stringToken.length() + " | ");
            }
        }
    }

    public static void syntax(ArrayList<Token> arrayToken) {

    }

}
