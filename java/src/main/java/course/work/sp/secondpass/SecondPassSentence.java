package course.work.sp.secondpass;

import course.work.sp.firstpass.FirstPassSentence;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;

public class SecondPassSentence {

    private FirstPassSentence firstPassSentence;
    private List<Byte> machineCode;

    @Override
    public String toString() {
        StringBuilder pass = new StringBuilder();
        pass.append(String.format("%2d ", firstPassSentence.getNewSentence().getNumber())).append(" ").append(String.format("%08X ", firstPassSentence.getOffset()));
        if(machineCode != null)
            for(Byte mc: machineCode)
            pass.append(String.format("%02X ", mc));
         pass.append(firstPassSentence.getNewSentence().getLine());
         if(firstPassSentence.getNewSentence().isError()) pass.append("  ERROR");
        return pass.toString();
    }

    public SecondPassSentence(FirstPassSentence firstPassSentence) {
        this.firstPassSentence = firstPassSentence;
        if(firstPassSentence.getNewSentence().isError() || firstPassSentence.getSize() == 0){
            machineCode = new ArrayList<>();
        }else {
            machineCode = new SetMachineCode().setMachineCode(firstPassSentence);
        }
        //TODO calculate machineCode
    }
}
