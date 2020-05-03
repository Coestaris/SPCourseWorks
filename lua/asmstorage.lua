local asm_storage = {}

local common = require("common")

--
-- Creates asm storage object. That objects stores all information about current assembly
--
function asm_storage.create_asm_storage()
    asm_storage = {}

    -- List of lines that contains errors. If there's error in line, it will be ignored
    asm_storage.error_lines = {}
    
    asm_storage.segments = {}
    asm_storage.segment_destinations = {}
    
    return asm_storage
end 

function asm_storage.has_error(line, storage)
    return common.has_value(storage.error_lines, line)
end

return asm_storage