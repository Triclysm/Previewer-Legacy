loadfile("animbase.lua")(); SetNumColors(0)

size     = 0
dir      = 1
corner   = nil
mSize    = nil
init     = false

bitField = {{0,0,0}, {1,0,0}, 
            {0,1,0}, {1,1,0}, 
            {0,0,1}, {1,0,1}, 
            {0,1,1}, {1,1,1}}

function Initialize()
    math.randomseed(os.time())

    mSize  = MaxSize()
    corner = math.random(1, 8)
    
    WriteConsole("Loaded hollowbox.lua v1.0");

    return true
end

function Update()
    if (size == 0  and init) then     
        DoneIteration()
    end

    Draw()

    size = size + dir

    if (size == (mSize -1)) then
        dir = -1
        corner = math.random(1, 8)  
    end

    if (size == 0) then
        dir = 1
    end
 
    init = true
end

function Draw()
    for i = 0, sz do SetPlaneState(XY_PLANE, i, false) end

    for x=0, math.min(size, sx-1) do
        for y=0, math.min(size, sy-1) do
            for z=0, math.min(size, sz-1) do
                SetVoxelState(T(X_AXIS, x), T(Y_AXIS, y), T(Z_AXIS, z), true)
            end
        end
    end

    for x = 1, math.min(size, sx-1)-1 do
        for y = 1, math.min(size, sy-1)-1 do
            SetColumnState(Z_AXIS, T(X_AXIS, x), T(Y_AXIS, y), false)
        end
        for z = 1, math.min(size, sz-1)-1 do
            SetColumnState(Y_AXIS, T(X_AXIS, x), T(Z_AXIS, z), false)
        end
    end

    for y = 1, math.min(size, sy-1)-1 do
        for z = 1, math.min(size, sz-1)-1 do
            SetColumnState(X_AXIS, T(Y_AXIS, y), T(Z_AXIS, z), false)
        end
    end

end

function T(axis, coord)
    if (bitField[corner][axis + 1] == 1) then 
        return (sc[axis] - 1) - coord
    else
        return coord
    end
end