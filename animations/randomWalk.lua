loadfile("animbase.lua")(); SetNumColors(0)

-- Moves Table, initialized to zero index
moves = {[0]=
        {[0]=  1,  0,  0},
        {[0]=  0,  1,  0},
        {[0]=  0,  0,  1},
        {[0]= -1,  0,  0},
        {[0]=  0, -1,  0},
        {[0]=  0,  0, -1}}

speed   = nil
ticks   = nil
pos     = nil
reset   = nil

path    = nil
vol     = nil
trk     = nil

function Initialize(_speed)
    math.randomseed(os.time())    
    
    ticks = -1
    speed = _speed
    reset = true
    
    trk   = 0
    avg   = 0
    path  = 0
    vol   = (sx*sy)*sz
    
    WriteConsole("Loaded randomWalk.lua v1.0");    
    return true
end

function Update()
    ticks = ticks + 1
    if ((ticks % speed) ~= 0) then return end    
    if (reset) then ResetAnimation() end
    
    SetVoxelState(pos[0], pos[1], pos [2], true)
    path = path + 1    
    
    pos = GetNext()    
end

function GetNext()    
    cnt, adjacent = GetEmptyAdjacent(pos)
    
    if(cnt > 0) then         
        return adjacent[math.random(0, cnt-1)]
    end     
    
    reset = true
    
    return {}
end

function GetEmptyAdjacent(coords)
    cnt = 0
    array = {}
    
    for i=0,5 do
        newPos = VectorSum(moves[i], coords)
        if(ValidEmpty(newPos)) then
            array[cnt] = newPos
            cnt = cnt + 1
        end
    end
    
    return cnt, array
end

function VectorSum(a, b)
    return {
               [X_AXIS] = a[X_AXIS] + b[X_AXIS],
               [Y_AXIS] = a[Y_AXIS] + b[Y_AXIS],
               [Z_AXIS] = a[Z_AXIS] + b[Z_AXIS]
           }
end

function ValidEmpty(coords)

    for axis=X_AXIS, Z_AXIS do
        if(coords[axis] >= sc[axis] or coords[axis] < 0) then 
            return false 
        end
    end
    
    return not GetVoxelState(coords[X_AXIS], coords[Y_AXIS], coords[Z_AXIS])
end

function ResetAnimation()    
    pos = {}
    
    for i=X_AXIS, Z_AXIS do pos[i] = math.random(0, sc[i]-1)  end    
    for i=0     , sx-1   do SetPlaneState(YZ_PLANE, i, false) end    
    
    if(ticks ~= 0) then
        tmp = math.round ((path/vol)*100,2,"ceil")
        
        WriteConsole("Fill: ".. tmp .." %")
        
        avg = (avg*trk) + tmp
        trk   = trk + 1
        
        avg = math.round (avg/trk,2,"ceil")
        
        WriteConsole("Avg.: ".. avg .." %  (".. trk ..")\n")
    end
    
    path  = 0
    reset = false    
end