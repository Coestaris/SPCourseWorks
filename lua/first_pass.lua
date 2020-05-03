local first_pass = {}

local types = require("types")
local error = require("error")

local global_fn = ""
local global_storage = {}

--
-- Redeclare compiler and source errors for current file
--
local function comp_error(msg) 
    error.comp_error("et1.lua", msg) 
end
local function source_error(line, char, msg) 
    error.error(global_fn, line, char, msg)
    table.insert(global_storage.error_lines, line) 
end

local function get_lexeme_type(tokens, token_types)
    assert(type(tokens) == "table")
    assert(type(token_types) == "table")

    if #tokens == 2 and token_types[1] == types.tt_identifier and tokens[2] == "SEGMENT" then
        return types.lt_segment_def;
    end;

    return types.lt_wrong 
end

function first_pass.do_first_pass(fn, i, line, tokens, token_types, structure, storage, et2_print)
    assert(type(fn) == "string")
    assert(type(i) == "number")
    assert(type(line) == "string")
    assert(type(tokens) == "table")
    assert(type(token_types) == "table")
    assert(type(structure) == "table")
    assert(type(storage) == "table")
    assert(type(et2_print) == "boolean")

    global_fn = fn
    global_storage = storage

    -- determine lexeme type
    local type = get_lexeme_type(tokens, token_types)
    if type == types.lt_wrong then 
        source_error(i, 1, "Unknown instruction type")
    end;

    -- proceed segments, labels, assume, variables

    -- check constants, bits

    -- check instructions

    -- calculate size


    if et2_print then 
        print(line)
    end;
end

return first_pass