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
    }
    private static Pattern _numberHexRegex = Pattern.compile("^[0-9A-F]+H$");
    private static Pattern _numberDecRegex = Pattern.compile("^[0-9]+$");
    private static Pattern _numberBinRegex = Pattern.compile("^[01]+B$");
    private static Pattern _identifierRegex = Pattern.compile("^[A-Z]\\w*$");


    public TokenType Type;
    public String stringToken;

    public Token(String string){
        stringToken = string;
        if(tokenName.containsKey(string)){
            Type = tokenName.get(string);
        }else

        if(_numberHexRegex.matcher(string).matches()){
            Type = TokenType.HexNumber;
        }else

        if(_numberDecRegex.matcher(string).matches()){
            Type = TokenType.DecNumber;
        } else

        if(_numberBinRegex.matcher(string).matches()){
            Type = TokenType.BinNumber;
        } else 

        if(_identifierRegex.matcher(string).matches()){
            Type = TokenType.Identifier;
        } else

        if (string.trim().startsWith("\"") && string.trim().endsWith("\"")){
             Type = TokenType.Text;
        }

        if(Type == TokenType.Text)
           stringToken = string.substring(1, string.length() - 1);


    }


    @Override
    public String toString() {
        return "Token{" +
                "Type=" + Type +
                ", stringToken='" + stringToken + '\'' +
                '}';
    }
}
