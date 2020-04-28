package course.work.sp.secondpass;

import course.work.sp.firstpass.FirstPassSentence;

import java.util.ArrayList;
import java.util.List;

public class SecondPass {
    private List<SecondPassSentence> secondPassSentenceList;

    public SecondPass(List <FirstPassSentence> firstPassSentenceList) {
        secondPassSentenceList = new ArrayList<>();
        for (FirstPassSentence fps: firstPassSentenceList){
            secondPassSentenceList.add(new SecondPassSentence(fps));
        }
    }

    @Override
    public String toString() {
        StringBuilder pass = new StringBuilder();
        for (SecondPassSentence sps: secondPassSentenceList)
            pass.append(sps.toString()).append("\n\n");
        return pass.toString();
    }
}
