package course.work.sp.segmentRegister;

import course.work.sp.tokenizer.TokenType;

public class SegmentRegister {

    private byte prefixCode;
    private TokenType tokenType;
    private String destination;

    public SegmentRegister(byte prefixCode, TokenType tokenType, String destination) {
        this.prefixCode = prefixCode;
        this.tokenType = tokenType;
        this.destination = destination;
    }

    public byte getPrefixCode() {
        return prefixCode;
    }

    public void setPrefixCode(byte prefixCode) {
        this.prefixCode = prefixCode;
    }

    public TokenType getTokenType() {
        return tokenType;
    }

    public void setTokenType(TokenType tokenType) {
        this.tokenType = tokenType;
    }

    public String getDestination() {
        return destination;
    }

    public void setDestination(String destination) {
        this.destination = destination;
    }
}
