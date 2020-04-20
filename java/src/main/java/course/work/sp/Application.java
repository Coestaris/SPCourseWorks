package course.work.sp;

import course.work.sp.fileparser.NewFileParser;
import course.work.sp.fileparser.NewSentence;
import course.work.sp.identifierstorage.IdentifierStore;
import course.work.sp.lexicalAndSyntaxisAnalysis.FileParser;

public class Application {
    public static String getFilepath(){
        return  "D:\\LAB\\SP\\CourseWork\\start.asm";
         //return "D:\\Repos\\sp_kurs\\java\\src\\main\\resources\\testFile.asm";
    }

    public static void main(String[] args) {
        //String filePath = getFilepath();
        //filePath = "D:\\LAB\\SP\\CourseWork\\start.asm";
        //filePath = "D:\\LAB\\SP\\CourseWork\\start.asm";
        //FileParser.LexicalAndSyntaxAnalysis(filePath);
        //FirstPass.SegmentDestination(FileParser.fileParser(filePath));
        //FirstPass.firstPass(FileParser.fileParser(filePath), filePath);
        for (NewSentence nw: NewFileParser.newFileParser(getFilepath()))
            System.out.println(nw.toString());
        System.out.println(IdentifierStore.getInstance().toString());

       // FirstPass.TableOfInstructionAndIdentifier();
        //FirstPass.print();
        //NewFileParser.newFileParser(getFilepath());
    }
}
