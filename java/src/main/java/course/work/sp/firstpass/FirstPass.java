package course.work.sp.firstpass;

import course.work.sp.fileparser.NewSentence;
import course.work.sp.identifierstorage.*;
import course.work.sp.tokenizer.TokenType;

import java.util.ArrayList;
import java.util.List;

public class FirstPass {

    public List<FirstPassSentence> firstPass(List<NewSentence> sentences) {
        List<FirstPassSentence> firstPassSentenceList = new ArrayList<>();
        List<Integer> availableNumber = new ArrayList<>();
        for(Segment sg: IdentifierStore.getInstance().getSegmentList()){
            for(int i = sg.getIndexStart(); i <= sg.getIndexFinish(); i++){
                    availableNumber.add(i);
            }
        }

        int preOffset = 0;
        for (NewSentence nw: sentences){
            if(availableNumber.contains(nw.getNumber())){
                firstPassSentenceList.add(new FirstPassSentence(nw, preOffset));
                preOffset += firstPassSentenceList.get(nw.getNumber()).getSize();
            }else {
                if(!(nw.getInstruction().equals(TokenType.KeyWord) ||
                        nw.getInstruction().equals(TokenType.EndWord) ||
                        nw.getInstruction().equals(TokenType.Empty)
                        )) nw.setError(true);
                preOffset  = 0;
                firstPassSentenceList.add(new FirstPassSentence(nw, preOffset));
            }
        }
        return firstPassSentenceList;
    }


}
