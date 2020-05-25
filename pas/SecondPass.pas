unit SecondPass;

{$MODE DELPHI}
{$I DefMacro.inc}

interface

uses Base;

procedure DoSecondPass(storage : PASMStorage; var outfile : TextFile; output : boolean);

implementation

uses 
    FirstPass, 
    SysUtils, 
    StrUtils, 
    ByteFunctions;

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

function GetInstructionBytes(l : PLexeme; storage : PASMStorage) : Bytes;
var 
    un : PUserName;
    i : integer;
    diff : longint;
    jmp, fwd, nr : boolean;
begin
    result := GetEmptyBytes();

    if (l.tokens[1].token = 'jns') or (l.tokens[1].token = 'jmp')  then
    begin
        un := GetUserNameByName(storage, false, l.tokens[2].token);
        diff := storage.lexemes[un.lexemeIndex].offset - l.offset - l.size;

        jmp := l.tokens[1].token = 'jmp';
        fwd := l.operandInfos[1].operandType = LabelF;
        nr := abs(diff) < 127;

        if jmp then
        begin
            if not nr then
            begin
                BSetOpcode(@result, $E9);
                BSetImm(@result, 2, diff);
            end
            else
            begin
                BSetOpcode(@result, $EB);
                if not fwd then     
                    BSetImm(@result, 1, diff)
                else
                begin
                    i := ((diff + 1) shl 8) or $90;
                    BSetImm(@result, 2, i)
                end;
            end;
        end 
        else
            if not nr then
            begin
                BSetOpcode(@result, $89);
                BSetPrefixes(@result, true, false, false);
                BSetImm(@result, 2, diff);
            end
            else
            begin
                BSetOpcode(@result, $79);
                if not fwd then     
                    BSetImm(@result, 1, diff)
                else
                begin
                    i := ((diff + 2) shl 16) or $9090;
                    BSetImm(@result, 3, i)
                end;
            end;
        begin
            
        end;

        exit(result);
    end;

    // Fill opcode
    BSetOpcode(@result, l.instr.opcode);

    // Data change prefix
    if l.b32mode then
        BSetPrefixes(@result, false, true, false);

    // Expansion prefix
    if l.instr.expPrefix then
        BSetPrefixes(@result, true, false, false);

    // Packed register (always first operand in our case)
    if l.instr.packedReg then
        BPackRegister(@result, l.tokens[l.op1Index].token);

    // ModRM / SIB / DISP / SEG CHANGE
    if l.instr.modrm <> 0 then
    begin
        // Fill REG part (const or register)
        if l.instr.regOpcode <> -1 then
            BSetRegConst(@result, l.instr.regOpcode)
        else
        begin
            if l.instr.modrm = 1 then
                BSetRegReg(@result, l.tokens[l.op2Index].token)
            else
                BSetRegReg(@result, l.tokens[l.op1Index].token);
        end;

        // Fill ModRM pard + SIB + DISP
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
            un := GetUserNameByName(storage, true, l.operandInfos[l.instr.modrm].token.token);
            if un = nil then
            begin
                writeln('Unable to find name ', l.operandInfos[l.instr.modrm].token.token);
                exit;
            end;

            if l.operandInfos[l.instr.modrm].directIndex then
            begin
                BSetModDirect(@result, storage.lexemes[un.lexemeIndex].offset);
            end
            else
            begin
                BSetModIndex(@result, storage.lexemes[un.lexemeIndex].offset, 
                    l.operandInfos[l.instr.modrm].base.token, 
                    l.operandInfos[l.instr.modrm].scale.token);
            end;
        end;

        // Segment change prefix
        if l.operandInfos[l.instr.modrm].hasSegmentReg  then
        begin
            if l.operandInfos[l.instr.modrm].segmentReg.token = 'ss' then
            begin
                if not (not l.operandInfos[l.instr.modrm].directIndex and (l.operandInfos[l.instr.modrm].base.token = 'ebp')) then
                begin
                    BSetSegmentPrefix(@result, l.operandInfos[l.instr.modrm].segmentReg.token);
                end;
            end
            else if l.operandInfos[l.instr.modrm].segmentReg.token = 'ds' then
            begin
                if (not l.operandInfos[l.instr.modrm].directIndex and (l.operandInfos[l.instr.modrm].base.token = 'ebp')) then
                begin
                    BSetSegmentPrefix(@result, l.operandInfos[l.instr.modrm].segmentReg.token);
                end;
            end
            else
            begin
                BSetSegmentPrefix(@result, l.operandInfos[l.instr.modrm].segmentReg.token);
            end;
        end;
    end;

    // Fill IMM
    for i := 1 to l.instr.opCount do 
    begin
        if (l.operandInfos[i].operandType = ImmSmall) or (l.operandInfos[i].operandType = ImmBig) then
        begin
            if l.instr.imm <> 0 then
            begin
                if l.b32mode and (l.instr.imm = 2) then
                    BSetImm(@result, 4, l.operandInfos[i].number)
                else 
                    BSetImm(@result, l.instr.imm, l.operandInfos[i].number);
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
        if l.hasError then
            continue;

        if l.lexemeType = InstructionLexeme then
            l.bytes := GetInstructionBytes(l, storage);
        if l.lexemeType = VarDefinition then
            l.bytes := GetVariableBytes(l);
    end;

    // Output
    if output then
    begin
        writeln(outfile, '  # |  Off | Size |             Bytes            | Lex');
        for i := 1 to storage.lexemesLen do 
        begin
            l := @storage.lexemes[i];
            if not l.hasError and ((l.lexemeType = InstructionLexeme) or (l.lexemeType = VarDefinition)) then
                str := GetByteString(l.bytes)
            else
                str := '';

            writeln(outfile, i:3, 
                ' | ', IntToHex(longint(l.offset), 4), 
                ' | ', IntToHex(longint(l.size), 4), 
                ' | ', PadRight(str, 28),
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