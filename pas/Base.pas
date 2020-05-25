unit Base;

{$MODE DELPHI}
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


type InstructionInfo = record
    mnem : string;
    opCount : integer;
    operands : array[1..2] of TOperandType;

    opcode : byte;
    expPrefix : boolean;
    packedReg : boolean;

    modrm : integer;
    imm : integer;
    regOpcode : integer;
end;


type PInstructionInfo = ^InstructionInfo;


type Bytes = record
    hasOpcode : boolean;
    opCode : byte;

    expPrefix : boolean;
    dataPrefix : boolean;
    indexPrefix : boolean;
    
    hasSegmentPrefix : boolean;
    segmentPrefix : byte;

    hasModmrm : boolean;
    modrm : byte;

    hasSib : boolean;
    sib : byte;

    immSize : integer;
    imm : int64;

    dispSize : integer;
    disp : int64;
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
    b32mode : boolean;
    instr : PInstructionInfo;
    offset : integer;
    size : integer;
    bytes : bytes;
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
type PBytes = ^Bytes;
type PASMStorage = ^ASMStorage;
type PUserName = ^UserName;
type POperandInfo = ^OperandInfo;


procedure SetError(lexeme : PLexeme; error : string; tokenIndex : integer);
procedure PrintError(lexeme : Lexeme; var outFile : TextFile);
function GetUserNameByName(storage : PASMStorage; variable : boolean; n : string) : PUserName;
function GetUserNameByLine(storage : PASMStorage; variable : boolean; n : integer) : PUserName;
function GetLexemeSegment(storage : PASMStorage; lexemeIndex : integer) : TType;
procedure AllocInstructions();

var instructions : array[1..MAX_INSTRUCTIONS] of InstructionInfo;

implementation

var instrCounter : integer;

function Instr(
    mnem : string; opCount : integer; op1 : TOperandType; op2 : TOperandType; 
    opcode : byte; expPrefix : boolean; modrm : integer; imm : integer; 
    regOpcode : integer; packedReg : boolean) : InstructionInfo;
var info : InstructionInfo; 
begin
    info.mnem := mnem; 
    info.opCount := opCount;
    info.operands[1] := op1;
    info.operands[2] := op2;
    info.opcode := opcode;
    info.expPrefix := expPrefix;
    info.modrm := modrm;
    info.imm := imm;
    info.regOpcode := regOpcode;
    info.packedReg := packedReg;

    instrCounter := instrCounter + 1;

    if instrCounter >= MAX_INSTRUCTIONS then 
    begin
        writeln('Instruction overflow =c');
        exit;
    end;

    instructions[instrCounter] := info;

    Instr := info;
end;

//9E            SAHF

//D0 /7         SAR r/m8,  1
//D1 /7         SAR r/m16, 1

// F6 /3        NEG r/m8
// F7 /3        NEG r/m16

// 0F A3        BT r/m16, r16

// 0F AF /r     IMUL r16, r/m16

// 84 /r        TEST r/m8, r8
// 85 /r        TEST r/m16, r16

// B0+rb        MOV reg8, imm8 
// B8+rw        MOV reg16, imm16

// 80 /6 ib     XOR r/m8, imm8
// 83 /6 ib     XOR r/m16, imm8
// 81 /6 wb     XOR r/m16, imm16

// JNS  79          OF 89 c
// JMP  EB          E9

procedure AllocInstructions();
begin
    instrCounter := 0;
    //  mnem opCount op1 op2 opcode expPrefix modrm imm regOpcode packedReg 
    Instr('sahf', 0, reg8, reg8,     $9E, false, 0, 0, -1, false);

    Instr('sar', 2, reg8,  immSmall, $D0, false, 1, 0, 7, false);
    Instr('sar', 2, reg16, immSmall, $D1, false, 1, 0, 7, false);

    Instr('neg', 1, mem8, reg8,      $F6, false, 1, 0, 3, false);
    Instr('neg', 1, mem16, reg8,     $F7, false, 1, 0, 3, false);
    
    Instr('bt', 2, reg16, reg16,     $A3, true, 1, 0, -1, false);

    Instr('imul', 2, reg16, mem16,   $AF, true, 2, 0, -1, false);

    Instr('test', 2, mem8, reg8,     $84, false, 1, 0, -1, false);
    Instr('test', 2, mem16, reg16,   $85, false, 1, 0, -1, false);

    Instr('mov', 2, reg8, immSmall,  $B0, false, 0, 1, -1, true);
    Instr('mov', 2, reg16, immBig,   $B8, false, 0, 2, -1, true);
    Instr('mov', 2, reg16, immSmall, $B8, false, 0, 2, -1, true);

    Instr('xor', 2, mem8,  immSmall, $80, false, 1, 1, 6, false);
    Instr('xor', 2, mem16, immSmall, $83, false, 1, 1, 6, false);
    Instr('xor', 2, mem16, immBig,   $81, false, 1, 2, 6, false);

    Instr('jns', 1, labelF, reg8,    $0, false, 0, 0, 0, false);
    Instr('jns', 1, labelB, reg8,    $0, false, 0, 0, 0, false);

    Instr('jmp', 1, labelF, reg8,    $0, false, 0, 0, 0, false);
    Instr('jmp', 1, labelB, reg8,    $0, false, 0, 0, 0, false);
end;

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
    if lexeme.hasError then 
    begin
        if lexeme.errorToken <= lexeme.tokensLen then
            writeln(outFile, 'Error: ', lexeme.errorMessage, ' near token "', lexeme.tokens[lexeme.errorToken].token, '"')
        else
            writeln(outFile, 'Error: ', lexeme.errorMessage, '');
    end
end;

end.