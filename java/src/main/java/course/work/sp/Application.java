package course.work.sp;

import course.work.sp.fileparser.NewFileParser;
import course.work.sp.fileparser.NewSentence;
import course.work.sp.firstpass.FirstPassSentence;
import course.work.sp.identifierstorage.IdentifierStore;
import course.work.sp.lexicalAndSyntaxisAnalysis.FileParser;
import course.work.sp.firstpass.FirstPass;

import java.io.FileWriter;
import java.io.IOException;
import java.util.List;

public class Application {
    public static String getFilepath(){
        //return  "D:\\LAB\\SP\\CourseWork\\start.asm";
         return "D:\\Repos\\sp_kurs\\java\\src\\main\\resources\\testFile.asm";
    }

    public static void main(String[] args) {
        //String filePath = getFilepath();
        //filePath = "D:\\LAB\\SP\\CourseWork\\start.asm";
        //filePath = "D:\\LAB\\SP\\CourseWork\\start.asm";
        //FileParser.LexicalAndSyntaxAnalysis(filePath);
        //FirstPass.SegmentDestination(FileParser.fileParser(filePath));
        //FirstPass.firstPass(FileParser.fileParser(filePath), filePath);
        String firstPass = new FirstPass(new NewFileParser(getFilepath()).getNewSentenceList()).toString();
        try(FileWriter writer = new FileWriter("D:\\Repos\\sp_kurs\\java\\src\\main\\resources\\FirstPass.txt", false)) {
            // запись всей строки
            writer.write(String.valueOf(firstPass));
            writer.flush();
        }
        catch(IOException ex){
            System.out.println(ex.getMessage());
        }
        //            System.out.println(nw.toString());





        // FirstPass.TableOfInstructionAndIdentifier();
        //FirstPass.print();
        //NewFileParser.newFileParser(getFilepath());
    }
}
