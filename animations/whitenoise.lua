loadfile("animbase.lua")(); SetNumColors(0)

cz = 0
cc = 1

voxels    = {}
numVoxels = 0


function UpdateVoxels()
    for i=1,numVoxels do
        cx, cy, cz = GetRandVoxel()
        voxels[i][X_AXIS] = cx;
        voxels[i][Y_AXIS] = cy;
        voxels[i][Z_AXIS] = cz;
    end
end


function GetRandVoxel()
    return math.random(0, sx-1), math.random(0, sy-1), math.random(0, sz-1)
end


function Initialize(num_voxels)

    math.randomseed(os.time())
    numVoxels = num_voxels
    for i=1,numVoxels do
        voxels[i] = {}
    end
    
    UpdateVoxels()

    return true
end


function Update()
    -- clear cube
    for i=0,sz-1 do
        SetPlaneState(XY_PLANE, i, false)
    end
    -- redraw voxels
    for i=1,numVoxels do
        SetVoxelState(voxels[i][X_AXIS], voxels[i][Y_AXIS], voxels[i][Z_AXIS], true) 
    end
    
    UpdateVoxels()
end
