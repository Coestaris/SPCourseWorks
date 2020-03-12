package course.work.sp;

import java.util.HashMap;
import java.util.Map;
import java.util.regex.Pattern;


public class Token {

    public static final Map<String, TokenType> NAME_TO_TOKEN_TYPE_MAP;
    static {
        NAME_TO_TOKEN_TYPE_MAP = new HashMap<String, TokenType>();
        NAME_TO_TOKEN_TYPE_MAP.put("SEGMENT", TokenType.SegmentWord);
        NAME_TO_TOKEN_TYPE_MAP.put("DB", TokenType.DbDir);
        NAME_TO_TOKEN_TYPE_MAP.put("DW", TokenType.DwDir);
        NAME_TO_TOKEN_TYPE_MAP.put("DD", TokenType.DdDir);
        NAME_TO_TOKEN_TYPE_MAP.put("ASSUME", TokenType.KeyWord);
        NAME_TO_TOKEN_TYPE_MAP.put("STC", TokenType.Instruction);
        NAME_TO_TOKEN_TYPE_MAP.put("PUSH", TokenType.Instruction);
        NAME_TO_TOKEN_TYPE_MAP.put("JMP", TokenType.Instruction);
        NAME_TO_TOKEN_TYPE_MAP.put("JNC", TokenType.Instruction);
        NAME_TO_TOKEN_TYPE_MAP.put("MOV", TokenType.Instruction);
        NAME_TO_TOKEN_TYPE_MAP.put("MUL", TokenType.Instruction);
        NAME_TO_TOKEN_TYPE_MAP.put("XOR", TokenType.Instruction);
        NAME_TO_TOKEN_TYPE_MAP.put("BTR", TokenType.Instruction);
        NAME_TO_TOKEN_TYPE_MAP.put("SUB", TokenType.Instruction);
        NAME_TO_TOKEN_TYPE_MAP.put("ADC", TokenType.Instruction);
        NAME_TO_TOKEN_TYPE_MAP.put("END", TokenType.EndWord);
        NAME_TO_TOKEN_TYPE_MAP.put("ENDS", TokenType.EndsWord);
        NAME_TO_TOKEN_TYPE_MAP.put("[", TokenType.Symbol);
        NAME_TO_TOKEN_TYPE_MAP.put("]", TokenType.Symbol);
        NAME_TO_TOKEN_TYPE_MAP.put("+", TokenType.Symbol);
        NAME_TO_TOKEN_TYPE_MAP.put(":", TokenType.Symbol);
        NAME_TO_TOKEN_TYPE_MAP.put(",", TokenType.Koma);
        NAME_TO_TOKEN_TYPE_MAP.put("DWORD", TokenType.DwordWord);
        NAME_TO_TOKEN_TYPE_MAP.put("PTR", TokenType.PtrWord);
        NAME_TO_TOKEN_TYPE_MAP.put("BYTE", TokenType.ByteWord);
        NAME_TO_TOKEN_TYPE_MAP.put("EAX", TokenType.Reg32);
        NAME_TO_TOKEN_TYPE_MAP.put("EBX", TokenType.Reg32);
        NAME_TO_TOKEN_TYPE_MAP.put("DX", TokenType.Reg16);
        NAME_TO_TOKEN_TYPE_MAP.put("SI", TokenType.Reg16);
        NAME_TO_TOKEN_TYPE_MAP.put("ECX", TokenType.Reg32);
        NAME_TO_TOKEN_TYPE_MAP.put("BX", TokenType.Reg16);
        NAME_TO_TOKEN_TYPE_MAP.put("DI", TokenType.Reg16);
        NAME_TO_TOKEN_TYPE_MAP.put("BP", TokenType.Reg16);
        NAME_TO_TOKEN_TYPE_MAP.put("ES", TokenType.SegmentRegister);
        NAME_TO_TOKEN_TYPE_MAP.put("CS", TokenType.SegmentRegister);
        NAME_TO_TOKEN_TYPE_MAP.put("DS", TokenType.SegmentRegister);
        NAME_TO_TOKEN_TYPE_MAP.put("FS", TokenType.SegmentRegister);
    }

    private static Pattern numberHexRegex = Pattern.compile("^[0-9A-F]+H$");
    private static Pattern numberDecRegex = Pattern.compile("^[0-9]+$");
    private static Pattern numberBinRegex = Pattern.compile("^[01]+B$");
    private static Pattern identifierRegex = Pattern.compile("^[A-Z]\\w*$");

    public TokenType type;
    public String stringToken;

    public Token(String string) {
        stringToken = string;
        if (NAME_TO_TOKEN_TYPE_MAP.containsKey(string)) {
            type = NAME_TO_TOKEN_TYPE_MAP.get(string);
        } else if (numberHexRegex.matcher(string).matches()) {
            type = TokenType.HexNumber;
        } else if (numberDecRegex.matcher(string).matches()) {
            type = TokenType.DecNumber;
        } else if (numberBinRegex.matcher(string).matches()) {
            type = TokenType.BinNumber;
        } else if (identifierRegex.matcher(string).matches()) {
            type = TokenType.Identifier;
        } else if (string.trim().startsWith("\"") && string.trim().endsWith("\"")) {
            type = TokenType.Text;
        }

        if (type == TokenType.Text)
            stringToken = string.substring(1, string.length() - 1);
    }


    @Override
    public String toString() {
        return "Token{" +
                "Type=" + type +
                ", stringToken='" + stringToken + '\'' +
                '}';
    }
}
