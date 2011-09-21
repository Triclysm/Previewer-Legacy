-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
-- *                                                                                     *
-- *                                   Animation Base                                    *
-- *                                      TRICLYSM                                       *
-- *                                                                                     *
-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
-- *                                                                                     *
-- *  This file contains the basic functions for initializing an animation, as well as   *
-- *  any constants that can be used with functions registered to the Lua animation.     *
-- *                                                                                     *
-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
-- *                                                                                     *
-- *  Copyright (C) 2011 Brandon Castellano, Ryan Mantha. All rights reserved.           *
-- *  Triclysm is provided under the BSD-2-Clause license. This program uses the SDL     *
-- *  (Simple DirectMedia Layer) library, and the Lua scripting language. See the        *
-- *  included LICENSE file or <http://www.triclysm.com/> for more details.              *
-- *                                                                                     *
-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
-- *                                      CONSTANTS                                      *
-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

X_AXIS   = 0    -- Represents the x-axis.
Y_AXIS   = 1    -- Represents the y-axis.
Z_AXIS   = 2    -- Represents the z-axis.

YZ_PLANE = 0    -- Represents the yz-plane.
ZX_PLANE = 1    -- Represents the zx-plane.
XY_PLANE = 2    -- Represents the xy-plane.

COLOR_R  = 0    -- Represents the color red.
COLOR_G  = 1    -- Represents the color green.
COLOR_B  = 2    -- Represents the color blue.

RGB_VALS = -1   --
RGB_HEX  = -2   --

sx = 0              -- The cube size in the x-dimension (set by InitSize).
sy = 0              -- The cube size in the y-dimension (set by InitSize).
sz = 0              -- The cube size in the z-dimension (set by InitSize).

_numColors = -1     -- The number of colors of the animation (set by SetNumColors).
_setColors = false  -- Set to true after the call of SetNumColors.

OAXIS              = {}         -- The other axis helper.  Allows you to retrieve the 
OAXIS[YZ_PLANE]    = {}         -- constant representing the two axis given a specified
OAXIS[ZX_PLANE]    = {}         -- plane. For example, the value of OAXIS[XY_PLANE][0] is
OAXIS[XY_PLANE]    = {}         -- the X_AXIS, and OAXIS[XY_PLANE][1] is the Y_AXIS.

OAXIS[YZ_PLANE][0] = Y_AXIS
OAXIS[YZ_PLANE][1] = Z_AXIS

OAXIS[ZX_PLANE][0] = Z_AXIS
OAXIS[ZX_PLANE][1] = X_AXIS

OAXIS[XY_PLANE][0] = X_AXIS
OAXIS[XY_PLANE][1] = Y_AXIS


-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
-- *                                      FUNCTIONS                                      *
-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

-- Called when the animation is created, this stores the cube size in the above variables.
function _InitSize(sizeX, sizeY, sizeZ)
    sx = sizeX
    sy = sizeY
    sz = sizeZ
end

-- Sets the number of colors in the animation.  Must be called at least once, and all
-- subsequent calls are simply ignored.
function SetNumColors(colors)
    if not _setColors then
        _numColors = colors
        _setColors = true
    end
end
