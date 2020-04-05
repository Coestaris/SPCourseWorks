package course.work.sp;

import java.net.BindException;
import java.util.List;

public class Sentences {
    public int indexOfString;
    public int offset = 0;
    public String thisSentences;
    public Sentences(int preOffset, int index, String[] lines){
        indexOfString = index;
        thisSentences = lines[index];


    }

    public int getOffset() {
        return offset;
    }
}
