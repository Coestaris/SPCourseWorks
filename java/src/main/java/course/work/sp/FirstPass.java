package course.work.sp;

import course.work.sp.lexicalAndSyntaxisAnalysis.FileParser;
import course.work.sp.segmentRegister.SegmentRegisterStorage;
import course.work.sp.sourcefile.DownloadFile;
import course.work.sp.tokenizer.Token;
import course.work.sp.tokenizer.TokenType;

import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class FirstPass {

    public static String SegmentDestination(List<List<Token>> fileToken) {
        SegmentRegisterStorage.getInstance().changeRegister(fileToken);
        return SegmentRegisterStorage.getInstance().toString();
    }


    public static List<Identifier> TableIdentifier(List<List<Token>> fileToken){
        List<Identifier> identifiers = new ArrayList<>();
        for (int index = 0; index < fileToken.size(); index++) {
            if(!fileToken.get(index).isEmpty() && fileToken.get(index).get(0).type == TokenType.Identifier) identifiers.add(new Identifier(index, fileToken.get(index)));
        }
        return identifiers;
    }

    public static List<Identifier> TableIdentifier(List<List<Token>> fileToken, List<Sentences> sentence){
        List<Identifier> identifiers = new ArrayList<>();
        for (int index = 0; index < fileToken.size(); index++) {
            if(!fileToken.get(index).isEmpty() && fileToken.get(index).get(0).type == TokenType.Identifier) identifiers.add(new Identifier(index, fileToken.get(index)));
        }

        for (Identifier identifier : identifiers) {
            for (Sentences sentences : sentence) {
                if (identifier.index == sentences.indexOfString) {
                    identifier.offset = sentences.offset;
                }
            }
        }
        return identifiers;
    }



    public static List<Sentences> TableSentences(List<List<Token>> fileToken){
        String file = DownloadFile.downloadFile(Application.getFilepath());
        file = file.toUpperCase();
        String[] lines = file.split("\n");
        List<Sentences> sentences = new ArrayList<>();
        for (int index = 0; index < fileToken.size(); index++){
            sentences.add(new Sentences(fileToken.get(index), index, lines[index]));
        }
        int sumOfsset = 0;
        boolean isSegment = false;

        for (Sentences sentence : sentences) {
            if (sentence.segmentStart) isSegment = true;
            if (sentence.segmentEnd) isSegment = false;
            if (isSegment) {
                sumOfsset+= sentence.offset;
                sentence.offset = sumOfsset;
            }else {
                sumOfsset = 0;
                if(!sentence.segmentEnd && !sentence.tokens.isEmpty() && (sentence.tokens.get(0).type.equals(TokenType.Instruction)
                        || sentence.tokens.get(0).type.equals(TokenType.Identifier))) sentence.error = true;
                sentence.offset = 0;
            }
        }

        for (int index = fileToken.size() - 1; index > 0; index--){
            sentences.get(index).offset = sentences.get(index - 1).offset;
        }

        return sentences;
    }

    public static List<Instruction> TableInstruction(List<List<Token>> fileToken){
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

    public static void TableOfInstructionAndIdentifier(){
//        System.out.println("Instruction\n");
//        System.out.println(FirstPass.TableInstruction(FileParser.fileParser(Application.getFilepath())) + "\n");
//        System.out.println("Identifier\n");
        System.out.println(FirstPass.TableIdentifier(FileParser.fileParser(Application.getFilepath())) + "\n");
  //      System.out.println(FirstPass.TableSentences(FileParser.fileParser(Application.getFilepath())) + "\n");
    }

    public static String FindSeg(List<Sentences> sentences, Identifier identifier){
        for(int i = identifier.index; i < sentences.size(); i++){
            if(sentences.get(i).segmentEnd) return sentences.get(i).tokens.get(0).stringToken;
        }
        return null;
    }

    public static void print(){
        List<Sentences> sentences = TableSentences(FileParser.fileParser(Application.getFilepath()));
        StringBuilder pass;
        pass = new StringBuilder("Index|  Offset |  Source " + "\n");
        for(Sentences sentence : sentences){
            pass.append(String.format(" %02d   ", sentence.indexOfString))
                    .append(" ")
                    .append(String.format("%08X  ", sentence.offset))
                    .append(" ").append(sentence.thisSentences);
            if(sentence.error) pass.append("  Error");

            pass.append("\n");
        }

        List<Identifier> identifiers = TableIdentifier(FileParser.fileParser(Application.getFilepath()), sentences);
        StringBuilder segmentTable;
        segmentTable = new StringBuilder("Segment name | Bit Depth | Size \n");
        StringBuilder UserDefined = new StringBuilder("NAME  | Type  |  seg  | Value\n");
        for(Identifier identifier : identifiers){
            if(identifier.identifier == TokenType.IdentifierEnd){
                    segmentTable.append(identifier.token)
                            .append("  32   ")
                            .append(String.format("%08X", identifier.offset))
                            .append("\n");
            }
            if(identifier.identifier == TokenType.Label ||
                    identifier.identifier == TokenType.Dw ||
                    identifier.identifier == TokenType.Db ||
                    identifier.identifier == TokenType.Dd){
                UserDefined.append(identifier.token)
                        .append(" | ")
                        .append(identifier.identifier)
                        .append(" | ")
                        .append(FindSeg(sentences, identifier))
                        .append(" | ")
                        .append(String.format("%08X",identifier.offset))
                        .append("\n");
            }
        }

        try(FileWriter writer = new FileWriter("D:\\Repos\\sp_kurs\\java\\src\\main\\resources\\FirstPass.txt", false)) {
            // запись всей строки
            writer.write(String.valueOf(pass));
            writer.append('\n');
            writer.write(SegmentDestination(FileParser.fileParser(Application.getFilepath())));
            writer.append('\n');
            writer.write(String.valueOf(segmentTable));
            writer.append('\n');
            writer.write(String.valueOf(UserDefined));

            writer.flush();
        }
        catch(IOException ex){
            System.out.println(ex.getMessage());
        }
    }

}



