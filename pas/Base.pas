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
    EmptySpecifier,
    LUnknown
);


type Token = record
    token : string;
    tokenType : TType; 
end;

type TOperandType = (
    Mem8, Mem16, Mem32,
    Reg8, Reg16, Reg32,
    ImmSmall, ImmBig,
    LabelF, LabelB
);

type OperandInfo = record
    operandType : TOperandType;
    token : Token;
    
    // For indexing
    directIndex : boolean;
    base : Token;
    scale : Token;
    hasSegmentReg : boolean;
    segmentReg : Token;

    // For imm
    number : longint;
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
    operandInfos : array[1..2] of OperandInfo;

    // Error fields
    hasError : boolean;
    errorToken : integer;
    errorMessage : string;

    // Bytes stuff
    offset : integer;
    size : integer;
end;


type UserName = record
    symbolName : string;
    lexemeIndex : integer;
    isVar : boolean;
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
type PUserName = ^UserName;
type POperandInfo = ^OperandInfo;

procedure SetError(lexeme : PLexeme; error : string; tokenIndex : integer);
procedure PrintError(lexeme : Lexeme; var outFile : TextFile);
function GetUserNameByName(storage : PASMStorage; variable : boolean; n : string) : PUserName;
function GetUserNameByLine(storage : PASMStorage; variable : boolean; n : integer) : PUserName;
function GetLexemeSegment(storage : PASMStorage; lexemeIndex : integer) : TType;

implementation

function GetLexemeSegment(storage : PASMStorage; lexemeIndex : integer) : TType;
begin
    if (lexemeIndex <= storage.codeSegmentIndex) and 
        (lexemeIndex >= storage.dataSegmentIndex) then
        exit(DataDirective);
    if (lexemeIndex >= storage.codeSegmentIndex) then
        exit(CodeDirective);
   
    GetLexemeSegment := Unknown;
end;

function GetUserNameByName(storage : PASMStorage; variable : boolean; n : string) : PUserName;
var i : integer;
begin
    for i := 1 to storage.userNamesLen do 
    begin
        if (storage.userNames[i].symbolName = n) and (storage.userNames[i].isVar = variable) then
            exit(@storage.userNames[i]);
    end;
    exit(nil);
end;

function GetUserNameByLine(storage : PASMStorage; variable : boolean; n : integer) : PUserName;
var i : integer;
begin
    for i := 1 to storage.userNamesLen do 
    begin
        if (storage.userNames[i].lexemeIndex = n) and (storage.userNames[i].isVar = variable) then
            exit(@storage.userNames[i]);
    end;
    exit(nil);
end;

procedure SetError(lexeme : PLexeme; error : string; tokenIndex : integer);
begin
    lexeme.hasError := true;
    lexeme.errorMessage := error;
    lexeme.errorToken := tokenIndex;
end;

procedure PrintError(lexeme : Lexeme; var outFile : TextFile);
var i : integer;
begin
    writeln(outfile, lexeme.lexemeLine);
    
    if lexeme.hasError then 
    begin
        if lexeme.errorToken <= lexeme.tokensLen then
            writeln(outFile, 'Error: ', lexeme.errorMessage, ' near token "', lexeme.tokens[lexeme.errorToken].token, '"')
        else
            writeln(outFile, 'Error: ', lexeme.errorMessage, '');
    end
    else if lexeme.lexemeType = InstructionLexeme then
            for i := 1 to lexeme.opCount do
                writeln(outfile, lexeme.operandInfos[i].operandType);
end;

end.