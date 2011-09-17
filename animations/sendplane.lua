loadfile("animbase.lua")(); SetNumColors(0)

cx = 0
cy = 0
cz = 0

function Initialize()
    math.randomseed(os.time())
    SetPlaneState(XY_PLANE, 0, true)
    GetNextRandVoxel(0)
    return true
end

function GetNextRandVoxel(offset)
    if not GetPlaneState(XY_PLANE, offset, false) then
        cz = offset
        repeat
            cx = math.random(0, sx - 1)
            cy = math.random(0, sy - 1)
        until GetVoxelState(cx, cy, cz)
        return true
    else
        return false
    end
end


cdir = 1

function Update()
    if (cz < (sz - 1) and cdir == 1) or (cz > 0 and cdir == -1) then
        SetVoxelState(cx, cy, cz, false);
    end
    cz = cz + cdir
    if (cz < sz) and (cz >= 0) then
        SetVoxelState(cx, cy, cz, true);
    else
        if (cdir == 1) then
            if not GetNextRandVoxel(0) then
                cdir = -1
                GetNextRandVoxel(sz - 1)
            end
        else
            if not GetNextRandVoxel(sz - 1) then
                cdir =  1
                GetNextRandVoxel(0)
            end
        end
    end
end
