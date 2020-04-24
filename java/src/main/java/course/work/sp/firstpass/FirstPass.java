package course.work.sp.firstpass;

import course.work.sp.fileparser.NewSentence;
import course.work.sp.identifierstorage.*;
import course.work.sp.tokenizer.TokenType;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class FirstPass {

    private List<FirstPassSentence> firstPassSentenceList;

    public FirstPass(List<NewSentence> sentences) {
        initFirstPassSentenceList(sentences);
        setOffsetForConstant();
        setOffsetForLabel();
        setOffsetForSegment();
    }

    private void initFirstPassSentenceList(List<NewSentence> sentences) {
        Set<Integer> availableNumber = new HashSet<>();
        for (Segment sg : IdentifierStore.getInstance().getSegmentList()) {
            for (int i = sg.getIndexStart(); i <= sg.getIndexFinish(); i++) {
                availableNumber.add(i);
            }
        }

        firstPassSentenceList = new ArrayList<>();
        int preOffset = 0;
        for (NewSentence nw : sentences) {
            if (availableNumber.contains(nw.getNumber())) {
                firstPassSentenceList.add(new FirstPassSentence(nw, preOffset));
                preOffset += firstPassSentenceList.get(nw.getNumber()).getSize();
            } else {
                if (!(nw.getInstruction().equals(TokenType.KeyWord) ||
                        nw.getInstruction().equals(TokenType.EndWord) ||
                        nw.getInstruction().equals(TokenType.Empty)
                )) nw.setError(true);
                preOffset = 0;
                firstPassSentenceList.add(new FirstPassSentence(nw, preOffset));
            }
        }
    }

    private void setOffsetForConstant() {
        for (Constant constant : IdentifierStore.getInstance().getConstantList()) {
            int index = constant.getIndex();
            constant.setOffset(firstPassSentenceList.get(index).getOffset());
        }
    }

    private void setOffsetForLabel() {
        for (Label label : IdentifierStore.getInstance().getLabelList()) {
            int index = label.getIndex();
            label.setOffset(firstPassSentenceList.get(index).getOffset());
        }
    }

    private void setOffsetForSegment() {
        for (Segment segment : IdentifierStore.getInstance().getSegmentList()) {
            int index = segment.getIndexFinish();
            segment.setOffSet(firstPassSentenceList.get(index).getOffset());
            for (Constant constant : IdentifierStore.getInstance().getConstantList()) {
                if (constant.getIndex() > segment.getIndexStart() && constant.getIndex() < segment.getIndexFinish())
                    constant.setSegment(segment.getSegment());
            }

            for (Label label : IdentifierStore.getInstance().getLabelList()) {
                if (label.getIndex() > segment.getIndexStart() && label.getIndex() < segment.getIndexFinish())
                    label.setSegment(segment.getSegment());
            }
        }
    }

    public List<FirstPassSentence> getFirstPassSentenceList() {
        return firstPassSentenceList;
    }

    @Override
    public String toString() {
        StringBuilder pass = new StringBuilder();
        pass.append("index|").append(" Offset| ").append("       Source\n");
        for (FirstPassSentence fps : firstPassSentenceList)
            pass.append(fps.toString()).append("\n");

        pass.append(IdentifierStore.getInstance().toString());
        return pass.toString();
    }
}
