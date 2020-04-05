package course.work.sp;

import com.sun.corba.se.impl.oa.toa.TOA;

import java.util.ArrayList;
import java.util.List;

public class FirstPass {

    public static void SegmentDestination(List<List<Token>> fileToken){
            boolean isAssume = false;
            List<List<Token>> operand = new ArrayList<>();
            for (List<Token> tokens : fileToken) {
                for (int tokenNumber = 0; tokenNumber < tokens.size(); tokenNumber++) {
                    if (tokens.get(tokenNumber).type == TokenType.KeyWord) {
                        operand = new ArrayList<>();
                        List<Token> arrayOperand = new ArrayList<>();
                        for (int numberOfOperands = tokenNumber + 1; numberOfOperands < tokens.size(); numberOfOperands++) {
                            if (tokens.get(numberOfOperands).type != TokenType.Comma){
                                arrayOperand.add(tokens.get(numberOfOperands));
                            }else {
                                operand.add(arrayOperand);
                                arrayOperand = new ArrayList<>();
                            }
                        }
                        operand.add(arrayOperand);
                        isAssume = true;
                    }
                }
            }
            String DS = "Nothing";
            String ES = "Nothing";
            String FS = "Nothing";
            String SS = "Nothing";
            String GS = "Nothing";
            String CS = "Nothing";

            if(isAssume){
                for(List<Token> operands : operand){
                    switch (operands.get(0).stringToken){
                        case("DS"):
                            DS = operands.get(2).stringToken;
                            break;
                        case ("ES"):
                            ES = operands.get(2).stringToken;
                            break;
                        case ("FS"):
                            FS = operands.get(2).stringToken;
                            break;
                        case ("SS"):
                            SS = operands.get(2).stringToken;
                            break;
                        case ("CS"):
                            CS = operands.get(2).stringToken;
                            break;
                        case ("GS"):
                            GS = operands.get(2).stringToken;
                            break;
                        default:
                            System.out.println("Unknown register");
                            break;
                    }
                }
                System.out.println("Segment Register | Destination");
                System.out.println("------------------------------");
                System.out.println("DS               | " + DS );
                System.out.println("CS               | " + CS );
                System.out.println("SS               | " + SS );
                System.out.println("ES               | " + ES );
                System.out.println("GS               | " + GS );
                System.out.println("FS               | " + FS );

            }
        }

    public static List<Identifier> TableIdentifier(List<List<Token>> fileToken){
        List<Identifier> identifiers = new ArrayList<>();
        for (int index = 0; index < fileToken.size(); index++) {
            if(!fileToken.get(index).isEmpty() && fileToken.get(index).get(0).type == TokenType.Identifier) identifiers.add(new Identifier(index, fileToken.get(index)));
        }
        return identifiers;
    }

    public static List<Instruction> TableInstraction(List<List<Token>> fileToken){
        List<Instruction> instructions = new ArrayList<>();
        for (int index = 0; index < fileToken.size(); index++) {
            if(!fileToken.get(index).isEmpty())
            for (int indexToken = 0; indexToken < fileToken.get(index).size();indexToken++)
            if(fileToken.get(index).get(indexToken).type == TokenType.Instruction ||
                    fileToken.get(index).get(indexToken).type == TokenType.JmpWord ||
                    fileToken.get(index).get(indexToken).type == TokenType.JncWord ) instructions.add(new Instruction(index, fileToken.get(index)));
        }
        return instructions;
    }
}


