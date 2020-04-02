package course.work.sp;

public class Application {

    public static void main(String[] args) {
        String filePath = "D:\\Repos\\sp_kurs\\java\\src\\main\\resources\\testFile.asm";
        //filePath = "D:\\LAB\\SP\\CourseWork\\start.asm";
        FileParser.LexicalAndSyntaxAnalysis(filePath);

        //FirstPass.SegmentDestination(FileParser.fileParser(filePath));
    }
}
