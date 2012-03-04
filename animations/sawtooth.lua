loadfile("animbase.lua")(); SetNumColors(0)

cz = 0
cc = 1

function Initialize()
    return true
end


function Update()
    Shift(ZX_PLANE, 1)
    SetColumnState(X_AXIS, 0, cz, true)
    cz = cz + cc
    if (cz < 0) then
        cc = 1
        cz = 0 + cc
    elseif (cz >= sz) then
        cc = -1
        cz = (sz - 1) + cc
    end
end
