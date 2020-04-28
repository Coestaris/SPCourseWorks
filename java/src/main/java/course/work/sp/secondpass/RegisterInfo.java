package course.work.sp.secondpass;

import java.util.Objects;

public class RegisterInfo {
    private String reg;
    private byte opcode;

    public RegisterInfo(String reg, byte opcode) {
        this.reg = reg;
        this.opcode = opcode;
    }

    public String getReg() {
        return reg;
    }

    public byte getOpcode() {
        return opcode;
    }


}
