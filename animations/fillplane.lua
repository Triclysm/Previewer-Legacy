animbase = loadfile("animbase.lua")
animbase()

cx = 0
cy = 0
cz = 0

function Initialize()
    math.randomseed(os.time())
    GetNextRandVoxel()
    SetVoxelState(cx, cy, cz, true)
    return true
end

function GetNextRandVoxel()
    if not GetPlaneState(XY_PLANE, sz - 1, true) then
        cz = sz - 1
        repeat
            cx = math.random(0, sx - 1)
            cy = math.random(0, sy - 1)
        until not GetVoxelState(cx, cy, cz)
    else
        return false
    end
end


cdir = 1
finished = false

function Update()
    if not finished then
    -- Is the voxel below the current one on?
    if (cz > 0 and GetVoxelState(cx, cy, cz - 1)) then
            finished = GetNextRandVoxel()
    else
        if cz > 0 then SetVoxelState(cx, cy, cz, false) end
        cz = cz - 1
        if cz >= 0 then
            SetVoxelState(cx, cy, cz, true)
        else
            finished = GetNextRandVoxel()
        end
    end
    -- .
    end
end
