package course.work.sp.identifierstorage;

import course.work.sp.tokenizer.TokenType;

public class AssumeRegister {

    private byte prefixCode;
    private TokenType tokenType;
    private String destination;

    public AssumeRegister(byte prefixCode, TokenType tokenType, String destination) {
        this.prefixCode = prefixCode;
        this.tokenType = tokenType;
        this.destination = destination;
    }

    public byte getPrefixCode() {
        return prefixCode;
    }

    public TokenType getTokenType() {
        return tokenType;
    }

    public String getDestination() {
        return destination;
    }

    public void setDestination(String destination) {
        this.destination = destination;
    }
}
