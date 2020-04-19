package course.work.sp;

import course.work.sp.tokenizer.Token;
import course.work.sp.tokenizer.TokenType;

import java.util.List;

public class Identifier {
    public int index;
    public boolean Error = false;
    public int offset = 0;
    public String token;
    public TokenType identifier;

    public Identifier(int index, List<Token> identifier){
        token = identifier.get(0).stringToken;
        this.index = index;
        if(identifier.get(1).type == TokenType.Symbol) {
            this.identifier = TokenType.Label;
        }else if(identifier.get(1).type == TokenType.DwDir){
            if(identifier.get(2).type == TokenType.Text){
                Error = true;
            }else{
                this.identifier = TokenType.Dw;
                offset += 2;
            }
        }else if(identifier.get(1).type == TokenType.DdDir) {
            if (identifier.get(2).type == TokenType.Text) {
                Error = true;
            } else {
                this.identifier = TokenType.Dd;
                offset += 4;
            }
        }else if(identifier.get(1).type == TokenType.DbDir){
            if (identifier.get(2).type == TokenType.Text) {
                offset += identifier.get(2).stringToken.length();
                this.identifier = TokenType.Dd;
            } else {
                this.identifier = TokenType.Dd;
                offset += 1;
            }
        }else if(identifier.get(1).type == TokenType.SegmentWord) this.identifier = TokenType.Identifier;
        else if(identifier.get(1).type == TokenType.EndsWord) this.identifier = TokenType.IdentifierEnd;
       // else if(identifier.get(0).type == TokenType.KeyWord) this.identifier = TokenType.KeyWord;
        else Error = true;
    }

    @Override
    public String toString() {
        return "Offset " + String.format("%08X", offset) + " |Index " + String.format("%02d", index) + " |Name " + token + " |Type " + identifier + "\n";
    }
}
