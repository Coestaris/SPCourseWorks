unit base;

interface

type TType = (
    ModelDirective, DataDirective, CodeDirective, EndDirective,
    DbDirective, DwDirective, DdDirective,
    Register8, Register16, Register32, RegisterSeg,
    Instruction,
    Identifier, Unknown, Symbol,
    NumberBin, NumberDec, NumberHex);


type Token = record
    token : string;
    tokenType : TType; 
end;


type Lexeme = record

    // General fields
    tokensLen : integer;
    tokens : array[1..20] of Token;
    lexemeLine : string;
    lexemeLineIndex : integer;
    
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

type PLexeme = ^Lexeme;

procedure SetError(lexeme : PLexeme; error : string; tokenIndex : integer);

implementation

procedure SetError(lexeme : PLexeme; error : string; tokenIndex : integer);
begin
    lexeme.hasError := true;
    lexeme.errorMessage := error;
    lexeme.errorToken := tokenIndex;
end;

end.