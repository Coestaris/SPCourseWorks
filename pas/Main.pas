program CourseWork;

uses 
    Base,
    Tokenizer,
    FirstPass;

const 
    InputFileName : string = 'bin/test1.asm';
    OutputFileName : string = 'bin/output1.log';

var    
    outputFile : TextFile;
    storage : ASMStorage;
    i : integer;

begin
    AllocInstructions();

    AssignFile(outputFile, OutputFileName);
    Rewrite(outputFile);

    storage := Tokenize(InputFileName, OutputFile, false);
    DoFirstPass(@storage, OutputFile, true);

    CloseFile(outputFile);
end.