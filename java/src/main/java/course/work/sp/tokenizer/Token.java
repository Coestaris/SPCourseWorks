package course.work.sp.tokenizer;

import java.util.HashMap;
import java.util.Map;
import java.util.regex.Pattern;


public class Token {

    private static final Map<String, TokenType> NAME_TO_TOKEN_TYPE_MAP;
    static {
        NAME_TO_TOKEN_TYPE_MAP = new HashMap<>();
        NAME_TO_TOKEN_TYPE_MAP.put("SEGMENT", TokenType.SegmentWord);
        NAME_TO_TOKEN_TYPE_MAP.put("DB", TokenType.DbDir);
        NAME_TO_TOKEN_TYPE_MAP.put("DW", TokenType.DwDir);
        NAME_TO_TOKEN_TYPE_MAP.put("DD", TokenType.DdDir);
        NAME_TO_TOKEN_TYPE_MAP.put("ASSUME", TokenType.KeyWord);
        NAME_TO_TOKEN_TYPE_MAP.put("STC", TokenType.Instruction);
        NAME_TO_TOKEN_TYPE_MAP.put("PUSH", TokenType.Instruction);
        NAME_TO_TOKEN_TYPE_MAP.put("JMP", TokenType.JmpWord);
        NAME_TO_TOKEN_TYPE_MAP.put("JNC", TokenType.JncWord);
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
        NAME_TO_TOKEN_TYPE_MAP.put(",", TokenType.Comma);
        NAME_TO_TOKEN_TYPE_MAP.put("DWORD", TokenType.DwordPtr);
        NAME_TO_TOKEN_TYPE_MAP.put("PTR", TokenType.Ptr);
        NAME_TO_TOKEN_TYPE_MAP.put("WORD", TokenType.WordPtr);
        NAME_TO_TOKEN_TYPE_MAP.put("BYTE", TokenType.BytePtr);
        NAME_TO_TOKEN_TYPE_MAP.put("EAX", TokenType.Reg32);
        NAME_TO_TOKEN_TYPE_MAP.put("EBP", TokenType.Reg32);
        NAME_TO_TOKEN_TYPE_MAP.put("EBX", TokenType.Reg32);
        NAME_TO_TOKEN_TYPE_MAP.put("EDX", TokenType.Reg32);
        NAME_TO_TOKEN_TYPE_MAP.put("ESI", TokenType.Reg32);
        NAME_TO_TOKEN_TYPE_MAP.put("EDI", TokenType.Reg32);
        NAME_TO_TOKEN_TYPE_MAP.put("ECX", TokenType.Reg32);
        NAME_TO_TOKEN_TYPE_MAP.put("DX", TokenType.Reg16);
        NAME_TO_TOKEN_TYPE_MAP.put("SI", TokenType.Reg16);
        NAME_TO_TOKEN_TYPE_MAP.put("BX", TokenType.Reg16);
        NAME_TO_TOKEN_TYPE_MAP.put("AX", TokenType.Reg16);
        NAME_TO_TOKEN_TYPE_MAP.put("DI", TokenType.Reg16);
        NAME_TO_TOKEN_TYPE_MAP.put("BP", TokenType.Reg16);
        NAME_TO_TOKEN_TYPE_MAP.put("ES:", TokenType.SegmentRegister);
        NAME_TO_TOKEN_TYPE_MAP.put("CS:", TokenType.SegmentRegister);
        NAME_TO_TOKEN_TYPE_MAP.put("DS:", TokenType.SegmentRegister);
        NAME_TO_TOKEN_TYPE_MAP.put("FS:", TokenType.SegmentRegister);
        NAME_TO_TOKEN_TYPE_MAP.put("SS:", TokenType.SegmentRegister);
        NAME_TO_TOKEN_TYPE_MAP.put("GS:", TokenType.SegmentRegister);
    }

    private static final Pattern numberHexRegex = Pattern.compile("^[0-9A-F]+H$");
    private static final Pattern numberDecRegex = Pattern.compile("^[0-9]+$");
    private static final Pattern numberBinRegex = Pattern.compile("^[01]+B$");
    private static final Pattern identifierRegex = Pattern.compile("^[A-Z]\\w*$");
    private static final Pattern labelRegex = Pattern.compile("^[A-Z]\\w*+:$");

    public TokenType type;
    public String stringToken;

    public TokenType getType() {
        return type;
    }

    public void setType(TokenType type) {
        this.type = type;
    }

    public String getStringToken() {
        return stringToken;
    }

    public void setStringToken(String stringToken) {
        this.stringToken = stringToken;
    }

    public Token(boolean empty){
        if (empty) type = TokenType.Empty;
     }
    public Token(){
        type = TokenType.Empty;
    }

    public Token(String string) {
        stringToken = string;
        if (NAME_TO_TOKEN_TYPE_MAP.containsKey(string)) {
            type = NAME_TO_TOKEN_TYPE_MAP.get(string);
            if(type == TokenType.SegmentRegister)
                stringToken = string.substring(0, string.length() - 1);
        } else if (numberHexRegex.matcher(string).matches()) {
            type = TokenType.HexNumber;
        } else if (numberDecRegex.matcher(string).matches()) {
            type = TokenType.DecNumber;
        } else if (numberBinRegex.matcher(string).matches()) {
            type = TokenType.BinNumber;
        } else if (identifierRegex.matcher(string).matches()) {
            type = TokenType.Identifier;
        } else if (labelRegex.matcher(string).matches()) {
            type = TokenType.Label;
            stringToken = string.substring(0, string.length() - 1);
        } else if (string.trim().startsWith("\"") && string.trim().endsWith("\"")) {
            type = TokenType.Text;
        }else {
            type = TokenType.Unknown;
        }

        if (type == TokenType.Text)
            stringToken = string.substring(1, string.length() - 1);
    }

    public boolean equals(TokenType type) {
        return type == this.type;
    }

    @Override
    public String toString() {
        return "Token{" +
                "Type=" + type +
                ", stringToken='" + stringToken + '\'' +
                '}';
    }
}
