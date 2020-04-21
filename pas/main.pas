program Hello;

uses tokenizer;

const 
    InputFile : string = 'test.asm';
    OutputFile : string = 'output.log';

begin
   Tokenize(InputFile, OutputFile, true);
end.