package course.work.sp.secondpass;

import course.work.sp.identifierstorage.AssumeRegisterStorage;

import java.util.ArrayList;

import java.util.List;
import java.util.Set;

public class RegisterInfoStorage {
    private List<RegisterInfo> registerInfoList;
    private static RegisterInfoStorage instance;

    private RegisterInfoStorage() {
        registerInfoList = new ArrayList<>();
        registerInfoList.add(new RegisterInfo("EAX", (byte) 0x0));
        registerInfoList.add(new RegisterInfo("AX", (byte) 0x0));
        registerInfoList.add(new RegisterInfo("AL", (byte) 0x0));

        registerInfoList.add(new RegisterInfo("ECX", (byte) 0x1));
        registerInfoList.add(new RegisterInfo("CX", (byte) 0x1));
        registerInfoList.add(new RegisterInfo("CL", (byte) 0x1));

        registerInfoList.add(new RegisterInfo("EDX", (byte) 0x2));
        registerInfoList.add(new RegisterInfo("DX", (byte) 0x2));
        registerInfoList.add(new RegisterInfo("DL", (byte) 0x2));

        registerInfoList.add(new RegisterInfo("EBX", (byte) 0x3));
        registerInfoList.add(new RegisterInfo("BX", (byte) 0x3));
        registerInfoList.add(new RegisterInfo("BL", (byte) 0x3));

        registerInfoList.add(new RegisterInfo("ESP", (byte) 0x4));
        registerInfoList.add(new RegisterInfo("SP", (byte) 0x4));
        registerInfoList.add(new RegisterInfo("AH", (byte) 0x4));

        registerInfoList.add(new RegisterInfo("EBP", (byte) 0x5));
        registerInfoList.add(new RegisterInfo("BP", (byte) 0x5));
        registerInfoList.add(new RegisterInfo("CH", (byte) 0x5));

        registerInfoList.add(new RegisterInfo("ESI", (byte) 0x6));
        registerInfoList.add(new RegisterInfo("SI", (byte) 0x6));
        registerInfoList.add(new RegisterInfo("DH", (byte) 0x6));

        registerInfoList.add(new RegisterInfo("EDI", (byte) 0x7));
        registerInfoList.add(new RegisterInfo("DI", (byte) 0x7));
        registerInfoList.add(new RegisterInfo("BH", (byte) 0x7));
    }

    public static RegisterInfoStorage getInstance() {
        if (instance == null) {
            instance = new RegisterInfoStorage();
        }
        return instance;
    }

    public byte getByteReg(String regName) {
        for (RegisterInfo regInf : registerInfoList) {
            if (regInf.getReg().equals(regName)) return regInf.getOpcode();
        }
        return (byte) -1;
    }
}
