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
    AssignFile(outputFile, OutputFileName);
    Rewrite(outputFile);

    storage := Tokenize(InputFileName, OutputFile, false);

    DoFirstPass(@storage);

    PrintUserNames(outputFile, @storage);
    for i := 1 to storage.lexemesLen do 
    begin
       PrintError(storage.lexemes[i], outputFile);
    end;

    CloseFile(outputFile);
end.