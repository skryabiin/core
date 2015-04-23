

Util = {}

function Util.addPositions(position1, position2)
	local x = position1[1] + position2[1]
	local y = position1[2] + position2[2]
	local z = position1[3] + position2[3]
	return {x, y, z}
end

function Util.subtractPositions(position1, position2)
	local x = position1[1] - position2[1]
	local y = position1[2] - position2[2]
	local z = position1[3] - position2[3]
	return {x, y, z}
end

function Util.printPosition(position)
	return "(" .. position[1] .. "," .. position[2] .. "," .. position[3] .. ")"
end

function Util.bounds(var, lb, ub)
	if var <= lb then
		return lb
	elseif var >= ub then
		return ub
	else 
		return var
	end
end