loadfile("animbase.lua")(); SetNumColors(0)

moves = {[0]=
        {[0]=  1,  0,  0},
        {[0]=  0,  1,  0},
        {[0]=  0,  0,  1},
        {[0]= -1,  0,  0},
        {[0]=  0, -1,  0},
        {[0]=  0,  0, -1}}

pos      = nil
reset	 = nil
init     = nil
stats    = nil
path	 = nil
vol		 = nil
it		 = nil
fillFile = nil
avgFile  = nil

function Initialize(_stats)
    if (_stats == 1) then stats = true
    else states = false end
    
    local _time = os.time()	
    math.randomseed(_time)	
    
    if (stats) then InitStats(_time) end
    
    reset = true
    init  = false

    WriteConsole("Loaded randomWalk.lua v1.0");	
    return true
end

function Update()
    if (reset) then ResetAnimation() end    
    SetVoxelState(pos[0], pos[1], pos[2], true)
    path = path + 1	    
    pos = GetNext()	
end

function GetNext()	
    cnt, adjacent = GetEmptyAdjacent(pos)    
    if(cnt > 0) then return adjacent[math.random(0, cnt-1)] end    
    reset = true    
    return {}
end

function GetEmptyAdjacent(coords)
    local cnt = 0
    local array = {}
    
    for i = 0, 5 do
        newPos = VectorSum(moves[i], coords)
        if(ValidEmpty(newPos)) then
            array[cnt] = newPos
            cnt = cnt + 1
        end
    end
    
    return cnt, array
end

function VectorSum(a, b)
    return {[X_AXIS] = a[X_AXIS] + b[X_AXIS],
            [Y_AXIS] = a[Y_AXIS] + b[Y_AXIS],
            [Z_AXIS] = a[Z_AXIS] + b[Z_AXIS]}
end

function ValidEmpty(coords)
    for axis = X_AXIS, Z_AXIS do
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
    
     if (init and stats) then WriteStats() end
    
     path  = 0
     reset = false	
     init  = true
     DoneIteration()
end

function InitStats(_time)
    it    = 0
    avg   = 0
    path  = 0
    vol   = (sx*sy)*sz

    fillFile = "Fill_Data.txt"
    avgFile  = "Avg_Data.txt"

    local file = assert(io.open(avgFile, "a+"))
    file:write("\n\nSeed [".._time.."], 0")
    file:close()
end

function WriteStats()
    local file = nil
    local tmp = math.round ((path/vol)*100, 2, "ceil")		
    local avg = (avg*it) + tmp

    it  = it + 1		
    avg = math.round(avg/it, 2, "ceil")
                
    file = assert(io.open(fillFile, "a+"))
    file:write(tmp.."\n")
    file:close()
        
    file = assert(io.open(avgFile, "a+"))
    file:write(", "..avg)
    file:close()
end