package course.work.sp.secondpass;

import course.work.sp.firstpass.FirstPassSentence;
import course.work.sp.identifierstorage.IdentifierStore;

import java.util.ArrayList;
import java.util.List;

public class SecondPass {
    private List<SecondPassSentence> secondPassSentenceList;

    public SecondPass(List<FirstPassSentence> firstPassSentenceList) {
        secondPassSentenceList = new ArrayList<>();
        for (FirstPassSentence fps : firstPassSentenceList) {
            secondPassSentenceList.add(new SecondPassSentence(fps));
        }
    }

    @Override
    public String toString() {
        StringBuilder pass = new StringBuilder();
        pass.append("index| ").append("Offset|").append("\n");
        for (SecondPassSentence sps : secondPassSentenceList)
            pass.append(sps.toString()).append("\n");

        pass.append(IdentifierStore.getInstance().toString());
        return pass.toString();
    }
}
