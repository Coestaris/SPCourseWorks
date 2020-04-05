package course.work.sp;

import java.util.ArrayList;
import java.util.List;

public class Application {
    public static String getFilepath(){
        return "D:\\Repos\\sp_kurs\\java\\src\\main\\resources\\testFile.asm";
    }

    public static void main(String[] args) {
        String filePath = getFilepath();
        //filePath = "D:\\LAB\\SP\\CourseWork\\start.asm";
        //filePath = "D:\\LAB\\SP\\CourseWork\\start.asm";
        //FileParser.LexicalAndSyntaxAnalysis(filePath);
        System.out.println(FirstPass.TableInstraction(FileParser.fileParser(filePath)) + "\n");
        //FirstPass.SegmentDestination(FileParser.fileParser(filePath));
        //FirstPass.firstPass(FileParser.fileParser(filePath), filePath);

    }
}
