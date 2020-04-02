package course.work.sp;

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
}
