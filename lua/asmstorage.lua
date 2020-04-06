local asm_storage = {}

--
-- Creates asm storage object. That objects stores all information about current assembly
--
function asm_storage.create_asm_storage()
    asm_storage = {}

    -- List of lines that contains errors. If there's error in line, it will be ignored
    asm_storage.error_lines = {}
    return asm_storage
end 

return asm_storage