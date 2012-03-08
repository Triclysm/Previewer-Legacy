loadfile("animbase.lua")(); SetNumColors(0)


ccx = sx - 1
cd = -1


function Initialize()
    -- do check in the future to ensure the cube is a box,
    -- and needs even dimensions - not???
    ccx = sx - 1
    cd = -1
    return true
end

function DrawBox()
    -- draw box of size (cmax-ccx) in bottom corner.

    Shift(XY_PLANE, sx)
    
    -- BOTTOM PLANE
    SetColumnState(X_AXIS, 0, 0, true)
    SetColumnState(Y_AXIS, ccx, 0, true)
    SetColumnState(X_AXIS, ccx, 0, true)
    SetColumnState(Y_AXIS, 0, 0, true)
    
    -- TOP PLANE
    SetColumnState(X_AXIS, 0, ccx, true)
    SetColumnState(Y_AXIS, ccx, ccx, true)
    SetColumnState(X_AXIS, ccx, ccx, true)
    SetColumnState(Y_AXIS, 0, ccx, true)
    
    -- VERTICAL COLUMNS
    
    SetColumnState(Z_AXIS, 0, 0, true)
    SetColumnState(Z_AXIS, 0, ccx, true)
    SetColumnState(Z_AXIS, ccx, 0, true)
    SetColumnState(Z_AXIS, ccx, ccx, true)
    
    -- Finally, zero out the rest of everything.
    csx = ccx + 1
    
    while (csx < sx) do
        SetPlaneState(XY_PLANE, csx, false)
        SetPlaneState(YZ_PLANE, csx, false)
        SetPlaneState(ZX_PLANE, csx, false)
        csx = csx + 1
    end
    
    
    --csx = ccx
end


function Update()
    DrawBox()
    ccx = ccx + cd
    if (ccx < 0) then
        ccx = 0
        cd = 1
    elseif (ccx >= sx) then
        ccx = sx - 1
        cd = -1
    end
end
