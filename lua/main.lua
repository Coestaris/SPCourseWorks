local table = require("table")
local pretty = require("pl.pretty")

local error = require("error")
local asm_storage = require("asmstorage")

local tokenize = require("tokenize")
local first_pass = require("first_pass")

-- Input file name
local filename = "tests/test.asm"
-- Object storing all asm information
local asm_storage = asm_storage.create_asm_storage()

--
-- Redeclare compiler and source errors for current file
--
local function comp_error(msg) 
    error.comp_error("main.lua", msg) 
end

--
-- Returns true if file exists and accessible
--
local function is_file_exists(file)
    assert(type(file) == "string")

    local f = io.open(file, "rb")
    if f then f:close() end
    return f ~= nil
end

--
-- Main compiler routine
--
local function main()
    if not is_file_exists(filename) then
        comp_error(string.format("Unable to open file %q", filename))
    end

    first_pass.register_instructions()

    tokenize.proceed(
        filename, asm_storage, false, 
        first_pass.do_first_pass, true)
end

main()