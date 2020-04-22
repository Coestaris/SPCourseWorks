unit tokenizer;

interface

procedure Tokenize(InputFile : string; OutputFile : string; output : boolean);

implementation

uses Sysutils;

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
    tokensLen : integer;
    tokens : array[1..20] of Token;
    lexemeLine : string;
    lexemeLineIndex : integer;
    
    hasError : boolean;
    errorToken : integer;
    errorMessage : string;
end;


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
    l.hasError := false;

    for i := 1 to length(lexeme) do
    begin
        c := lexeme[i];
        if IsSplitter(c) then   
        begin
            if length(token) <> 0 then
            begin
                tokens := tokens + 1;
                l.tokens[tokens].token := LowerCase(token);
                l.tokensLen := tokens;
            end;

            if (c <> ' ') and (c <> '\n') and (c <> '\t') then
            begin
                SetLength(token, 1);
                token[1] := c;
                
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
            ok := ok or ((token[i] >= 'a') and (token[i] <= 'e'));

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
    start, i : integer;
    ok : boolean;

begin
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
    else if IsNumberBin(token.token) then result := NumberBin
    else if IsNumberDec(token.token) then result := NumberDec
    else if IsNumberHex(token.token) then result := NumberHex
    else if IsIdentifier(token.token) then result := Identifier
    else if IsSplitter(token.token[1]) then result := Symbol
    else result := Unknown;
end;

procedure PrintTokens(lexeme : Lexeme);
var i : integer;
begin
    for i := 1 to lexeme.tokensLen do
    begin
        write(' ( "', lexeme.tokens[i].token, '" | ', TokenTypeToName(lexeme.tokens[i].tokenType), ' ) ,');
    end; 
    writeln;
end;

procedure Lexemize(fileName : string);
var 
    f : TextFile;
    lineCounter, i : integer;
    line : string;
    currentLexeme : Lexeme;

begin
    lineCounter := 0;

    AssignFile(f, fileName);
    Reset(f);
    
    while not eof(f) do
    begin
        lineCounter := lineCounter + 1;

        readln(f, line);
        currentLexeme := TokenizeLexeme(line, lineCounter);
        
        for i := 1 to currentLexeme.tokensLen do 
            currentLexeme.tokens[i].tokenType := DetermineTokenType(currentLexeme.tokens[i]);
            
        PrintTokens(currentLexeme);
    end;
    
    CloseFile(f);
end;

procedure Tokenize(InputFile : string; OutputFile : string; output : boolean);
begin
    Lexemize(InputFile);
end;

end.