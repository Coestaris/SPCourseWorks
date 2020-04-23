program Hello;

uses tokenizer;

const 
    InputFileName : string = 'bin/test.asm';
    OutputFileName : string = 'bin/output.log';

var    
    outputFile : TextFile;

begin
    AssignFile(outputFile, OutputFileName);
    Rewrite(outputFile);

    Tokenize(InputFileName, OutputFile, true);

    CloseFile(outputFile);
end.