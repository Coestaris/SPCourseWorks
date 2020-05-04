local common = {}

--
--
--
function common.trim(s)
    return (s:gsub("^%s*(.-)%s*$", "%1"))
end

--
--
--
function common.is_empty(s)
    return s == nil or s == ''
end

--
-- Checks if table has specified value by default comparator
--
function common.has_value(tab, val)
    for index, value in ipairs(tab) do
        if value == val then
            return true
        end
    end

    return false
end

function common.padleft(s, l, c)
    s = string.format('%X', s)
    local res = string.rep(c or ' ', l - #s) .. s
    return res
end

return common