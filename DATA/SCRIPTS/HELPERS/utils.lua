Utils = {}
Utils["writeVariables"] = function()
    filename = "lua.variables.log"
    remove(filename)
	local LogVars
    LogVars = function(x, y)
        local file = appendto(filename)
        write(file, tostring(x).." ( "..tostring(y).." )\n")
        flush(file)
        closefile(file)
    end
    foreachvar(LogVars)
	filename = nil
end