-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
-- *                                                                                     *
-- *                                   Animation Base                                    *
-- *                                      TRICLYSM                                       *
-- *                                                                                     *
-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
-- *                                                                                     *
-- *  This file contains the basic required initialization for an animation, as well as  *
-- *  any constants which can be used with the cube modifying functions.                 *
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

sx = 0          -- The cube size in the x-dimension (set by InitSize).
sy = 0          -- The cube size in the y-dimension (set by InitSize).
sz = 0          -- The cube size in the z-dimension (set by InitSize).

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
function InitSize(sizeX, sizeY, sizeZ)
    sx = sizeX
    sy = sizeY
    sz = sizeZ
end
