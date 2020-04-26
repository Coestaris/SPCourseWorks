unit FirstPass;
{$I DefMacro.inc}

interface

uses Base;

procedure DoFirstPass(storage : PASMStorage);

implementation

uses SysUtils;
    
function GetLexemeType(lexeme : Lexeme) : LType;
begin
    if (lexeme.tokensLen = 1) and (lexeme.tokens[1].tokenType = ModelDirective) then
        exit(ModelSpecifier);

    if (lexeme.tokensLen = 1) and (lexeme.tokens[1].tokenType = DataDirective) then
        exit(DataSpecifier);
    
    if (lexeme.tokensLen = 1) and (lexeme.tokens[1].tokenType = CodeDirective) then
        exit(CodeSpecifier);
    
  
    if (lexeme.tokensLen = 3) and 
        (lexeme.tokens[1].tokenType = CodeDirective) and
        (lexeme.tokens[2].tokenType = CodeDirective) and
        (lexeme.tokens[3].tokenType = CodeDirective) then
        exit(CodeSpecifier);
    
end;

procedure DoFirstPass(storage : PASMStorage);
var i : integer;
begin
    // Determine lexeme types
    for i := 1 to storage.lexemesLen do 
    begin
        if storage.lexemes[i].hasError then     
            continue;

        storage.lexemes[i].lexemeType := GetLexemeType(storage.lexemes[i]);
        if storage.lexemes[i].lexemeType = LUnknown then
        begin
            SetError(@(storage.lexemes[i]), 'Unkown lexeme type', MAX_TOKENS);
            continue;
        end;
    end;

    // Check usernames

    // Calculate size
end;

end.