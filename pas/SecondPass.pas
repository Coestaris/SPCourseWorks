unit SecondPass;
{$I DefMacro.inc}

interface

uses Base;

procedure DoSecondPass(storage : PASMStorage; var outfile : TextFile; output : boolean);

implementation

uses FirstPass, SysUtils, StrUtils, ByteFunctions;

function GetVariableBytes(l : PLexeme) : Bytes;
var imm : longint;
begin
    result := GetEmptyBytes();

    imm := TokenToInt(l.tokens[3]);

    if l.tokens[2].tokenType = DbDirective then
        BSetImm(@result, 1, imm);

    if l.tokens[2].tokenType = DwDirective then
        BSetImm(@result, 2, imm);
    
    if l.tokens[2].tokenType = DdDirective then
        BSetImm(@result, 4, imm);

    GetVariableBytes := result;
end;

function GetInstructionBytes(l : PLexeme) : Bytes;
var dest, reg : integer;
begin
    result := GetEmptyBytes();

    if (l.tokens[1].token = 'jns') or (l.tokens[1].token = 'jmp') then
    begin
        exit(result);
    end; 

    BSetOpcode(@result, l.instr.opcode);

    if l.b32mode then
        BSetPrefixes(@result, false, true, false);

    if l.instr.expPrefix then
        BSetPrefixes(@result, true, false, false);

    if l.instr.packedReg then
        BPackRegister(@result, l.tokens[l.op1Index].token);

    if l.instr.modrm <> 0 then
    begin
        if l.instr.regOpcode <> -1 then
            BSetRegConst(@result, l.instr.regOpcode)
        else
        begin
            if l.instr.modrm = 1 then
                BSetRegReg(@result, l.tokens[l.op2Index].token)
            else
                BSetRegReg(@result, l.tokens[l.op1Index].token);
        end;

        if (l.operandInfos[l.instr.modrm].operandType = Reg8) or 
                (l.operandInfos[l.instr.modrm].operandType = Reg16) or 
                (l.operandInfos[l.instr.modrm].operandType = Reg32) then
        begin
            if l.instr.modrm = 1 then
                BSetModReg(@result, l.tokens[l.op1Index].token)
            else
                BSetModReg(@result, l.tokens[l.op2Index].token);
        end
        else
        begin
            if l.operandInfos[l.instr.modrm].directIndex then
            begin
                BSetModDirect()
            end;
        end;
    end;

    GetInstructionBytes := result;
end;

procedure DoSecondPass(storage : PASMStorage; var outfile : TextFile; output : boolean);
var 
    i : integer;
    l : PLexeme;
    str : string;
begin
    for i := 1 to storage.lexemesLen do
    begin
        l := @storage.lexemes[i];
        if l.lexemeType = InstructionLexeme then
            l.bytes := GetInstructionBytes(l);
        if l.lexemeType = VarDefinition then
            l.bytes := GetVariableBytes(l);
    end;

    // Output
    if output then
    begin
        writeln(outfile, ' # |  Off | Size |        Bytes       | Lex');
        for i := 1 to storage.lexemesLen do 
        begin
            l := @storage.lexemes[i];
            if (l.lexemeType = InstructionLexeme) or (l.lexemeType = VarDefinition) then
                str := GetByteString(l.bytes)
            else
                str := '';

            writeln(outfile, i:2, 
                ' | ', IntToHex(longint(l.offset), 4), 
                ' | ', IntToHex(longint(l.size), 4), 
                ' | ', PadRight(str, 18),
                ' | ',l.lexemeLine);
            PrintError(storage.lexemes[i], outfile);
        end;
        writeln(outfile);
        PrintUserNames(outfile, storage);
        writeln(outfile);
        PrintSegmentTables(outfile, storage);
    end;    
end;

end.