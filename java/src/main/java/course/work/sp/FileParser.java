package course.work.sp;

import java.util.ArrayList;
import java.util.List;

public class FileParser {

    private static void createToken(List<Token> arrayToken, String string) {
        string = string.trim();
        if (!string.isEmpty()) {
            arrayToken.add(new Token(string));
        }
    }

    public static void fileParser() {
        String file = DownloadFile.downloadFile("D:\\Repos\\sp_kurs\\java\\src\\main\\resources\\testFile.asm");
        List<List<Token>> fileToken= new ArrayList<>();
        file = file.toUpperCase();
        String[] lines = file.split("\n");
        for (String line : lines) {
            List<Token> arrayToken = new ArrayList<Token>();
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
            fileToken.add(arrayToken);
        }
        //lexicalAnalysis(fileToken, lines);
        print(lines, fileToken);
        //syntaxAnalysis(fileToken, lines);
    }

    public static boolean contains(char ch) {
        return ch == ' ' || ch == '[' || ch == ']' || ch == '+' || ch == ',' || ch == ':';
    }

    public static void lexicalAnalysis(List<List<Token>> fileToken/*, String [] line*/, int lineNumber) {
        if(fileToken != null) {
            //for(int lineNumber = 0; lineNumber < fileToken.size(); lineNumber++) {
                //System.out.println(line[lineNumber]);
                for (int tokenNumber = 0; tokenNumber < fileToken.get(lineNumber).size(); tokenNumber++){
                    if (fileToken.get(lineNumber).get(tokenNumber).type == TokenType.Unknown) {
                        System.out.println("ERROR: Unknown TokenType");
                    } else {
                        System.out.println(tokenNumber
                                + " | \"" + fileToken.get(lineNumber).get(tokenNumber).stringToken + "\" "
                                + " : " + fileToken.get(lineNumber).get(tokenNumber).type
                                + " : " + fileToken.get(lineNumber).get(tokenNumber).stringToken.length()
                                + " | ");
                    }
                }
            }
        //}
    }

    public static void syntaxAnalysis(List<List<Token>> fileToken/*,  String [] line*/, int lineNumber) {
        boolean isMnemCode = false;
        if(fileToken != null) {
            //for(int lineNumber = 0; lineNumber < fileToken.size(); lineNumber++){
                //System.out.println(line[lineNumber]);
                for (int tokenNumber = 0; tokenNumber < fileToken.get(lineNumber).size(); tokenNumber++) {
                    if(tokenNumber == 0 && fileToken.get(lineNumber).get(tokenNumber).type == TokenType.Identifier){
                        System.out.print("| " + tokenNumber + " |");
                    }else {
                        if (fileToken.get(lineNumber).get(tokenNumber).type == TokenType.Instruction ||
                                fileToken.get(lineNumber).get(tokenNumber).type == TokenType.JmpWord ||
                                fileToken.get(lineNumber).get(tokenNumber).type == TokenType.SegmentWord ||
                                fileToken.get(lineNumber).get(tokenNumber).type == TokenType.EndsWord ||
                                fileToken.get(lineNumber).get(tokenNumber).type == TokenType.EndWord ||
                                fileToken.get(lineNumber).get(tokenNumber).type == TokenType.DbDir ||
                                fileToken.get(lineNumber).get(tokenNumber).type == TokenType.DwDir ||
                                fileToken.get(lineNumber).get(tokenNumber).type == TokenType.DdDir ||
                                fileToken.get(lineNumber).get(tokenNumber).type == TokenType.KeyWord) {
                            if (tokenNumber == 0) System.out.print("| -- |");
                            System.out.print(" " + tokenNumber + " 1 |");
                            isMnemCode = true;
                        }
                        if (isMnemCode && (fileToken.get(lineNumber).get(tokenNumber).type == TokenType.Reg8 ||
                                fileToken.get(lineNumber).get(tokenNumber).type == TokenType.Reg16 ||
                                fileToken.get(lineNumber).get(tokenNumber).type == TokenType.Reg32 ||
                                fileToken.get(lineNumber).get(tokenNumber).type == TokenType.Identifier ||
                                fileToken.get(lineNumber).get(tokenNumber).type == TokenType.DwordWord ||
                                fileToken.get(lineNumber).get(tokenNumber).type == TokenType.PtrWord ||
                                fileToken.get(lineNumber).get(tokenNumber).type == TokenType.SegmentRegister ||
                                fileToken.get(lineNumber).get(tokenNumber).type == TokenType.ByteWord ||
                                fileToken.get(lineNumber).get(tokenNumber).type == TokenType.HexNumber ||
                                fileToken.get(lineNumber).get(tokenNumber).type == TokenType.BinNumber ||
                                fileToken.get(lineNumber).get(tokenNumber).type == TokenType.DecNumber ||
                                fileToken.get(lineNumber).get(tokenNumber).type == TokenType.Text)) {
                            int countOperands = 0;
                            for (int i = tokenNumber; i < fileToken.get(lineNumber).size(); i++) {
                                if (fileToken.get(lineNumber).get(i).type == TokenType.Comma) break;
                                countOperands++;
                            }
                            System.out.print(" " + tokenNumber + " " + countOperands + " |");
                            tokenNumber += countOperands;
                        }
                    }
                }
               // isMnemCode = false;
                System.out.println();
           // }
        }
    }
    public static void print(String[] line, List<List<Token>> fileToken){
        for (int i = 0; i < line.length; i++){

            System.out.println("Source: " + line[i]);
            System.out.println("Lexical Analysis :");
            lexicalAnalysis(fileToken, i);
            System.out.println("Syntax Analysis :");
            System.out.println("|lb1| mnem |  OP1  | OP2 |");
            syntaxAnalysis(fileToken, i);
            System.out.println();
        }
    }
}
