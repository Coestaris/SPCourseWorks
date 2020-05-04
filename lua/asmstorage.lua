local asm_storage = {}

local common = require("common")
local pretty = require("pl.pretty")
local types = require("types")

function asm_storage.create_op_info()
    info = {}
    info.type = 0
   
    info.token = ""
    info.token_type = 0

    -- For mem
    info.sum1 = ""
    info.sum1_type = 0
    
    info.sum2 = ""
    info.sum2_type = 0
    
    -- For imm
    info.imm = 0
    return info
end

function asm_storage.create_instruction_info(nmem)
    info = {}
    info.mnem = mnem
    info.op_count = 0

    info.op1 = {}
    info.op2 = {}
    
    return info
end

function asm_storage.create_user_name(type, name)
    user_name = {}  
    user_name.start_line = 0
    user_name.end_line = 0
    user_name.name = name
    user_name.size = 0
    user_name.type = type
    user_name.sub_type = 0
    return user_name
end

--
-- Creates asm storage object. That objects stores all information about current assembly
--
function asm_storage.create_asm_storage()
    asm_storage = {}

    -- List of lines that contains errors. If there's error in line, it will be ignored
    asm_storage.error_lines = {}
    
    asm_storage.segments = {}
    asm_storage.segment_destinations = {
        ["CS"] = {}, 
        ["DS"] = {}, 
        ["ES"] = {}, 
        ["FS"] = {}, 
        ["GS"] = {}, 
        ["SS"] = {},
    }
    asm_storage.user_names = {}
    asm_storage.offsets = {}
    asm_storage.instruction_infos = {}

    function asm_storage.push_user_name(self, user_name)
        table.insert(self.user_names, user_name)
    end
    
    function asm_storage.get_user_name_line(self, type, line)
        for i, v in ipairs(self.user_names) do
            if v.type == type and v.start_line == line then 
                return v
            end
        end
        return nil
    end
    
    function asm_storage.get_user_name(self, type, name)
        for i, v in ipairs(self.user_names) do
            if v.type == type and v.name == name then 
                return v
            end
        end
        return nil
    end

    function asm_storage.has_error(self, line)
        return common.has_value(self.error_lines, line)
    end

    function asm_storage.get_segment_by_line(self, line) 
        for i, v in ipairs(self.user_names) do
            if v.type == types.ut_segment then 
                if v.end_line == 0 then 
                    if line > v.start_line then
                        return v
                    end
                else 
                    if line > v.start_line and line < v.end_line then 
                        return v
                    end
                end
            end
        end
        return nil
    end
    
    function asm_storage.get_destination(self, segment)
        for k, v in pairs(self.segment_destinations) do
            if v.name == segment.name then
                return k
            end
        end
        return nil
    end

    return asm_storage
end 

return asm_storage