package course.work.sp;

import jdk.nashorn.internal.runtime.regexp.joni.Regex;

import javax.swing.text.Segment;
import java.util.HashMap;
import java.util.Map;
import java.util.regex.Pattern;

public class Token {

    public static final Map<String, TokenType> tokenName;
    static{
        tokenName = new HashMap<String, TokenType>();
        tokenName.put("SEGMENT", TokenType.SegmentWord);
        tokenName.put("DB", TokenType.DbDir);
        tokenName.put("DW", TokenType.DwDir);
        tokenName.put("DD", TokenType.DdDir);
        tokenName.put("ASSUME", TokenType.KeyWord);
        tokenName.put("STC", TokenType.Instruction);
        tokenName.put("PUSH", TokenType.Instruction);
        tokenName.put("JMP", TokenType.Instruction);
        tokenName.put("JNC", TokenType.Instruction);
        tokenName.put("MOV", TokenType.Instruction);
        tokenName.put("MUL", TokenType.Instruction);
        tokenName.put("XOR", TokenType.Instruction);
        tokenName.put("BTR", TokenType.Instruction);
        tokenName.put("SUB", TokenType.Instruction);
        tokenName.put("ADC", TokenType.Instruction);
        tokenName.put("END", TokenType.EndWord);
        tokenName.put("ENDS", TokenType.EndsWord);
        tokenName.put("[", TokenType.Symbol);
        tokenName.put("]", TokenType.Symbol);
        tokenName.put("+", TokenType.Symbol);
        tokenName.put(":", TokenType.Symbol);
        tokenName.put(",", TokenType.Symbol);
        tokenName.put("DWORD", TokenType.DwordWord);
        tokenName.put("PTR", TokenType.PtrWord);
        tokenName.put("BYTE", TokenType.ByteWord);
        tokenName.put("EAX", TokenType.Reg32);
        tokenName.put("EBX", TokenType.Reg32);
        tokenName.put("DX", TokenType.Reg16);
        tokenName.put("SI", TokenType.Reg16);
        tokenName.put("ECX", TokenType.Reg32);
        tokenName.put("BX", TokenType.Reg16);
        tokenName.put("DI", TokenType.Reg16);
        tokenName.put("BP", TokenType.Reg16);
        tokenName.put("ES", TokenType.SegmentRegister);
        tokenName.put("CS", TokenType.SegmentRegister);
        tokenName.put("DS", TokenType.SegmentRegister);
        tokenName.put("FS", TokenType.SegmentRegister);
        tokenName.put("21H", TokenType.HexNumber);
        tokenName.put("12", TokenType.DecNumber);
        tokenName.put("0111110B", TokenType.BinNumber);
        tokenName.put("string", TokenType.Text);
        tokenName.put("2", TokenType.DecNumber);
        tokenName.put("1", TokenType.DecNumber);
        tokenName.put("8", TokenType.DecNumber);
        tokenName.put("LABLE1", TokenType.Label);
        tokenName.put("LABLE2", TokenType.Label);
        tokenName.put("07", TokenType.Imm8);
        tokenName.put("05", TokenType.Imm8);
    }
    private static Pattern _numberHexRegex = Pattern.compile("^[0-9a-f]+h$");
    private static Pattern _numberDecRegex = Pattern.compile("^[0-9]+$");
    private static Pattern _numberBinRegex = Pattern.compile("^[01]+b$");
    private static Pattern _identifierRegex = Pattern.compile("^[a-z]\\w*$");

    public Token(){
        //System.out.println( _numberBinRegex.matcher("10b").matches());
        String file = DownloadFile.downloadFile("D:\\javaProject\\course-workSP\\src\\\\main\\resources\\testFile.asm");
        file = file.toUpperCase();
        String [] lines = file.split("\n");
        for (String line : lines) {
            char [] charLine = line.toCharArray();
            String token = "";
            for(int i = 0; i < charLine.length; i++){
                if(!contains(charLine[i])) {
                    token += charLine[i];
                } else {
                    System.out.println(token);
                    System.out.println(charLine[i]);
                    token = "";
                }
            }

        }
    }

    public boolean contains(char ch){
        return ch == ' ' || ch == '[' || ch == ']' || ch == '+' || ch == ',' || ch == ':';
    }
}
