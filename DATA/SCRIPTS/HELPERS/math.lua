Math = {}
Math["round"] = function (num, numDecimalPlaces)
    local mult = 10^(numDecimalPlaces or 0)
    return floor(num * mult + 0.5) / mult
end

function sphereCollision(a, b)
    local ax = a.center[1]
    local ay = a.center[2]
    local az = a.center[3]

    local bx = b.center[1]
    local by = b.center[2]
    local bz = b.center[2]

    local threshold = (a.radius + b.radius) * (a.radius + b.radius)

    local dx = bx - ax
    local dy = by - ay
    local dz = bz - az
  
    local distSq = dx * dx + dy * dy + dz * dz

    return distSq < threshold
end

-- Default functions
Math["abs"] = abs
Math["asin"] = asin
Math["atan"] = atan
Math["atan2"] = atan2
Math["ceil"] = ceil
Math["cos"] = cos
Math["sin"] = sin
Math["tan"] = tan
Math["deg"] = deg
Math["floor"] = floor
Math["log"] = log
Math["log10"] = log10
Math["max"] = max
Math["min"] = min
Math["mod"] = mod
Math["sqrt"] = sqrt
Math["frexp"] = frexp
Math["ldexp"] = ldexp
Math["random"] = random
Math["randomseed"] = randomseed