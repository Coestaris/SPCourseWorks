package course.work.sp;

import course.work.sp.fileparser.NewFileParser;
import course.work.sp.fileparser.NewSentence;
import course.work.sp.firstpass.FirstPassSentence;
import course.work.sp.identifierstorage.IdentifierStore;
import course.work.sp.lexicalAndSyntaxisAnalysis.FileParser;
import course.work.sp.firstpass.FirstPass;
import course.work.sp.secondpass.SecondPass;

import java.io.FileWriter;
import java.io.IOException;
import java.util.List;

public class Application {
    public static String getFilepath(){
        //return  "D:\\LAB\\SP\\CourseWork\\start.asm";
         return "D:\\Repos\\sp_kurs\\java\\src\\main\\resources\\testFile.asm";
    }

    public static void main(String[] args) {
        //FileParser.LexicalAndSyntaxAnalysis(filePath);
        FirstPass firstPass = new FirstPass(new NewFileParser(getFilepath()).getNewSentenceList());
//        try(FileWriter writer = new FileWriter("D:\\Repos\\sp_kurs\\java\\src\\main\\resources\\FirstPass.txt", false)) {
//            // запись всей строки
//            writer.write(String.valueOf(firstPass.toString()));
//            writer.flush();
//        }
//        catch(IOException ex){
//            System.out.println(ex.getMessage());
//        }
        //System.out.println(firstPass.toStringTest());
        SecondPass secondPass = new SecondPass(firstPass.getFirstPassSentenceList());
        System.out.println(secondPass.toString());

    }
}
