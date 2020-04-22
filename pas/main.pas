program Hello;

uses tokenizer;

const 
    InputFile : string = 'bin/test.asm';
    OutputFile : string = 'bin/output.log';

begin
   Tokenize(InputFile, OutputFile, true);
end.