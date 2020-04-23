package course.work.sp.identifierstorage;

import course.work.sp.tokenizer.Token;
import course.work.sp.tokenizer.TokenType;

import java.util.ArrayList;
import java.util.List;

public class IdentifierStore {

    private List<Constant> constantList;
    private List<Label> labelList;
    private List<Segment> segmentList;

    private static IdentifierStore instance;

    private IdentifierStore(){
        constantList = new ArrayList<>();
        labelList = new ArrayList<>();
        segmentList = new ArrayList<>();
    }

    public static IdentifierStore getInstance(){
        if (instance == null) {
            instance = new IdentifierStore();
        }
        return instance;
    }

    public TokenType addIdentifierStore(List<Token> tokens, int number){
        int index = 0;
        final int next = 1;
        if(tokens.get(index).equals(TokenType.Label)) {
            for(Label lbl : labelList){
                if(lbl.getLabel().equals(tokens.get(index).getStringToken())) return TokenType.Unknown;
            }
            labelList.add(new Label(number, tokens.get(index).getStringToken()));
            return TokenType.Label;
        }
        if(tokens.get(index).equals(TokenType.Identifier) && tokens.get(index + next).equals(TokenType.SegmentWord)) {
            segmentList.add(new Segment(tokens.get(index).getStringToken(), TokenType.SegmentIdentifier, number));
            return TokenType.SegmentIdentifier;
        }
        if(tokens.get(index).equals(TokenType.Identifier) && tokens.get(index + next).equals(TokenType.EndsWord)){
            for (Segment sql: segmentList)
                if(sql.equalSegment(tokens.get(index).getStringToken())){
                    sql.setIndexFinish(number);
                    return TokenType.SegmentIdentifier;
                }
        }
        if(tokens.get(index).equals(TokenType.Identifier) && ( tokens.get(index + next).equals(TokenType.DbDir) ||
                tokens.get(index + next).equals(TokenType.DwDir)||
                tokens.get(index + next).equals(TokenType.DdDir))){
            if(tokens.get(index + next).equals(TokenType.DbDir)) {
                constantList.add(new Constant(number, tokens.get(index).getStringToken(), TokenType.DbIdentifier));
                return TokenType.DbIdentifier;
            }
            if(tokens.get(index + next).equals(TokenType.DwDir)) {
                constantList.add(new Constant(number, tokens.get(index).getStringToken(), TokenType.DwIdentifier));
                return TokenType.DwIdentifier;
            }
            if(tokens.get(index + next).equals(TokenType.DdDir)) {
                constantList.add(new Constant(number, tokens.get(index).getStringToken(), TokenType.DdIdentifier));
                return TokenType.DdIdentifier;
            }
        }
        return TokenType.Unknown;
    }

    public List<Constant> getConstantList() {
        return constantList;
    }

    public List<Label> getLabelList() {
        return labelList;
    }

    public List<Segment> getSegmentList() {
        return segmentList;
    }

    @Override
    public String toString() {
        StringBuilder pass = new StringBuilder();
        pass.append("\n\n");
        pass.append(" NAME |").append("   TYPE  | ").append(" SEG |").append(" VALUE ").append("\n");
        for (Label lb: labelList)
            pass.append(lb);
        for (Constant cost: constantList)
            pass.append(cost);
        pass.append("\n\n");
        pass.append("Segment name|").append(" Bit Depth|").append(" Size").append("\n");
        for (Segment sql: segmentList)
            pass.append(sql);

        pass.append("\n\n");
        pass.append(AssumeRegisterStorage.getInstance().toString());

        return pass.toString();
    }
}
