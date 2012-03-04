loadfile("animbase.lua")(); SetNumColors(0)

tick 	= 0
size 	= 0
speed 	= 0
corner 	= nil
mSize	= nil
state	= true

bitField = {{false,false,false},	{true,false,false},
			{false,true,false},		{true,true,false},
			{false,false,true},		{true,false,true},
			{false,true,true},		{true,true,true}}

function Initialize(_speed)
	math.randomseed(os.time())

	speed 	= _speed
	mSize 	= MaxSize()
	corner = math.random(1, 8)
	
    WriteConsole("Loaded box.lua v1.0");

    return true
end

function Update()
	if((tick % speed) == 0) then
		if ( size == 0  and tick ~= 0) then 
			corner = math.random(1, 8)
			state = not state
		end

		for x = 0, math.min(size, sx-1) do
			for y = 0, math.min(size, sy-1) do
				for z = 0, math.min(size, sz-1) do
					SetVoxelState(	T(X_AXIS, x), 
									T(Y_AXIS, y), 
									T(Z_AXIS, z),
									state)
				end
			end
		end

		size = (size + 1) % mSize
	end
    tick = tick + 1
end

-- Translated Voxels to proper position
function  T(axis, coord)
	if( bitField[corner][axis + 1]) then 
		return (__AXIS[axis] - 1) - coord
	else
		return coord
	end
end