package course.work.sp;

import java.util.ArrayList;
import java.util.List;

public class FileParser {

    private static void createToken(ArrayList<Token> arrayToken, String string){
        string = string.trim();
        if(!string.isEmpty()){
            arrayToken.add(new Token(string));
        }

    }

    public static void fileParser(){
        String file = DownloadFile.downloadFile("D:\\Repos\\sp_kurs\\java\\src\\main\\resources\\testFile.asm");
        file = file.toUpperCase();
        String [] lines = file.split("\n");
        for (String line : lines) {
            ArrayList<Token> arrayToken = new ArrayList<Token>();
            char [] charLine = line.toCharArray();
            String token = "";
            for(int i = 0; i < charLine.length; i++){
                if(!contains(charLine[i])) {
                    token += charLine[i];
                } else {
                    createToken(arrayToken, token);
                    createToken(arrayToken, charLine[i] + "");
                    token = "";
                }
            }
            createToken(arrayToken, token);
            System.out.println();
            print(arrayToken, line);
            arrayToken.clear();
        }
    }

    public static boolean contains(char ch){
        return ch == ' ' || ch == '[' || ch == ']' || ch == '+' || ch == ',' || ch == ':';
    }

    public static void print(ArrayList<Token> arrayToken, String string){
        if(!arrayToken.isEmpty()) {
            System.out.println(string);
            System.out.print("[");
            for (Token tokenArrayList : arrayToken) {
                System.out.print("(\"" + tokenArrayList.stringToken + "\"" + ":" + tokenArrayList.Type + ":" + tokenArrayList.stringToken.length() + ") ");
            }
            System.out.print("]");
        }

    }



}
