unit FirstPass;
{$I DefMacro.inc}

interface

uses Base;

procedure DoFirstPass(storage : PASMStorage);
procedure PrintUserNames(var outfile : TextFile; storage : PASMStorage);

implementation

uses SysUtils, Math;
    
function GetLexemeType(lexeme : PLexeme) : LType;
begin

    if(lexeme.tokensLen = 0) then
        exit(EmptySpecifier);

    if (lexeme.tokensLen = 2) and 
        (lexeme.tokens[1].tokenType = ModelDirective) and
        (lexeme.tokens[2].token = 'small') then
        exit(ModelSpecifier);

    if (lexeme.tokensLen = 1) and (lexeme.tokens[1].tokenType = DataDirective) then
        exit(DataSpecifier);
    
    if (lexeme.tokensLen = 1) and (lexeme.tokens[1].tokenType = EndDirective) then
        exit(EndSpecifier);

    if (lexeme.tokensLen = 1) and (lexeme.tokens[1].tokenType = CodeDirective) then
        exit(CodeSpecifier);
    
    if (lexeme.tokensLen = 2) and 
        (lexeme.tokens[1].tokenType = Identifier) and 
        (lexeme.tokens[2].token = ':') then
        exit(LabelDefinition);

    if (lexeme.tokensLen >= 1) and
        (lexeme.tokens[1].tokenType = Instruction) then
        exit(InstructionLexeme);

    if (lexeme.tokensLen = 3) and 
        (lexeme.tokens[1].tokenType = Identifier) and
        ((lexeme.tokens[2].tokenType = DwDirective) or (lexeme.tokens[2].tokenType = DdDirective) or (lexeme.tokens[2].tokenType = DbDirective)) and 
        ((lexeme.tokens[3].tokenType = NumberHex) or (lexeme.tokens[3].tokenType = NumberDEc) or (lexeme.tokens[3].tokenType = NumberBin)) then
        exit(VarDefinition);

    GetLexemeType := LUnknown;
end;

procedure PrintUserNames(var outfile : TextFile; storage : PASMStorage);
var i : integer;
    t : string;
    s : string;
    nameLex : PLexeme;
    currSeg : TType;
begin
    for i := 1 to storage.userNamesLen do 
    begin
        nameLex := @storage.lexemes[storage.usernames[i].lexemeIndex];
        currSeg := GetLexemeSegment(storage, storage.usernames[i].lexemeIndex);

        if storage.usernames[i].isVar then 
        begin
            if nameLex.tokens[2].tokenType = DdDirective then
                t := 'dword'
            else if nameLex.tokens[2].tokenType = DwDirective then
                t := 'word'
            else 
                t := 'byte';
        end 
        else t := 'label';

        if currSeg = CodeDirective then s := 'cs'
        else s := 'ds';

        write(outfile, i, ' | ', storage.usernames[i].symbolName:10, ' | ');
        write(outfile, t : 5, ' | ');
        write(outfile, storage.usernames[i].lexemeIndex:5, ' | ');
        writeln(outfile, s, ':', nameLex.offset);
    end;
end;

function StrToIntBase(str : string; base : integer) : longint;
const 
    literals : string = '0123456789abcdef';
var 
    has_sign : boolean;
    i, j, literal : integer;
    mul : longint;
begin
    has_sign := str[1] = '-';
    result := 0;
    mul := 1;
    for i := length(str) downto 1 do
    begin
        literal := 0;
        for j := 1 to length(literals) do
            if literals[j] = str[i] then
            begin
                literal := j;
                break;
            end; 
        
        result := result + mul * (literal - 1);
        mul := mul * base;
    end;
    
    if has_sign then
        exit(-result);

    StrToIntBase := result;
end;

function TokenToInt(t : Token) : longint;
var s : string;
begin
    if t.tokenType = NumberDec then
        exit(StrToInt(t.token))
    else 
    begin
        s := t.token;
        SetLength(s, length(s) - 1);
        if t.tokenType = NumberHex then
            exit(StrToIntBase(s, 16))
        else
            exit(StrToIntBase(s, 2));
    end;
    TokenToInt := 0;
end;

procedure DetermineOperandType(lexeme : PLexeme; storage : PASMStorage);
var 
    offset, start, len, i : integer;
    imm : longint;
    co : POperandInfo;
    un : PUserName;
    varLexeme : PLexeme;
begin

    if lexeme.lexemeType <> InstructionLexeme then
        exit;

    lexeme.operandInfos[1].operandType := Mem8;
    lexeme.operandInfos[2].operandType := Mem8;

    for i := 1 to lexeme.opCount do 
    begin
        if i = 1 then begin
            start := lexeme.op1Index;
            len := lexeme.op1Len;
        end 
        else 
        begin
            start := lexeme.op2Index;
            len := lexeme.op2Len;
        end;
        co := @lexeme.operandInfos[i];

        // imm, reg, label or direct mem (are they the same??)
        if len = 1 then 
        begin
            co.token := lexeme.tokens[start];
            if co.token.tokenType = Register8 then
                co.operandType := Reg8
            else if co.token.tokenType = Register16 then
                co.operandType := Reg16
            else if co.token.tokenType = Register32 then
                co.operandType := Reg32
            else if 
                (co.token.tokenType = NumberBin) or
                (co.token.tokenType = NumberDec) or
                (co.token.tokenType = NumberHex) then
            begin
                imm := TokenToInt(co.token);
                co.number := imm;
                if abs(imm) < 127 then
                    co.operandType := ImmSmall
                else
                    co.operandType := ImmBig;
            end
            else if co.token.tokenType = Identifier then
            begin
                // label or direct
                un := GetUserNameByName(storage, true, co.token.token);
                if un <> nil then
                begin
                    // Variable
                    varLexeme := @storage.Lexemes[un.lexemeIndex]; 
                    co.directIndex := true;
                    
                    if varLexeme.tokens[2].tokenType = DbDirective then
                        co.operandType := mem8
                    else if varLexeme.tokens[2].tokenType = DwDirective then
                        co.operandType := mem16
                    else if varLexeme.tokens[2].tokenType = DdDirective then
                        co.operandType := mem32;
                end
                else 
                begin
                    // Label
                    un := GetUserNameByName(storage, false, co.token.token);
                    if un = nil then
                    begin
                        SetError(lexeme, 'Undefined reference', start);
                        exit;
                    end;

                    if un.lexemeIndex > lexeme.lexemeLineIndex then 
                       co.operandType := LabelF
                    else
                        co.operandType := LabelB;
                end;
            end
            else
            begin
                SetError(lexeme, 'Wrong operand type', start);
                exit;
            end;
        end
        else
        begin
            offset := 0;
            if (len > 8) or (len < 6) then
            begin
                SetError(lexeme, 'Wrong token count in operand', start);
                exit;
            end;

            if lexeme.tokens[start].tokenType = RegisterSeg  then
            begin
                if (lexeme.tokens[start + 1].token <> ':') then
                begin
                    SetError(lexeme, 'Expected ":"', start + 1);
                    exit;
                end;
                offset := 2;
                co.hasSegmentReg := true;
                co.segmentreg := lexeme.tokens[start + 2];
            end;

            if len - offset <> 6 then
            begin
                SetError(lexeme, 'Wrong token count in operand', start);
                exit;
            end;

            if  (lexeme.tokens[start + offset].tokenType <> Identifier) or 
                (lexeme.tokens[start + offset + 1].token <> '[') or 
                (lexeme.tokens[start + offset + 2].tokenType <> Register32) or 
                (lexeme.tokens[start + offset + 3].token <> '*') or
                (lexeme.tokens[start + offset + 5].token <> ']') then 
            begin
                SetError(lexeme, 'Wrong memory format. Expected: ID [ REG32 * NUMBER ]', start);
                exit;
            end;

            un := GetUserNameByName(storage, true, lexeme.tokens[start + offset].token);
            if un = nil then 
            begin
                SetError(lexeme, 'Undefined reference', start);
                exit;
            end;

            co.directIndex := true;
            co.base := lexeme.tokens[start + offset + 2];
            co.scale := lexeme.tokens[start + offset + 4];

            if co.base.token = 'ebp' then 
            begin
                SetError(lexeme, 'Base register couldn`t be EBP', start + 2);
                exit;
            end;
            
            if (co.scale.token <> '1') and (co.scale.token <> '2') and
                (co.scale.token <> '4') and (co.scale.token <> '8') then
            begin
                SetError(lexeme, 'Wrong scale. Scale coudld be only 1,2,4,8', start + 4);
                exit;
            end; 


            varLexeme := @storage.Lexemes[un.lexemeIndex]; 
            
            if varLexeme.tokens[2].tokenType = DbDirective then
                co.operandType := mem8
            else if varLexeme.tokens[2].tokenType = DwDirective then
                co.operandType := mem16
            else if varLexeme.tokens[2].tokenType = DdDirective then
                co.operandType := mem32;
        end;
    end;    
end;

procedure DoFirstPass(storage : PASMStorage);
var i : integer;
    l : PLexeme;
    un : PUserName;
    currSeg : TType;
    wrongOperands : boolean;
    wrongConstant : boolean;
    imm : longint;
begin
    // Determine lexeme types
    for i := 1 to storage.lexemesLen do 
    begin
        l := @storage.lexemes[i];

        if l.hasError then     
            continue;

        l.lexemeType := GetLexemeType(l);
        if l.lexemeType = LUnknown then
        begin
            SetError(l, 'Unkown lexeme type', MAX_TOKENS);
            continue;
        end;
    end;

    // Check usernames
    for i := 1 to storage.lexemesLen do 
    begin
        l := @storage.lexemes[i];

        if l.hasError then     
            continue;
    
        // check for .data
        if l.lexemeType = DataSpecifier then
        begin
            if storage.dataSegmentIndex <> 0 then
            begin
                SetError(l, 'Data segment redeclaration', 1);
                continue;
            end;
            storage.dataSegmentIndex := i;
        end;
        
        // check for .code
        if l.lexemeType = CodeSpecifier then
        begin
            if storage.codeSegmentIndex <> 0 then
            begin
                SetError(l, 'Code segment redeclaration', 1);
                continue;
            end;
            storage.codeSegmentIndex := i;
        end;
    
        // check for variables
        if l.lexemeType = VarDefinition then
        begin
            un := GetUserNameByName(storage, true, l.tokens[1].token);
            if un = nil then
            begin
                storage.userNamesLen := storage.userNamesLen + 1;
                if(storage.userNamesLen >= MAX_USERNAMES) then
                begin
                    SetError(l, 'Too many usernames. Increase MAX_USERNAMES constant to add more usernames', 3);
                    continue;
                end;

                storage.userNames[storage.userNamesLen].symbolName := l.tokens[1].token;
                storage.userNames[storage.userNamesLen].lexemeIndex := i;
                storage.userNames[storage.userNamesLen].isVar := true;
            end
            else
            begin
                SetError(l, 'Variable redeclaration', 1);
                continue;
            end;
        end;

          // check for labels
        if l.lexemeType = LabelDefinition then
        begin
            un := GetUserNameByName(storage, false, l.tokens[1].token);
            if un = nil then
            begin
                storage.userNamesLen := storage.userNamesLen + 1;
                if(storage.userNamesLen >= MAX_USERNAMES) then
                begin
                    SetError(l, 'Too many usernames. Increase MAX_USERNAMES constant to add more usernames', 3);
                    continue;
                end;

                storage.userNames[storage.userNamesLen].symbolName := l.tokens[1].token;
                storage.userNames[storage.userNamesLen].lexemeIndex := i;
                storage.userNames[storage.userNamesLen].isVar := false;
            end
            else
            begin
                SetError(l, 'Label redeclaration', 1);
                continue;
            end;
        end;
    end;

    // Check for lexemes out of segments
    for i := 1 to storage.lexemesLen do 
    begin
        l := @storage.lexemes[i];

        if l.hasError then     
            continue;
    
        currSeg := GetLexemeSegment(storage, i);
        if (currSeg = Unknown) and 
            (not ((l.lexemeType = ModelSpecifier) or (l.lexemeType = EmptySpecifier))) then
        begin
            SetError(l, 'Only "end" and ".model" could be out of segments', 1);
            continue;
        end;
    end;

    // Determine operand types
    for i := 1 to storage.lexemesLen do 
    begin
        l := @storage.lexemes[i];

        if l.hasError then     
            continue;
    
        DetermineOperandType(l, storage);

        // Check for variable constant sizes
        if (l.lexemeType = VarDefinition) and not l.hasError then
        begin
            imm := TokenToInt(l.tokens[3]); 
            if l.tokens[2].tokenType = DbDirective then
                if abs(imm) > $FF then 
                    begin
                        SetError(l, 'Constant is too big for byte variable', 3);
                        continue;  
                    end;
            
            if l.tokens[2].tokenType = DwDirective then
                if abs(imm) > $FFFF then 
                    begin
                        SetError(l, 'Constant is too big for word variable', 3);
                        continue;  
                    end;
            
            if l.tokens[2].tokenType = DdDirective then
                if abs(imm) > $FFFFFF then 
                    begin
                        SetError(l, 'Constant is too big for dword variable', 3);
                        continue;  
                    end;
        end;

        // Check for mem16|reg32, etc. cases...
        wrongOperands := false;
        wrongConstant := false;
        if (l.lexemeType = InstructionLexeme) and (l.opCount = 2) and not l.hasError then
        begin
            if (l.operandInfos[1].operandType = Mem16) and (l.operandInfos[2].operandType = Reg32) then wrongOperands := true;
            if (l.operandInfos[1].operandType = Mem32) and (l.operandInfos[2].operandType = Reg16) then wrongOperands := true;
            if (l.operandInfos[1].operandType = Reg16) and (l.operandInfos[2].operandType = Mem32) then wrongOperands := true;
            if (l.operandInfos[1].operandType = Reg32) and (l.operandInfos[2].operandType = Mem16) then wrongOperands := true;
            
            if (l.operandInfos[1].operandType = Reg16) and (l.operandInfos[2].operandType = Reg32) then wrongOperands := true;
            if (l.operandInfos[1].operandType = Reg32) and (l.operandInfos[2].operandType = Reg16) then wrongOperands := true;

            if (l.operandInfos[2].operandType = ImmBig) then
            begin
                // Assuming 16 
                if ((l.operandInfos[1].operandType = Mem16) or (l.operandInfos[1].operandType = Reg16)) 
                    and (abs(l.operandInfos[2].number) > $FFFF) then 
                    wrongConstant := true;
            end;
            
             if (l.operandInfos[1].operandType = ImmBig) then
            begin
                // Assuming 16 
                if ((l.operandInfos[2].operandType = Mem16) or (l.operandInfos[2].operandType = Reg16)) 
                    and (abs(l.operandInfos[1].number) > $FFFF) then 
                    wrongConstant := true;
            end;

            if wrongOperands then
            begin
                SetError(l, 'Operands bit depths mismatch', 1);
                continue;  
            end;

            if wrongConstant then
            begin
                SetError(l, 'Constant is too big for 16bit mode', 1);
                continue;  
            end;
        end;
    end;

    // Calculate size
end;

end.