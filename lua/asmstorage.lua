local asm_storage = {}

local common = require("common")
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
    info.seg_reg = nil

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
    info.instr_prot = nil
    info.b16mode = False
    
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

local function get_modrm(mod, reg, rm)
    return ((mod & 3) << 6) | ((reg & 7) << 3) | (rm & 7)
end

function asm_storage.create_bytes()
    bytes = {}
    bytes.prefixes = {}
    bytes.opcode = nil
    bytes.modrm = nil
    bytes.sib = nil
    bytes.disp = {}
    bytes.imm = {}

    function bytes.set_opcode(self, opcode)
        self.opcode = opcode
    end

    function bytes.pack_reg(self, reg)
        self.opcode = self.opcode | types.reg_codes[reg]
    end

    function bytes.set_index_prefix(self)
        table.insert(self.prefixes, 0x67)
    end

    function bytes.set_data_prefix(self)
        table.insert(self.prefixes, 0x66)
    end

    function bytes.set_seg_prefix(self, seg)
        table.insert(self.prefixes, types.seg_prefixes[seg])
    end

    function bytes.set_exp_prefix(self)
        table.insert(self.prefixes, 0x0F)
    end 

    function bytes.set_reg_const(self, const)
        if self.modrm == nil then self.modrm = 0 end
        self.modrm = self.modrm | get_modrm(0, const, 0)
    end 

    function bytes.set_reg_reg(self, reg)
        if self.modrm == nil then self.modrm = 0 end
        self.modrm = self.modrm | get_modrm(0, types.reg_codes[reg], 0)
    end 

    function bytes.set_mod_reg(self, reg)
        if self.modrm == nil then self.modrm = 0 end
        self.modrm = self.modrm | get_modrm(11, 0, types.reg_codes[reg])
    end 

    function bytes.set_mod_sum(self, b16, reg1, reg2)
        if self.modrm == nil then self.modrm = 0 end
        if b16 == types.tt_reg16 then
            local rm = 0
            if reg1 == "BX" and reg2 == "SI" then rm = 0
            elseif reg1 == "BX" and reg2 == "DI" then rm = 1
            elseif reg1 == "BP" and reg2 == "SI" then rm = 2
            elseif reg1 == "BP" and reg2 == "DI" then rm = 3
            elseif reg2 == "BX" and reg1 == "SI" then rm = 0
            elseif reg2 == "BX" and reg1 == "DI" then rm = 1
            elseif reg2 == "BP" and reg1 == "SI" then rm = 2
            elseif reg2 == "BP" and reg1 == "DI" then rm = 3
            else print("=c") end
            self.modrm = self.modrm | get_modrm(0, 0, rm)
            self:set_index_prefix()
        else
            self.modrm = self.modrm | get_modrm(0, 0, 4)
            self.sib = get_modrm(0, types.reg_codes[reg2], types.reg_codes[reg1])
        end
    end 

    function bytes.set_imm8(self, imm)
        self.imm = { imm & 0xFF }
    end

    function bytes.set_imm16(self, imm)
        self.imm = { (imm << 8) & 0xFF, (imm << 0) & 0xFF }
    end

    function bytes.set_imm32(self, imm)
        self.imm = { (imm << 24) & 0xFF, (imm << 16) & 0xFF, (imm << 8) & 0xFF, (imm << 0) & 0xFF }
    end

    function bytes.set_imm_long(self, imm)
        for i = 2,#imm do
            table.insert(self.imm, utf8.codepoint(imm:sub(i, i)) & 0xFF)
        end
    end

    function bytes.get_size(self)
        local size = 0
        size = size + #self.prefixes
        if self.opcode ~= nil then size = size + 1 end
        if self.modrm ~= nil then size = size + 1 end
        if self.sib ~= nil then size = size + 1 end
        size = size + #self.disp
        size = size + #self.imm
        return size
    end

    function bytes.get_string(self)
        string = ""
        for k, v in pairs(self.prefixes) do
            string = string .. common.padleft(v, 2, '0') .. ": "
        end
        if self.opcode ~= nil then string = string .. common.padleft(self.opcode, 2, "0") .. " " end
        if self.modrm ~= nil then string = string .. common.padleft(self.modrm, 2, "0") .. " " end
        if self.sib ~= nil then string = string .. common.padleft(self.sib, 2, "0") .. " " end
        if #self.disp ~= 0 then
            for k, b in pairs(self.disp) do
                string = string .. common.padleft(b, 2, '0')
            end
            string = string .. " "
        end
        if #self.imm ~= 0 then
            for _, b in pairs(self.imm) do
                string = string .. common.padleft(b, 2, '0')
            end
            string = string .. " "
        end
        return string
    end

    return bytes
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