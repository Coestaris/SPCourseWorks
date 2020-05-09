unit ByteFunctions;

interface

uses Base, SysUtils;

function GetEmptyBytes() : Bytes;
function GetByteString(b : Bytes) : string;

procedure BSetImm(b : PBytes; size : integer; imm : int64);
procedure BSetOpcode(b : PBytes; opcode : integer);
procedure BPackRegister(b : PBytes; register : string);
procedure BSetPrefixes(b : PBytes; exp : boolean; data : boolean; idx : boolean);
procedure BSetSegmentPrefix(b : PBytes; register : string);
procedure BSetRegReg(b : PBytes; register : string);
procedure BSetRegConst(b : PBytes; constant : byte);
procedure BSetModReg(b : PBytes; register : string);
procedure BSetModIndex(b : PBytes; offset : integer; register : string; scale : string);
procedure BSetModDirect(b : PBytes; offset : integer);

implementation

const MOD_REG : integer = 3;
const MOD_IND_REG_DISP32 : integer = 2;
const MOD_DISP_ONLY : integer = 0;
const MOD_DISP32 : integer = 0;
const RM_SIB : integer = 4;
const RM_DISP_ONLY : integer = 6;
const SIB_B_NO_BASE : integer = 5;

function GetSegRegCode(register : string) : byte;
begin
    if register = 'cs' then exit($2E);
    if register = 'ss' then exit($36);
    if register = 'ds' then exit($3E);
    if register = 'es' then exit($26);
    if register = 'fs' then exit($64);
    if register = 'gs' then exit($65);
    writeln(register, ' invalid segreg')
end;

function GetRegCode(register : string) : byte;
begin
    if register = 'eax' then exit(0);
    if register = 'ecx' then exit(1);
    if register = 'edx' then exit(2);
    if register = 'ebx' then exit(3);
    if register = 'esp' then exit(4);
    if register = 'ebp' then exit(5);
    if register = 'esi' then exit(6);
    if register = 'edi' then exit(7);
    if register = 'ax' then exit(0);
    if register = 'cx' then exit(1);
    if register = 'dx' then exit(2);
    if register = 'bx' then exit(3);
    if register = 'sp' then exit(4);
    if register = 'bp' then exit(5);
    if register = 'si' then exit(6);
    if register = 'di' then exit(7);
    if register = 'al' then exit(0);
    if register = 'cl' then exit(1);
    if register = 'dl' then exit(2);
    if register = 'bl' then exit(3);
    if register = 'ah' then exit(4);
    if register = 'ch' then exit(5);
    if register = 'dh' then exit(6);
    if register = 'bh' then exit(7);
    writeln(register, 'sadasd')
end;

function GetModRM(m : byte; r : byte; rm : byte) : byte;
begin
    GetModRM := ((m and 3) shl 6) or ((r and 7) shl 3) or (rm and 7);
end;

function GetSIB(s : byte; i : byte; b : byte) : byte;
begin
    GetSIB := ((s and 3) shl 6) or ((i and 7) shl 3) or (b and 7);
end;

function GetEmptyBytes() : Bytes;
begin
    result.hasOpcode := false;
    result.opCode := 0;
    result.expPrefix := false;
    result.dataPrefix := false;
    result.indexPrefix := false;
    result.hasSegmentPrefix := false;
    result.segmentPrefix := 0;
    result.hasModmrm := false;
    result.modrm := 0;
    result.hasSib := false;
    result.sib := 0;
    result.immSize := 0;
    result.imm := 0;
    result.dispSize := 0;
    result.disp := 0;
    GetEmptyBytes := result;
end;

procedure BSetImm(b : PBytes; size : integer; imm : int64);
begin
    b.immSize := size;
    if size = 1 then
        b.imm := imm and $FF
    else if size = 2 then
        b.imm := imm and $FFFF
    else if size = 3 then
        b.imm := imm and $FFFFFF
    else
        b.imm := imm and $FFFFFFFF;
end;

procedure BSetOpcode(b : PBytes; opcode : integer);
begin
    b.hasOpcode := true;
    b.opcode := opcode;
end;

procedure BPackRegister(b : PBytes; register : string);
begin
    b.hasOpcode := true;
    b.opcode := b.opcode or GetRegCode(register);
end;

procedure BSetPrefixes(b : PBytes; exp : boolean; data : boolean; idx : boolean);
begin
    b.expPrefix := b.expPrefix or exp;
    b.dataPrefix := b.dataPrefix or data;
    b.indexPrefix := b.indexPrefix or idx;
end;

procedure BSetSegmentPrefix(b : PBytes; register : string);
begin
    b.hasSegmentPrefix := true;
    b.segmentPrefix := GetSegRegCode(register);
end;

procedure BSetRegReg(b : PBytes; register : string);
begin
    b.hasModmrm := true;
    b.modrm := b.modrm or GetModRM(0, GetRegCode(register), 0);
end;

procedure BSetRegConst(b : PBytes; constant : byte);
begin
    b.hasModmrm := true;
    b.modrm := b.modrm or GetModRM(0, constant, 0);
end;

procedure BSetModReg(b : PBytes; register : string);
begin
    b.hasModmrm := true;
    b.modrm := b.modrm or GetModRM(MOD_REG, 0, GetRegCode(register));
end;

procedure BSetModDirect(b : PBytes; offset : integer);
begin
    b.hasModmrm := true;
    b.modrm := b.modrm or GetModRM(MOD_DISP_ONLY, 0, RM_DISP_ONLY);
    b.dispSize := 2;
    b.disp := offset and $FFFF;
end;

procedure BSetModIndex(b : PBytes; offset : integer; register : string; scale : string);
var s : integer;
begin
    b.hasModmrm := true;
   
    b.dispSize := 4;
    b.disp := offset and $FFFFFFFF;
    b.indexPrefix := true;
   
    if scale = '1' then 
    begin
        b.modrm := b.modrm or GetModRM(MOD_IND_REG_DISP32, 0, GetRegCode(register));
        exit;
    end
    else if scale = '2' then s := 1
    else if scale = '4' then s := 2
    else s := 3;
    
    b.modrm := b.modrm or GetModRM(MOD_DISP32, 0, RM_SIB);
    b.hasSib := true;
    b.sib := b.sib or GetSIB(s, GetRegCode(register), SIB_B_NO_BASE);
end;

function GetByteString(b : Bytes) : string;
begin
    result := '';
    if b.hasSegmentPrefix then 
        result := Concat(Concat(result, IntToHex(b.segmentPrefix, 2)), ': ');

    if b.dataPrefix then
        result := Concat(result, '66| ');

    if b.indexPrefix then
        result := Concat(result, '67| ');

    if b.expPrefix then
        result := Concat(result, '0F| ');

    if b.hasOpcode then
        result := Concat(Concat(result, IntToHex(b.opcode, 2)), ' ');

    if b.hasModmrm then
        result := Concat(Concat(result, IntToHex(b.modrm, 2)), ' ');

    if b.hasSib then
        result := Concat(Concat(result, IntToHex(b.sib, 2)), ' ');

    if b.dispSize <> 0 then
        result := Concat(Concat(result, IntToHex(b.disp, b.dispSize * 2)), ' ');

    if b.immSize <> 0 then
        result := Concat(Concat(result, IntToHex(b.imm, b.immSize * 2)), ' ');

    GetByteString := result;
end;

end.