loadfile("animbase.lua")(); SetNumColors(3)

cx = 0
cy = 0
cz = 0

function Initialize()
    math.randomseed(os.time())
    SetPlaneRandom(0)
    GetNextRandVoxel(0)
    return true
end

function GetNextRandVoxel(offset)
    if not ComparePlaneColor(XY_PLANE, offset, 0x000000) then
        print(cz)
        cz = offset
        repeat
            cx = math.random(0, sx - 1)
            cy = math.random(0, sy - 1)
        until (GetVoxelColor(cx, cy, cz, RGB_HEX) ~= 0x000000)
        return true
    else
        return false
    end
end


function GetRandomColor()
    return math.random(0, 0xFFFFFF)
end


function SetPlaneRandom(offset)
    for x = 0, (sx - 1) do
        for y = 0, (sy - 1) do
            SetVoxelColor(x, y, offset, GetRandomColor())
        end
    end
end

cdir = 1

function Update()
    tmpVoxelColor = GetVoxelColor(cx, cy, cz, RGB_HEX)
    print(tmpVoxelColor)
    if (cz < (sz - 1) and cdir == 1) or (cz > 0 and cdir == -1) then
        SetVoxelColor(cx, cy, cz, 0x000000)
    else
        SetVoxelColor(cx, cy, cz, GetRandomColor())
    end
    cz = cz + cdir
    if (cz < sz) and (cz >= 0) then
        SetVoxelColor(cx, cy, cz, tmpVoxelColor);
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
