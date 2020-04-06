local error = {}

--
-- Prints compiler error and aborts program 
--
function error.comp_error(file, msg)
    print(string.format("[Error]: Internal compiler error at file %s: %q", file, msg))
    os.exit(1)
end

--
-- Prints source error 
--
function error.error(file, line, pos, msg)
    print(string.format("[ERROR]: Error at file %s:%i at position %i: %q", file, line, pos, msg))
end

return error