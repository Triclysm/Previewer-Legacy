loadfile("animbase.lua")(); SetNumColors(0)

planes = {}
plane  = 0
level  = 0
s_flag = false
dir    = -1

function Initialize()

    planes[YZ_PLANE] = sx
    planes[ZX_PLANE] = sy
    planes[XY_PLANE] = sx    

    WriteConsole("Loaded scan.lua v1.0");

    return true
end

function Update() 
    SetPlaneState(plane, level, false)

    if (s_flag) then
        plane = (plane + 1) % 3
        if (plane == 0 and dir == 1) then 
            dir = -1             
        elseif (plane == 0 and dir == -1) then 
            dir = 1 
            DoneIteration()
        end
    end

    level = (level + dir) % planes[plane]

    SetPlaneState(plane, level, true)

    if (level == planes[plane] - 1 and dir == 1) then
        s_flag = true
    elseif (level == 0 and dir == -1) then
        s_flag = true
    else
        s_flag = false
    end
end
