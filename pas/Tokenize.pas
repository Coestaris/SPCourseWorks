unit tokenizer;
{$MODE DELPHI}
{$I DefMacro.inc}

interface

uses base;

function Tokenize(InputFile : string; var outFile : TextFile; output : boolean) : ASMStorage;

implementation

function IsSplitter(c : char) : boolean;
begin
    IsSplitter := (c = ' ') or (c = '\n') or (c = ',') or (c = '\t') or (c = '[') or
        (c = ']') or (c = ':') or (c = '*');
end;

function TokenizeLexeme(lexeme : string; lineIndex : integer) : Lexeme;
var 
    c : char;
    i, tokens : integer;
    l : Lexeme;
    token : string;

begin
    tokens := 0;
    token := '';

    l.lexemeLineIndex := lineIndex;
    l.lexemeLine := lexeme;
    l.tokensLen := 0;

    l.hasName := false;
    l.hasMnem := false;
    l.opCount := 0;

    l.b32mode := false;
    l.offset := 0;
    l.size := 0;

    l.hasError := false;

    for i := 1 to length(lexeme) do
    begin
        c := lexeme[i];
        if IsSplitter(c) then   
        begin
            if length(token) <> 0 then
            begin
                if tokens = MAX_TOKENS then
                begin
                    SetError(@l, 'Line is too long. Increase MAX_TOKENS to allow longer lines', tokens - 1);
                    exit(l);
                end;
        
                tokens := tokens + 1;
                l.tokens[tokens].token := LowerCase(token);
                l.tokensLen := tokens;
            end;

            if (c <> ' ') and (c <> '\n') and (c <> '\t') then
            begin
                SetLength(token, 1);
                token[1] := c;
                
                if tokens = MAX_TOKENS then
                begin
                    SetError(@l, 'Line is too long. Increase MAX_TOKENS to allow longer lines', tokens - 1);
                    exit(l);
                end;

                tokens := tokens + 1;
                l.tokens[tokens].token := token;
                l.tokensLen := tokens;
            end;

            token := '';
        end
        else
        begin
            SetLength(token, length(token) + 1);
            token[length(token)] := c;
        end;
    end;

    if length(token) <> 0 then
    begin
        
        if tokens = MAX_TOKENS then
        begin
            SetError(@l, 'Line is too long. Increase MAX_TOKENS to allow longer lines', tokens - 1);
            exit(l);
        end;

        tokens := tokens + 1;
        l.tokens[tokens].token := LowerCase(token);
        l.tokensLen := tokens;
    end;

    TokenizeLexeme := l;
end;

function IsRegisterSegment(token : string) : boolean;
begin
    IsRegisterSegment := 
        (token = 'ds') or (token = 'cs') or (token = 'fs') or 
        (token = 'gs') or (token = 'es') or (token = 'ss');
end;

function IsRegister8(token : string) : boolean;
begin
    IsRegister8 := 
        (token = 'al') or (token = 'bl') or (token = 'cl') or (token = 'dl') or
        (token = 'ah') or (token = 'bh') or (token = 'ch') or (token = 'dh');
end;

function IsRegister16(token : string) : boolean;
begin
    IsRegister16 := 
        (token = 'ax') or (token = 'bx') or (token = 'cx') or (token = 'dx') or
        (token = 'si') or (token = 'di') or (token = 'sp') or (token = 'bp');
end;

function IsRegister32(token : string) : boolean;
begin
    IsRegister32 := 
        (token = 'eax') or (token = 'ebx') or (token = 'ecx') or (token = 'edx') or
        (token = 'esi') or (token = 'edi') or (token = 'esp') or (token = 'ebp');
end;

function IsNumberBin(token : string) : boolean;
var start, i : integer;
begin
     if length(token) < 2 then
        exit(false);
        
    IsNumberBin := true;

    if token[length(token)] <> 'b' then 
        IsNumberBin := false
    else
    begin
        if token[1] = '-' then start := 2
        else start := 1;

        for i := start to length(token) - 1 do 
        begin
            if (token[i] <> '1') and (token[i] <> '0') then 
            begin
                IsNumberBin := false;
                break;    
            end;
        end;
    end;    
end;

function IsNumberDec(token : string) : boolean;
var start, i : integer;
begin
    IsNumberDec := true;

    if token[1] = '-' then start := 2
    else start := 1;

    for i := start to length(token) do 
    begin
        if (token[i] < '0') or (token[i] > '9') then 
        begin
            IsNumberDec := false;
            break;    
        end;
    end;
end;

function IsNumberHex(token : string) : boolean;
var 
    start, i : integer;
    ok : boolean;

begin
    if length(token) < 2 then
        exit(false);

    IsNumberHex := true;

    if token[length(token)] <> 'h' then 
        IsNumberHex := false
    else
    begin
        if token[1] = '-' then start := 2
        else start := 1;
        for i := start to length(token) - 1 do 
        begin
            ok := (token[i] >= '0') and (token[i] <= '9');
            ok := ok or ((token[i] >= 'a') and (token[i] <= 'f'));

            if not ok then 
            begin
                IsNumberHex := false;
                break;    
            end;
        end;
    end;   
end;

function IsIdentifier(token : string) : boolean;
var 
    i : integer;
    ok : boolean;

begin
    if length(token) > 8 then
        exit(false);
    
    IsIdentifier := true;

    for i := 1 to length(token) do 
    begin
        ok := (token[i] >= '0') and (token[i] <= '9');
        if ok and (i = 1) then
        begin
            IsIdentifier := false;
            break;    
        end;

        ok := ok or ((token[i] >= 'a') and (token[i] <= 'z'));

        if not ok then 
        begin
            IsIdentifier := false;
            break;    
        end;
    end;
end;

function IsInstruction(token : string) : boolean;
begin
    IsInstruction := 
        (token = 'sahf') or (token = 'sar') or (token = 'neg') or
        (token = 'bt') or (token = 'imul') or (token = 'test') or
        (token = 'mov') or (token = 'xor') or (token = 'jns') or 
        (token = 'jmp');
end;

function TokenTypeToName(tokenType : TType) : string;
begin
    if tokenType = ModelDirective then result := 'ModelDirective'
    else if tokenType = DataDirective then result := 'DataDirective'
    else if tokenType = CodeDirective then result := 'CodeDirective'
    else if tokenType = EndDirective then result := 'EndDirective'
    else if tokenType = DbDirective then result := 'DbDirective'
    else if tokenType = DwDirective then result := 'DwDirective'
    else if tokenType = DdDirective then result := 'DdDirective'
    else if tokenType = Register8 then result := 'Register8'
    else if tokenType = Register16 then result := 'Register16'
    else if tokenType = Register32 then result := 'Register32'
    else if tokenType = Instruction then result := 'Instruction'
    else if tokenType = Identifier then result := 'Identifier'
    else if tokenType = Unknown then result := 'Unknown'
    else if tokenType = Symbol then result := 'Symbol'
    else if tokenType = NumberBin then result := 'NumberBin'
    else if tokenType = NumberDec then result := 'NumberDec'
    else if tokenType = NumberHex then result := 'NumberHex'
    else if tokenType = RegisterSeg then result := 'RegisterSeg'
    else result := 'lol';
end;

function DetermineTokenType(token : Token) : TType;
begin
    if token.token = '.model' then result := ModelDirective
    else if token.token = '.data' then result := DataDirective
    else if token.token = '.code' then result := CodeDirective
    else if token.token = 'end' then result := EndDirective
    else if token.token = 'db' then result := DbDirective
    else if token.token = 'dw' then result := DwDirective
    else if token.token = 'dd' then result := DdDirective
    else if IsRegister8(token.token) then result := Register8
    else if IsInstruction(token.token) then result := Instruction
    else if IsRegister16(token.token) then result := Register16
    else if IsRegister32(token.token) then result := Register32
    else if IsRegisterSegment(token.token) then result := RegisterSeg
    else if IsNumberBin(token.token) then result := NumberBin
    else if IsNumberDec(token.token) then result := NumberDec
    else if IsNumberHex(token.token) then result := NumberHex
    else if IsIdentifier(token.token) then result := Identifier
    else if IsSplitter(token.token[1]) then result := Symbol
    else result := Unknown;
end;


function IsInstructionToken(token : Token) : boolean;
var t : TType;
begin
    t := token.tokenType;
    IsInstructionToken := 
        (t = ModelDirective) or (t = DataDirective) or (t = CodeDirective) or
        (t = EndDirective) or (t = DbDirective) or (t = DwDirective) or 
        (t = DdDirective) or (t = Instruction);
end;

procedure DetermineStructure(lexeme : PLexeme);
var offset, comaIndex, i: integer;
begin
    offset := 1;
    if lexeme.tokensLen = 0 then
        exit;
    
    if lexeme.tokens[1].tokenType = Identifier then
    begin
        offset := 2;
        lexeme.hasName := true;
        if (lexeme.tokensLen = 2) and (lexeme.tokens[2].token = ':') then
        begin
            offset := 3;
        end; 
    end; 

    if lexeme.tokensLen = offset - 1 then
    begin
        if offset = 2 then
        begin
            SetError(lexeme, 'Name without instruction or directive', offset);
        end;
        exit;        
    end;

    if not IsInstructionToken(lexeme.tokens[offset]) then
    begin
        SetError(lexeme, 'Expected instruction or directive', offset);
        exit;        
    end;

    lexeme.hasMnem := true;
    lexeme.mnemIndex := offset;
    offset := offset + 1;

    if lexeme.tokensLen = offset - 1 then
        exit;      

    comaIndex := 0;
    for i := offset to lexeme.tokensLen do
        if lexeme.tokens[i].token = ',' then 
        begin
            comaIndex := i;
            break;
        end; 

    lexeme.op1Index := offset;
    if comaIndex = 0 then 
    begin
        lexeme.opCount := 1;
        lexeme.op1Len := lexeme.tokensLen - offset + 1
    end 
        else 
    begin
        lexeme.opCount := 2;
        lexeme.op1Len := comaIndex - offset;
        lexeme.op2Index := comaIndex + 1;
        lexeme.op2Len := lexeme.tokensLen - comaIndex;
    end;    
end;

procedure PrintTokens(lexeme : Lexeme; var outFile : TextFile);
var 
    i : integer;
    structure : string;

begin
    writeln(outFile, 'source: ', lexeme.lexemeLine);

    write(outFile, 'tokens: ');
    for i := 1 to lexeme.tokensLen do
    begin
        write(outFile, 
            ' ( "', lexeme.tokens[i].token, '" | ', TokenTypeToName(lexeme.tokens[i].tokenType), ' | ', i, ' ), ');
        
    end; 
    writeln(outFile);

    if lexeme.hasName then structure := ' 1 | '
    else structure := ' -- | ' ;

    write(outFile, 'struct:  |' , structure);

    if lexeme.hasMnem then
    begin
        write(outFile, lexeme.mnemIndex, '  1 | ');

        if lexeme.opCount >= 1 then 
            write(outFile, lexeme.op1Index, '  ', lexeme.op1Len, ' | ');
        if lexeme.opCount > 1 then
            write(outFile, lexeme.op2Index, '  ', lexeme.op2Len, ' | ');
    end;

    writeln(outFile);
    writeln(outFile);
end;

function Tokenize(InputFile : string; var outFile : TextFile; output : boolean) : ASMStorage;
var 
    f : TextFile;
    t : TType;
    lineCounter, i : integer;
    line : string;
    currentLexeme : Lexeme;
    storage : ASMStorage;

begin
    storage.lexemesLen := 0;
    storage.codeSegmentIndex := 0;
    storage.dataSegmentIndex := 0;
    storage.userNamesLen := 0;

    lineCounter := 0;

    AssignFile(f, InputFile);
    Reset(f);
    
    while not eof(f) do
    begin
        lineCounter := lineCounter + 1;

        readln(f, line);
        currentLexeme := TokenizeLexeme(line, lineCounter);
        
        for i := 1 to currentLexeme.tokensLen do 
        begin
            if currentLexeme.hasError then
                continue;
            
            t := DetermineTokenType(currentLexeme.tokens[i]);
            if t = Unknown then
                SetError(@currentLexeme, 'Unkown token type', i)
            else 
                currentLexeme.tokens[i].tokenType := t;
        end;
            
        if not currentLexeme.hasError then
            DetermineStructure(@currentLexeme);
        
     
        
        if lineCounter >= MAX_LEXEMES then
        begin
            writeln('Too many lexemes =c');
            continue;
        end;
        storage.lexemes[lineCounter] := currentLexeme;

        if output then
            PrintTokens(currentLexeme, outFile);
    end;
    
    storage.lexemesLen := lineCounter;

    CloseFile(f);
    Tokenize := storage;
end;

end.