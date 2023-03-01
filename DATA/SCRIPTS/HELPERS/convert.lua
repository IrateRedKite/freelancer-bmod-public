Convert = {}
Convert["QuatToMatrix"] = function(quat)

	local x = quat[1]
	local y = quat[2]
	local z = quat[3]
	local w = quat[4]

	local x2 = x + x
	local y2 = y + y
	local z2 = z + z

	local xx = x * x2
	local yx = y * x2
	local yy = y * y2
	local zx = z * x2
	local zy = z * y2
	local zz = z * z2
	local wx = w * x2
	local wy = w * y2
	local wz = w * z2

	local result = {
		{ 1 - yy - zz, yx + wz, zx - wy },
		{ yx - wz, 1 - xx - zz, zy + wx },
		{ zx + wy, zy - wx, 1 - xx - yy }
	}

	return result
end

Convert["EulerToQuat"] = function (x, y, z)

    local b = y * 0.5;
    local c = x * 0.5;
    local d = z * 0.5;
    
    local sp = sin(b)
    local cp = cos(b)
    local sy = sin(c)
    local cy = cos(c)
    local sr = sin(d) 
    local cr = cos(d)
    local a0 = sp * cy * cr
    local b0 = cp * sy * sr
    local a1 = cp * sy * cr
    local b1 = sp * cy * sr
    local a2 = cp * cy * sr
    local b2 = sp * sy * cr
    local a3 = cp * cy * cr
    local b3 = sp * sy * sr
     
    local result = { Math.round(a0 + b0, 5), Math.round(a1 - b1, 5), Math.round(a2 + b2, 5), Math.round(a3 - b3, 5) }
    return result
end


Convert["EulerToMatrix"] = function (x, y, z)
	local q = Convert.EulerToQuat(y, x, z)
	local m = Convert.QuatToMatrix(q)
	return m
end