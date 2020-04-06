local table = require("table")
local pretty = require("pl.pretty")

local error = require("error")
local asm_storage = require("asmstorage")


-- Input file name
local filename = "test.asm"
-- Object storing all asm information
local asm_storage = asm_storage.create_asm_storage()

--
-- Redeclare compiler and source errors for current file
--
local function comp_error(msg) 
    error.comp_error("main.lua", msg) 
end

local function source_error(line, char, msg) 
    error.error(filename, line, char, msg)
    table.insert(asm_storage.error_lines, line) 
end

--
-- Returns true if file exists and accessible
--
local function is_file_exists(file)
    local f = io.open(file, "rb")
    if f then f:close() end
    return f ~= nil
end

--
-- Main compiler routine
--
local function main()
end

main()