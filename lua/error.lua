local error = {}

--
-- Prints compiler error and aborts program 
--
function error.comp_error(file, msg)
    assert(type(file) == "string")
    assert(type(msg) == "string")

    print(string.format("[Error]: Internal compiler error at file %s: %q", file, msg))
    os.exit(1)
end

--
-- Prints source error 
--
function error.error(file, line, pos, msg)
    assert(type(file) == "string")
    assert(type(msg) == "string")
    assert(type(line) == "number")
    assert(type(pos) == "number")

    print(string.format("[ERROR]: Error at file %s:%i at position %i: \"%s\"", file, line, pos, msg))
end

return error