package course.work.sp.identifierstorage;

import course.work.sp.fileparser.Operand;
import course.work.sp.tokenizer.TokenType;

import java.util.ArrayList;
import java.util.List;

public class AssumeRegisterStorage {

    private static final String NOTHING = "Nothing";

    private static AssumeRegisterStorage instance;


    private List<AssumeRegister> assumeRegisterList;

    private AssumeRegisterStorage() {
        assumeRegisterList = new ArrayList<>();
        assumeRegisterList.add(new AssumeRegister((byte) 0x3E, TokenType.DS, NOTHING));
        assumeRegisterList.add(new AssumeRegister((byte) 0x2E, TokenType.CS, NOTHING));
        assumeRegisterList.add(new AssumeRegister((byte) 0x26, TokenType.ES, NOTHING));
        assumeRegisterList.add(new AssumeRegister((byte) 0x36, TokenType.SS, NOTHING));
        assumeRegisterList.add(new AssumeRegister((byte) 0x64, TokenType.FS, NOTHING));
        assumeRegisterList.add(new AssumeRegister((byte) 0x65, TokenType.GS, NOTHING));
    }

    private void changeDestinationByTokenType(TokenType tokenType, String destination) {
        for (AssumeRegister sg : assumeRegisterList) {
            if (sg.getTokenType().equals(tokenType)) {
                sg.setDestination(destination);
            }
        }
    }

    public static AssumeRegisterStorage getInstance() {
        if (instance == null) {
            instance = new AssumeRegisterStorage();
        }
        return instance;
    }

    public void changeRegister(Operand operand) {
        int index = 0;
        int next = 1;
                switch (operand.getTokens().get(index).getStringToken()) {
                    case ("DS"):
                        changeDestinationByTokenType(TokenType.DS, operand.getTokens().get(index + next).getStringToken());
                        break;
                    case ("ES"):
                        changeDestinationByTokenType(TokenType.ES, operand.getTokens().get(index + next).getStringToken());
                        break;
                    case ("FS"):
                        changeDestinationByTokenType(TokenType.FS, operand.getTokens().get(index + next).getStringToken());
                        break;
                    case ("SS"):
                        changeDestinationByTokenType(TokenType.SS, operand.getTokens().get(index + next).getStringToken());
                        break;
                    case ("CS"):
                        changeDestinationByTokenType(TokenType.CS, operand.getTokens().get(index + next).getStringToken());
                        break;
                    case ("GS"):
                        changeDestinationByTokenType(TokenType.GS, operand.getTokens().get(index + next).getStringToken());
                        break;
                    default:
                        System.out.println("Unknown register");
                        break;
                }
        
    }

    public byte getSegmentRegisterByte(String segmentReg){
        TokenType typeReg = TokenType.Unknown;
        switch (segmentReg) {
            case ("DS"):
                typeReg = TokenType.DS;
                break;
            case ("ES"):
                typeReg = TokenType.ES;
                break;
            case ("FS"):
                typeReg = TokenType.FS;
                break;
            case ("SS"):
                typeReg = TokenType.SS;
                break;
            case ("CS"):
                typeReg = TokenType.CS;
                break;
            case ("GS"):
                typeReg = TokenType.GS;
                break;
            default:
                break;
        }
        for (AssumeRegister asr: assumeRegisterList){
            if(asr.getTokenType().equals(typeReg)) return asr.getPrefixCode();
        }
        return 0;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("Segment Register | Destination\n");
        for (AssumeRegister sr : assumeRegisterList){
            sb.append(sr.getTokenType()).append("    ").append(sr.getDestination()).append("\n");
        }
        return sb.toString();
    }
}
