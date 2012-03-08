loadfile("animbase.lua")(); SetNumColors(0)

size     = 0
corner   = nil
mSize    = nil
state    = true
init     = false

bitField = {{0,0,0}, {1,0,0}, 
            {0,1,0}, {1,1,0}, 
            {0,0,1}, {1,0,1}, 
            {0,1,1}, {1,1,1}}

function Initialize()
    math.randomseed(os.time())

    mSize  = MaxSize()
    corner = math.random(1, 8)
    
    WriteConsole("Loaded box.lua v1.0");

    return true
end

function Update()
    if (size == 0  and init) then 
        corner = math.random(1, 8)
        state = not state
        DoneIteration()
    end

    for x=0, math.min(size, sx-1) do
        for y=0, math.min(size, sy-1) do
            for z=0, math.min(size, sz-1) do
                SetVoxelState( T(X_AXIS, x), 
                                T(Y_AXIS, y), 
                                T(Z_AXIS, z),
                                state         )
            end
        end
    end

    size = (size + 1) % mSize
    init = true
end

function T(axis, coord)
    if (bitField[corner][axis + 1] == 1) then 
        return (sc[axis] - 1) - coord
    else
        return coord
    end
end