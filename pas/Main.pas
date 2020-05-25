program CourseWork;

{$MODE DELPHI}

uses 
    Base,
    Tokenizer,
    FirstPass,
    SecondPass;

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
    DoFirstPass(@storage, OutputFile, false);
    DoSecondPass(@storage, OutputFile, true);

    CloseFile(outputFile);
end.