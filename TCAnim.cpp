/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                            Animation Object Source Code                             *
 *                                      TRICLYSM                                       *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *  This file contains the implementation of the TCAnim class as defined by the        *
 *  TCAnim.h header file.  This class is the base for any cube animation objects.      *
 *  This class uses the TCCube class to hold the animation's current cube state.       *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *  Copyright (C) 2011 Brandon Castellano, Ryan Mantha. All rights reserved.           *
 *  Triclysm is provided under the BSD-2-Clause license. This program uses the SDL     *
 *  (Simple DirectMedia Layer) library, and the Lua scripting language. See the        *
 *  included LICENSE file or <http://www.triclysm.com/> for more details.              *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///
/// \file  TCAnim.cpp
/// \brief This file contains the implementation of the TCAnim class as defined by the
///        TCAnim.h header file.
///

#include "TCAnim.h"
#include <cstdlib>      // Used for pointer NULL define value.


///
/// \brief Cubic Constructor
///
/// Creates a TCAnim object where the voxels in each dimension of the cube state span
/// from 0 to cubeSize-1.  This will also allocate the internal cubeState object, and
/// store the size of the cube in the internal sc[3] array.
///
/// \param cubeSize The size (in voxels) of each dimension.
/// \see   cubeState
///
TCAnim::TCAnim(byte cubeSize, byte colors)
{
    numColors = colors;
    if (colors == 0) colors = 1;
    cubeState = new TCCube*[colors];
    for (byte i = 0; i < colors; i++)
    {
        cubeState[i] = new TCCube(cubeSize);
    }
    sc[0] = sc[1] = sc[2] = cubeSize;
    iterations = ticks = 0;
}


///
/// \brief Rectangular Prism Constructor
///
/// Creates a TCAnim object where the voxels in each dimension of the cube state span
/// from 0 to each passed dimension (sizeX, sizeY, sizeZ) minus 1.  This will also
/// allocate the internal cubeState object, and store the size of the cube in the
/// internal sc[3] array.
///
/// \param sizeX The size (in voxels) of the x-dimension.
/// \param sizeY The size (in voxels) of the y-dimension.
/// \param sizeZ The size (in voxels) of the z-dimension.
/// \see   cubeState | sc
///
TCAnim::TCAnim(byte sizeX, byte sizeY, byte sizeZ, byte colors)
{
    numColors = colors;
    if (colors == 0) colors = 1;
    cubeState = new TCCube*[colors];
    for (byte i = 0; i < colors; i++)
    {
        cubeState[i] = new TCCube(sizeX, sizeY, sizeZ);
    }
    //cubeState = new TCCube(sizeX, sizeY, sizeZ);
    sc[0] = sizeX; sc[1] = sizeY; sc[2] = sizeZ;
    iterations = ticks = 0;
}


///
/// \brief Rectangular Prism Constructor (Array)
///
/// Creates a TCAnim object where the voxels in each dimension of the cube state span from
/// 0 to each passed dimension minus 1.  This will also allocate the internal cubeState
/// object, and store the size of the cube in the internal sc[3] array.
///
/// \param tccSize An array containing the x, y, and z sizes (in voxels). The
///                x-dimension is tccSize[0], y is in tccSize[1], and z is in tccSize[2].
/// \see   cubeState
///
TCAnim::TCAnim(byte tccSize[3], byte colors)
{
    numColors = colors;
    if (colors == 0) colors = 1;
    cubeState = new TCCube*[colors];
    for (byte i = 0; i < colors; i++)
    {
        cubeState[i] = new TCCube(tccSize);
    }
    sc[0] = tccSize[0]; sc[1] = tccSize[1]; sc[2] = tccSize[2];
    iterations = ticks = 0;
}


///
/// \brief Destructor
///
/// Deletes the dynamically allocated TCCube cubeState object.
///
/// \see AllocateCube | pCubeState
///
TCAnim::~TCAnim()
{
    for (byte i = 0; i < numColors; i++)
    {
        delete cubeState[i];
    }
    delete[] cubeState;
}


///
/// \brief Tick? Tock.
///
/// Advances the animation by one step.  This increments the internal tick counter,
/// and calls the animation-defined \ref Update method.
///
/// \see Update | ticks
///
void TCAnim::Tick()
{
    ticks++;
    Update();
}


///
/// \brief Get Iterations
///
/// Method to get the number of iterations the animation has completed.  In other words,
/// the number of times the animation has repeated itself.
///
/// \returns The number of iterations the animation has completed.
/// \see     iterations
///
unsigned int TCAnim::GetIterations()
{
    return iterations;
}


///
/// \brief Get Ticks
///
/// Method to get the number of times the \ref Tick method has been called.  In other
/// words, the number of times the animation has been stepped through.
///
/// \returns The number of ticks the animation has gone through.
/// \see     ticks
///
unsigned int TCAnim::GetTicks()
{
    return ticks;
}


///
/// \brief Get Number of Colors
///
/// Method to return the number of colors in the current animation.
///
/// \returns The number of colors in the animation (0, 1, or 3).
/// \see     numColors
///
byte TCAnim::GetNumColors()
{
    return numColors;
}


///
/// \brief Set Voxel Color (Greyscale)
///
/// Sets the specified voxel's color to the passed greyscale value.
///
/// \param x    The x-coordinate in three-space.
/// \param y    The y-coordinate in three-space.
/// \param z    The z-coordinate in three-space.
/// \param grey The greyscale value (0-255) to set the voxel to.
///
/// \remarks If numColors is 0, the voxel's state is set to 0x01 for a non-zero grey value.
///          If numColors is 3, each R/G/B color value is set to the grey value.
///
void TCAnim::SetVoxelColor(byte x, byte y, byte z, byte grey)
{
    switch (numColors)
    {
        case 0:
            cubeState[0]->SetVoxelState(x, y, z, grey == 0x00 ? 0x00 : 0x01);
            break;
        case 1:
            cubeState[0]->SetVoxelState(x, y, z, grey);
            break;
        case 3:
            SetVoxelColor(x, y, z, grey, grey, grey);
            break;
        default:
            break;
    }
}


///
/// \brief Set Voxel Color (RGB Values)
///
/// Sets the specified voxel's color to the passed red, green, and blue values.
///
/// \param x The x-coordinate in three-space.
/// \param y The y-coordinate in three-space.
/// \param z The z-coordinate in three-space.
/// \param r The value (0-255) of the red color.
/// \param g The value (0-255) of the green color.
/// \param b The value (0-255) of the blue color.
///
/// \remarks If numColors is 0, the voxel state is set to 0x01 if r, g, and b are non-zero.
///          If numColors is 1, the voxel state is set to the average of r, g, and b.
///
void TCAnim::SetVoxelColor(byte x, byte y, byte z, byte r, byte g, byte b)
{
    switch (numColors)
    {
        case 0:
            cubeState[0]->SetVoxelState(x, y, z, 
                (r == 0x00 && g == 0x00 && b == 0x00) == 0x00 ? 0x00 : 0x01);
            break;
        case 1:
            cubeState[0]->SetVoxelState(x, y, z, (r + g + b) / 3);
            break;
        case 3:
            cubeState[TC_COLOR_R]->SetVoxelState(x, y, z, r);
            cubeState[TC_COLOR_G]->SetVoxelState(x, y, z, g);
            cubeState[TC_COLOR_B]->SetVoxelState(x, y, z, b);
            break;
        default:
            break;
    }
}


///
/// \brief Set Voxel Color (RGB Hexadecimal)
///
/// Sets the specified voxel's color to the passed hexadecimal value (e.g. 0xFF0000). This
/// function is mostly a wrapper, which splits the rgbColorValue argument into the proper
/// red, green, and blue values, and passes them to another SetVoxelColor overload.
///
/// \param x             The x-coordinate in three-space.
/// \param y             The y-coordinate in three-space.
/// \param z             The z-coordinate in three-space.
/// \param rgbColorValue The hexadecimal RGB color value (as a 32-bit integer).  Only the
///                      lower 24-bits are considered (the remaining bits are masked off).
///
void TCAnim::SetVoxelColor(byte x, byte y, byte z, ulint rgbColorValue)
{
    SetVoxelColor(x, y, z,
        (byte)((rgbColorValue & 0xFF0000) >> 16), 
        (byte)((rgbColorValue & 0xFF0000) >>  8), 
        (byte)((rgbColorValue & 0xFF0000)) );
}


///
/// \brief Get Voxel Color (Array)
///
/// Returns the color of the passed voxel as an array.  The number of dimensions in the
/// array is equal to the number of colors in the animation.  The return value should be
/// deleted with the delete or delete[] operator after it is not needed.
///
/// \param x          The x-coordinate in three-space.
/// \param y          The y-coordinate in three-space.
/// \param z          The z-coordinate in three-space.
///
/// \returns A pointer to an array containing each color value of the voxel.
///
/// \remarks If numColors is 0, this function will return a NULL pointer.
///
byte* TCAnim::GetVoxelColor(byte x, byte y, byte z)
{
    byte *toReturn = NULL;
    switch (numColors)
    {
        case 1:
            toReturn = new byte;
            *toReturn = cubeState[0]->GetVoxelState(x, y, z);
            break;
        case 3:
            toReturn = new byte[3];
            toReturn[TC_COLOR_R] = cubeState[TC_COLOR_R]->GetVoxelState(x, y, z);
            toReturn[TC_COLOR_R] = cubeState[TC_COLOR_R]->GetVoxelState(x, y, z);
            toReturn[TC_COLOR_R] = cubeState[TC_COLOR_R]->GetVoxelState(x, y, z);
            break;
        default:
            toReturn = NULL;
            break;
    }
    return toReturn;
}


///
/// \brief Set Column Color (Greyscale)
///
/// Sets the specified column's color to the passed greyscale value.
///
/// \param axis The axis to set the column (TC_X_AXIS, TC_Y_AXIS, or TC_Z_AXIS).
/// \param dim1 The first remaining dimension
///                 (e.g. the first character from 'XYZ' if you remove the axis).
/// \param dim2 The second remaining dimension
///                 (e.g. the second character from 'XYZ' if you remove the axis).
/// \param grey The greyscale value (0-255) to set the column to.
///
/// \remarks If numColors is 0, the column's state is set to 0x01 for a non-zero grey value.
///          If numColors is 3, each R/G/B color value is set to the grey value.
///
void TCAnim::SetColumnColor(byte axis, byte dim1, byte dim2, byte grey)
{
    switch (numColors)
    {
        case 0:
            cubeState[0]->SetColumnState(axis, dim1, dim2, grey == 0x00 ? 0x00 : 0x01);
            break;
        case 1:
            cubeState[0]->SetColumnState(axis, dim1, dim2, grey);
            break;
        case 3:
            SetColumnColor(axis, dim1, dim2, grey, grey, grey);
            break;
        default:
            break;
    }
}


///
/// \brief Set Column Color (RGB Values)
///
/// Sets the specified column's color to the passed red, green, and blue values.
///
/// \param axis The axis to set the column (TC_X_AXIS, TC_Y_AXIS, or TC_Z_AXIS).
/// \param dim1 The first remaining dimension
///                 (e.g. the first character from 'XYZ' if you remove the axis).
/// \param dim2 The second remaining dimension
///                 (e.g. the second character from 'XYZ' if you remove the axis).
/// \param r    The value (0-255) of the red color.
/// \param g    The value (0-255) of the green color.
/// \param b    The value (0-255) of the blue color.
///
/// \remarks If numColors is 0, the column state is set to 0x01 if r, g, and b are non-zero.
///          If numColors is 1, the column state is set to the average of r, g, and b.
///
void TCAnim::SetColumnColor(byte axis, byte dim1, byte dim2, byte r, byte g, byte b)
{
    switch (numColors)
    {
        case 0:
            cubeState[0]->SetColumnState(axis, dim1, dim2, 
                (r == 0x00 && g == 0x00 && b == 0x00) == 0x00 ? 0x00 : 0x01);
            break;
        case 1:
            cubeState[0]->SetColumnState(axis, dim1, dim2, (r + g + b) / 3);
            break;
        case 3:
            cubeState[TC_COLOR_R]->SetColumnState(axis, dim1, dim2, r);
            cubeState[TC_COLOR_G]->SetColumnState(axis, dim1, dim2, g);
            cubeState[TC_COLOR_B]->SetColumnState(axis, dim1, dim2, b);
            break;
        default:
            break;
    }
}


///
/// \brief Set Column Color (RGB Hexadecimal)
///
/// Sets the specified column's color to the passed hexadecimal value (e.g. 0xFF0000). This
/// function is mostly a wrapper, which splits the rgbColorValue argument into the proper
/// red, green, and blue values, and passes them to another SetColumnColor overload.
///
/// \param axis          The axis to set the column (TC_X_AXIS, TC_Y_AXIS, or TC_Z_AXIS).
/// \param dim1          The first remaining dimension
///                          (e.g. the first character from 'XYZ' if you remove the axis).
/// \param dim2          The second remaining dimension
///                          (e.g. the second character from 'XYZ' if you remove the axis).
/// \param rgbColorValue The hexadecimal RGB color value (as a 32-bit integer).  Only the
///                      lower 24-bits are considered (the remaining bits are masked off).
///
void TCAnim::SetColumnColor(byte axis, byte dim1, byte dim2, ulint rgbColorValue)
{
    SetVoxelColor(axis, dim1, dim2,
        (byte)((rgbColorValue & 0xFF0000) >> 16), 
        (byte)((rgbColorValue & 0xFF0000) >>  8), 
        (byte)((rgbColorValue & 0xFF0000)) );
}


///
/// \brief Set Plane Color (Greyscale)
///
/// Sets the specified plane's color to the passed greyscale value.
///
/// \param plane  The plane to set the color of the voxels in (either TC_XY_PLANE,
///               TC_ZX_PLANE, or TC_YZ_PLANE).
/// \param offset The offset from 0 to the size-1 of the remaining dimension.
/// \param grey   The greyscale value (0-255) to set the plane to.
///
/// \remarks If numColors is 0, the plane's state is set to 0x01 for a non-zero grey value.
///          If numColors is 3, each R/G/B color value is set to the grey value.
///
void TCAnim::SetPlaneColor(byte plane, byte offset, byte grey)
{
    switch (numColors)
    {
        case 0:
            cubeState[0]->SetPlaneState(plane, offset, grey == 0x00 ? 0x00 : 0x01);
            break;
        case 1:
            cubeState[0]->SetPlaneState(plane, offset, grey);
            break;
        case 3:
            SetPlaneColor(plane, offset, grey, grey, grey);
            break;
        default:
            break;
    }
}


///
/// \brief Set Plane Color (RGB Values)
///
/// Sets the specified plane's color to the passed red, green, and blue values.
///
/// \param plane  The plane to set the color of the voxels in (either TC_XY_PLANE,
///               TC_ZX_PLANE, or TC_YZ_PLANE).
/// \param offset The offset from 0 to the size-1 of the remaining dimension.
/// \param r      The value (0-255) of the red color.
/// \param g      The value (0-255) of the green color.
/// \param b      The value (0-255) of the blue color.
///
/// \remarks If numColors is 0, the plane state is set to 0x01 if r, g, and b are non-zero.
///          If numColors is 1, the plane state is set to the average of r, g, and b.
///
void TCAnim::SetPlaneColor(byte plane, byte offset, byte r, byte g, byte b)
{
    switch (numColors)
    {
        case 0:
            cubeState[0]->SetPlaneState(plane, offset, 
                (r == 0x00 && g == 0x00 && b == 0x00) == 0x00 ? 0x00 : 0x01);
            break;
        case 1:
            cubeState[0]->SetPlaneState(plane, offset, (r + g + b) / 3);
            break;
        case 3:
            cubeState[TC_COLOR_R]->SetPlaneState(plane, offset, r);
            cubeState[TC_COLOR_G]->SetPlaneState(plane, offset, g);
            cubeState[TC_COLOR_B]->SetPlaneState(plane, offset, b);
            break;
        default:
            break;
    }
}


///
/// \brief Set Plane Color (RGB Hexadecimal)
///
/// Sets the specified plane's color to the passed hexadecimal value (e.g. 0xFF0000). This
/// function is mostly a wrapper, which splits the rgbColorValue argument into the proper
/// red, green, and blue values, and passes them to another SetColumnColor overload.
///
/// \param plane         The plane to set the color of the voxels in (either TC_XY_PLANE,
///                      TC_ZX_PLANE, or TC_YZ_PLANE).
/// \param offset        The offset from 0 to the size-1 of the remaining dimension.
/// \param rgbColorValue The hexadecimal RGB color value (as a 32-bit integer).  Only the
///                      lower 24-bits are considered (the remaining bits are masked off).
///
void TCAnim::SetPlaneColor(byte plane, byte offset, ulint rgbColorValue)
{
    SetPlaneColor(plane, offset,
        (byte)((rgbColorValue & 0xFF0000) >> 16), 
        (byte)((rgbColorValue & 0xFF0000) >>  8), 
        (byte)((rgbColorValue & 0xFF0000)) );
}


///
/// \brief Shift Cube
///
/// Shifts all of the voxels in the cube by the specified offset in the direction of the
/// specified plane.  The new voxels that are shifted are set to 0 for all colors.
///
/// \param plane   The plane to shift of the voxels in (either TC_XY_PLANE, TC_ZX_PLANE,
///                or TC_YZ_PLANE).
/// \param offset  The offset (positive or negative) to shift in the plane.
///
/// \remarks This function is a wrapper around the actual cubestate's Shift method. This
///          is useful for RGB animations, since this will shift each color at once.
///
void TCAnim::Shift(byte plane, sbyte offset)
{
    switch (numColors)
    {
        case 0:
        case 1:
            cubeState[0]->Shift(plane, offset);
            break;
        case 3:
            cubeState[TC_COLOR_R]->Shift(plane, offset);
            cubeState[TC_COLOR_G]->Shift(plane, offset);
            cubeState[TC_COLOR_B]->Shift(plane, offset);
            break;
        default:
            break;
    }
}
