unit base;

{$I DefMacro.inc}

interface

type TType = (
    ModelDirective, DataDirective, CodeDirective, EndDirective,
    DbDirective, DwDirective, DdDirective,
    Register8, Register16, Register32, RegisterSeg,
    Instruction,
    Identifier, Unknown, Symbol,
    NumberBin, NumberDec, NumberHex
);


type LType = (
    ModelSpecifier,
    DataSpecifier,
    CodeSpecifier,
    EndSpecifier,
    VarDefinition,
    LabelDefinition,
    InstructionLexeme,
    LUnknown
);


type Token = record
    token : string;
    tokenType : TType; 
end;


type Lexeme = record
    // General fields
    tokensLen : integer;
    tokens : array[1 .. MAX_TOKENS] of Token;
    lexemeLine : string;
    lexemeLineIndex : integer;
    lexemeType : LType;
    
    // Structure fields
    hasName : boolean;
    hasMnem : boolean;
    mnemIndex : integer;
    opCount : integer;
    op1Index : integer;
    op1Len   : integer;
    op2Index : integer;
    op2Len   : integer;

    // Error fields
    hasError : boolean;
    errorToken : integer;
    errorMessage : string;

end;


type UserName = record
    symbolName : string;
    lexemeIndex : integer;
end;


type ASMStorage = record
    lexemesLen : integer;
    lexemes : array[1..MAX_LEXEMES] of Lexeme;
    codeSegmentIndex : integer;
    dataSegmentIndex : integer;
    
    userNamesLen : integer;
    userNames : array[1..MAX_USERNAMES] of UserName;
end;


type PLexeme = ^Lexeme;
type PASMStorage = ^ASMStorage;

procedure SetError(lexeme : PLexeme; error : string; tokenIndex : integer);
procedure PrintError(lexeme : Lexeme; var outFile : TextFile);

implementation

procedure SetError(lexeme : PLexeme; error : string; tokenIndex : integer);
begin
    lexeme.hasError := true;
    lexeme.errorMessage := error;
    lexeme.errorToken := tokenIndex;
end;

procedure PrintError(lexeme : Lexeme; var outFile : TextFile);
begin
    if lexeme.hasError then 
    begin
        if lexeme.errorToken <= lexeme.tokensLen then
            writeln(outFile, 'Error: ', lexeme.errorMessage, ' near token "', lexeme.tokens[lexeme.errorToken].token, '"')
        else
            writeln(outFile, 'Error: ', lexeme.errorMessage, '');
    end;
end;

end.