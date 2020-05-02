program CourseWork;

uses 
    Base,
    Tokenizer,
    FirstPass;

const 
    InputFileName : string = 'bin/test.asm';
    OutputFileName : string = 'bin/output.log';

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