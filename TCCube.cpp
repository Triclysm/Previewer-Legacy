/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                             TCCube Object  Source Code                              *
 *                                      TRICLYSM                                       *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *  This file contains the implementation of the TCCube class as defined by the        *
 *  TCCube.h header file.  This class allows a three-dimensional rectangular prism to  *
 *  be represented by discrete voxels, each with a particular state.  The terms cube   *
 *  and rectangular prism may be interchanged depending on the context.                *
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
/// \file  TCCube.cpp
/// \brief This file contains the implementation of the TCCube class as defined by the
///        TCCube.h header file.
///

#include "TCCube.h"
#include <assert.h>     // Used in the CheckVoxelBounds method.


///
/// \brief Cubic Constructor
///
/// Creates a TCCube object where the voxels in each dimension span from 0 to cubeSize-1.
/// This will also allocate the internal \ref pCubeState array to the appropriate size. 
///
/// \param cubeSize The size (in voxels) of each dimension.
/// \see AllocateCube | pCubeState
///
TCCube::TCCube(byte cubeSize)
{
    // We just have to call AllocateCube with all of the dimensions equal to cubeSize.
    AllocateCube(cubeSize, cubeSize, cubeSize);
}


///
/// \brief Rectangular Prism Constructor
///
/// Creates a TCCube object where the voxels in each dimension span from 0 each passed
/// dimension (sizeX, sizeY, sizeZ) minus 1.  This also allocates the internal 
/// \ref pCubeState array to the appropriate size.
///
/// \param sizeX The size (in voxels) of the x-dimension.
/// \param sizeY The size (in voxels) of the y-dimension.
/// \param sizeZ The size (in voxels) of the z-dimension.
/// \see AllocateCube | pCubeState
///
TCCube::TCCube(byte sizeX, byte sizeY, byte sizeZ)
{
    // We just have to pass the given sizes to the AllocateCube method.
    AllocateCube(sizeX, sizeY, sizeZ);
}


///
/// \brief Rectangular Prism Constructor (Array)
///
/// Creates a TCCube object where the voxels in each dimension span from 0 each passed
/// dimension minus 1.  This also allocates the internal  \ref pCubeState array to the
/// appropriate size.  Each element in the tccSize array represents the size of the
/// x, y, and z dimensions respectively.
///
/// \param tccSize An array containing the x, y, and z sizes (in voxels). The
///                x-dimension is tccSize[0], y is in tccSize[1], and z is in tccSize[2].
/// \see AllocateCube | pCubeState
///
TCCube::TCCube(byte tccSize[3])
{
    // We just have to pass the given sizes to the AllocateCube method.
    AllocateCube(tccSize[0], tccSize[1], tccSize[2]);
}


///
/// \brief Copy Constructor
///
/// Creates a new TCCube object which is a clone of the passed one.
///
/// \param toCopy The TCCube object to duplicate.
/// \see AllocateCube | pCubeState
///
TCCube::TCCube(const TCCube &toCopy)
{
    // We first allocate enough memory by using the sizes of the passed cube.
    AllocateCube(toCopy.sc[0], toCopy.sc[0], toCopy.sc[0]);
    // Then, looping through each dimension, we copy all elements of the array from the
    // toCopy object to this object.
    for (int x = 0; x < sc[0]; x++)
    {
        for (int y = 0; y < sc[1]; y++)
        {
            for (int z = 0; z < sc[2]; z++)
            {
                pCubeState[x][y][z] = toCopy.pCubeState[x][y][z];
            }
        }
    }
}


///
/// \brief Destructor
///
/// Deletes the dynamic array that the constructor allocated.
///
/// \see AllocateCube | pCubeState
///
TCCube::~TCCube()
{
    for (int x = 0; x < sc[0]; x++)
    {
        for (int y = 0; y < sc[1]; y++)
        {
            // Delete the current array of booleans (z-dimension).
            delete[] pCubeState[x][y];
        }
        // Delete the current array of pointers to booleans (y-dimension).
        delete[] pCubeState[x];
    }
    // Finally, delete the array of pointers to pointers to booleans (x-dimension).
    delete[] pCubeState;
}


///
/// \brief Reset Cube State
///
/// Iterates through each voxel in the internal \ref pCubeState array and sets each value
/// to the passed state (defaults to false).  This can be used to "clear" the cube, and
/// is used to initialize it after the \ref AllocateCube method is called.
///
/// \param state The state to set the voxels in the cube to (defaults to false).
///
/// \see AllocateCube | pCubeState
///
void TCCube::ResetCubeState(bool state)
{
    // Looping through each voxel in the cube, we set them all to the passed state.
    for (int x = 0; x < sc[0]; x++)
    {
        for (int y = 0; y < sc[1]; y++)
        {
            for (int z = 0; z < sc[2]; z++)
            {
                pCubeState[x][y][z] = state;
            }
        }
    }
}


///
/// \brief Set Voxel State
///
/// Sets the state of a single voxel in the cube.  The passed coordinates are first
/// checked with the \ref CheckVoxelBounds method.
///
/// \param x     The x-coordinate in three-space.
/// \param y     The y-coordinate in three-space.
/// \param z     The z-coordinate in three-space.
/// \param state The state to set the voxel to.
///
/// \see CheckVoxelBounds
///
void TCCube::SetVoxelState(byte x, byte y, byte z, bool state)
{
    // After checking the cube bounds, we just set the state of that voxel.
    CheckVoxelBounds(x, y, z);
    pCubeState[x][y][z] = state;
}


///
/// \brief Set Voxel State (Array)
///
/// Sets the state of a single voxel in the cube.  The passed coordinates are first
/// checked with the \ref CheckVoxelBounds method.
///
/// \param cVoxel An array containing the x, y, and z coordinate (in voxels).
/// \param state  The state to set the voxel to.
///
/// \see CheckVoxelBounds
///
void TCCube::SetVoxelState(byte cVoxel[3], bool state)
{
    // After checking the cube bounds, we just set the state of that voxel.
    CheckVoxelBounds(cVoxel[0], cVoxel[1], cVoxel[2]);
    pCubeState[cVoxel[0]][cVoxel[1]][cVoxel[2]] = state;
}


///
/// \brief Get Voxel State
///
/// Gets the state of a single voxel in the cube.  The passed coordinates are first
/// checked with the \ref CheckVoxelBounds method.
///
/// \param x The x-coordinate in three-space.
/// \param y The y-coordinate in three-space.
/// \param z The z-coordinate in three-space.
///
/// \returns The state of the voxel at the passed coordinates.
/// \see     CheckVoxelBounds
///
bool TCCube::GetVoxelState(byte x, byte y, byte z)
{
    // After checking the cube bounds, we just return the state of that voxel.
    CheckVoxelBounds(x, y, z);
    return pCubeState[x][y][z];
}


///
/// \brief Get Voxel State (Array)
///
/// Gets the state of a single voxel in the cube.  The passed coordinates are first
/// checked with the \ref CheckVoxelBounds method.
///
/// \param cVoxel An array containing the x, y, and z coordinate (in voxels).
///
/// \returns The state of the voxel at the passed coordinates.
/// \see     CheckVoxelBounds
///
bool TCCube::GetVoxelState(byte cVoxel[3])
{
    // After checking the cube bounds, we just return the state of that voxel.
    CheckVoxelBounds(cVoxel[0], cVoxel[1], cVoxel[2]);
    return pCubeState[cVoxel[0]][cVoxel[1]][cVoxel[2]];
}


///
/// \brief Set Column State
///
/// Sets the state of a single column in the cube.  The passed coordinates are first
/// checked with the \ref CheckVoxelBounds method.
///
/// \param axis  The axis to set the column (TC_X_AXIS, TC_Y_AXIS, or TC_Z_AXIS).
/// \param dim1  The first remaining dimension
///                  (e.g. the first character from 'XYZ' if you remove the axis).
/// \param dim2  The second remaining dimension
///                  (e.g. the second character from 'XYZ' if you remove the axis).
/// \param state The state to set the voxels in the column to.
///
/// \see CheckVoxelBounds
///
void TCCube::SetColumnState(byte axis, byte dim1, byte dim2, bool state)
{
    // What we do here is first check if the passed dim1 and dim2 values are within their
    // proper ranges (depending on the axis of the column).  Then, we loop through the
    // passed axis, and set the state of each voxel in the column.
    switch (axis)
    {
        case TC_X_AXIS:
            CheckVoxelBounds(0, dim1, dim2);
            for (int x = 0; x < sc[0]; x++)
            {
                pCubeState[x][dim1][dim2] = state;
            }
            break;

        case TC_Y_AXIS:
            CheckVoxelBounds(dim1, 0, dim2);
            for (int y = 0; y < sc[1]; y++)
            {
                pCubeState[dim1][y][dim2] = state;
            }
            break;

        case TC_Z_AXIS:
            CheckVoxelBounds(dim1, dim2, 0);
            for (int z = 0; z < sc[2]; z++)
            {
                pCubeState[dim1][dim2][z] = state;
            }
            break;
    }
}


///
/// \brief Get Column State
///
/// Gets the state of a single column in the cube. The passed coordinates are first
/// checked with the \ref CheckVoxelBounds method, and then the state of every voxel in
/// the column is compared with the comparison value.
///
/// \param axis   The axis containing the column (TC_X_AXIS, TC_Y_AXIS, or TC_Z_AXIS).
/// \param dim1   The first remaining dimension
///                   (e.g. the first character from 'XYZ' if you remove the axis).
/// \param dim2   The second remaining dimension
///                   (e.g. the second character from 'XYZ' if you remove the axis).
/// \param cmpVal The value to compare the voxels in the column to.
///
/// \returns True if all voxels in the column were equal to cmpVal (i.e. logical AND),
///          returns false otherwise.
/// \see     CheckVoxelBounds
///
bool TCCube::GetColumnState(byte axis, byte dim1, byte dim2, bool cmpVal)
{
    // What we do here is first check if the passed dim1 and dim2 values are within their
    // proper ranges (depending on the axis of the column).  Then, we loop through the
    // voxels on the axis, ANDingthem with the comparison value.  If any voxel does NOT
    // match the compare value, we return false.  If the loop passes, we return true.
    switch (axis)
    {
        case TC_X_AXIS:
            CheckVoxelBounds(0, dim1, dim2);
            for (int x = 0; x < sc[0]; x++)
            {
                if (cmpVal != pCubeState[x][dim1][dim2]) return false;
            }
            break;

        case TC_Y_AXIS:
            CheckVoxelBounds(dim1, 0, dim2);
            for (int y = 0; y < sc[1]; y++)
            {
                if (cmpVal != pCubeState[dim1][y][dim2]) return false;
            }
            break;

        case TC_Z_AXIS:
            CheckVoxelBounds(dim1, dim2, 0);
            for (int z = 0; z < sc[2]; z++)
            {
                if (cmpVal != pCubeState[dim1][dim2][z]) return false;
            }
            break;
    }
    return true;    // If the control gets to this point, then all voxels were valid.        
}


///
/// \brief Set Plane State
///
/// Sets the state of a single plane in the cube.  The passed offset is first checked
/// with the \ref CheckVoxelBounds method.
///
/// \param plane  The plane to set the state of the voxels in (either TC_XY_PLANE,
///               TC_ZX_PLANE, or TC_YZ_PLANE).
/// \param offset The offset from 0 to the size-1 of the remaining dimension.
/// \param state  The state to set the voxels in the plane to.
///
/// \see CheckVoxelBounds
///
void TCCube::SetPlaneState(byte plane, byte offset, bool state)
{
    // Similar to the SetColumnState method, what we do here is first check if the passed
    // offset value (depending on the passed plane) is within the proper range. Then, we
    // loop through the two dimensions on the plane, and set the state of each voxel.
    switch (plane)
    {
        case TC_XY_PLANE:
            CheckVoxelBounds(0, 0, offset);
            for (int x = 0; x < sc[0]; x++)
            {
                for (int y = 0; y < sc[1]; y++)
                {
                    pCubeState[x][y][offset] = state;
                }
            }
            break;

        case TC_ZX_PLANE:
            CheckVoxelBounds(0, offset, 0);
            for (int x = 0; x < sc[0]; x++)
            {
                for (int z = 0; z < sc[2]; z++)
                {
                    pCubeState[x][offset][z] = state;
                }
            }
            break;

        case TC_YZ_PLANE:
            CheckVoxelBounds(offset, 0, 0);
            for (int y = 0; y < sc[1]; y++)
            {
                for (int z = 0; z < sc[2]; z++)
                {
                    pCubeState[offset][y][z] = state;
                }
            }
            break;
    }
}


///
/// \brief Get Plane State
///
/// Sets the state of a single plane in the cube.  The passed offset is first checked
/// with the \ref CheckVoxelBounds methodm and then the state of every voxel in the plane
/// is compared with the comparison value.
///
/// \param plane  The plane to set the state of the voxels in (either TC_XY_PLANE,
///               TC_ZX_PLANE, or TC_YZ_PLANE).
/// \param offset The offset from 0 to the size-1 of the remaining dimension.
/// \param cmpVal The value to compare the voxels in the plane to.
///
/// \returns True if all voxels in the plane were equal to cmpVal (i.e. logical AND),
///          returns false otherwise.
/// \see     CheckVoxelBounds
///
bool TCCube::GetPlaneState(byte plane, byte offset, bool cmpVal)
{
    // What we do here is first check if the passed offset value (depending on the passed
    // plane) is within the proper range.  Then, we loop through the two dimensions on
    // the plane, ANDing each voxel with the comparison value.  If any voxel does NOT
    // match the compare value, we return false.  If the loops pass, we return true.
    switch (plane)
    {
        case TC_XY_PLANE:
            CheckVoxelBounds(0, 0, offset);
            for (int x = 0; x < sc[0]; x++)
            {
                for (int y = 0; y < sc[1]; y++)
                {
                    if (cmpVal != pCubeState[x][y][offset]) return false;
                }
            }
            break;

        case TC_ZX_PLANE:
            CheckVoxelBounds(0, offset, 0);
            for (int x = 0; x < sc[0]; x++)
            {
                for (int z = 0; z < sc[2]; z++)
                {
                    if (cmpVal != pCubeState[x][offset][z]) return false;
                }
            }
            break;

        case TC_YZ_PLANE:
            CheckVoxelBounds(offset, 0, 0);
            for (int y = 0; y < sc[1]; y++)
            {
                for (int z = 0; z < sc[2]; z++)
                {
                    if (cmpVal != pCubeState[offset][y][z]) return false;
                }
            }
            break;
    }
    return true;    // If the control gets to this point, then all voxels were valid. 
}


///
/// \brief Shift Cube State
///
/// Shifts all of the voxels in the cube by the specified offset in the direction of the
/// specified plane.  The new voxels that are shifted in take the passed state as well.
///
/// \param plane   The plane to shift of the voxels in (either TC_XY_PLANE, TC_ZX_PLANE,
///                or TC_YZ_PLANE).
/// \param offset  The offset (positive or negative) to shift in the plane.
/// \param shiftIn The value that the shifted in voxels take.
///
/// \returns True if all voxels in the plane were equal to cmpVal (i.e. logical AND),
///          returns false otherwise.
/// \see     CheckVoxelBounds
///
void TCCube::Shift(byte plane, char offset, bool shiftIn = false)
{
    switch (plane)
    {
        case TC_XY_PLANE:       // For the XY plane, we shift along the Z-axis.
            if (offset > 0)
            {
                // Start at the top, and shift each layer up by the offset.
                for (int z = sc[2] - 1; (z - offset) >= 0; z--)
                {
                    for (int x = 0; x < sc[0]; x++)
                    {
                        for (int y = 0; y < sc[1]; y++)
                        {
                            pCubeState[x][y][z] = pCubeState[x][y][z-offset];
                        }
                    }
                }
                // Finally, shift in as many new layers as we need.
                for (int z = 0; (z < offset) && (z < sc[2]); z++)
                {
                    SetPlaneState(plane, z, shiftIn);
                }
            }
            else if (offset < 0)
            {
                // Start at the bottom, and shift each layer down by the offset.
                for (int z = 0; (z - offset) < sc[2]; z++)
                {
                    for (int x = 0; x < sc[0]; x++)
                    {
                        for (int y = 0; y < sc[1]; y++)
                        {
                            pCubeState[x][y][z] = pCubeState[x][y][z-offset];
                        }
                    }
                }
                // Finally, shift in as many new layers as we need.
                for (int z = 1; (z <= (0 - offset)) && (z <= sc[2]); z++)
                {
                    SetPlaneState(plane, sc[2] - z, shiftIn);
                }
            }
            break;

        case TC_ZX_PLANE:       // For the XZ plane, we shift along the y-axis.
            if (offset > 0)
            {
                // Start at the top, and shift each layer up by the offset.
                for (int y = sc[0] - 1; (y - offset) >= 0; y--)
                {
                    for (int x = 0; x < sc[0]; x++)
                    {
                        for (int z = 0; z < sc[2]; z++)
                        {
                            pCubeState[x][y][z] = pCubeState[x][y-offset][z];
                        }
                    }
                }
                // Finally, shift in as many new layers as we need.
                for (int y = 0; (y < offset) && (y < sc[1]); y++)
                {
                    SetPlaneState(plane, y, shiftIn);
                }
            }
            else if (offset < 0)
            {
                // Start at the bottom, and shift each layer down by the offset.
                for (int y = 0; (y - offset) < sc[1]; y++)
                {
                    for (int x = 0; x < sc[0]; x++)
                    {
                        for (int z = 0; z < sc[2]; z++)
                        {
                            pCubeState[x][y][z] = pCubeState[x][y-offset][z];
                        }
                    }
                }
                // Finally, shift in as many new layers as we need.
                for (int y = 1; (y <= (0 - offset)) && (y <= sc[1]); y++)
                {
                    SetPlaneState(plane, sc[1] - y, shiftIn);
                }
            }
            break;

        case TC_YZ_PLANE:       // For the YZ plane, we shift along the x-axis.
            if (offset > 0)
            {
                // Start at the top, and shift each layer up by the offset.
                for (int x = sc[0] - 1; (x - offset) >= 0; x--)
                {
                    for (int y = 0; y < sc[1]; y++)
                    {
                        for (int z = 0; z < sc[2]; z++)
                        {
                            pCubeState[x][y][z] = pCubeState[x-offset][y][z];
                        }
                    }
                }
                // Finally, shift in as many new layers as we need.
                for (int x = 0; (x < offset) && (x < sc[0]); x++)
                {
                    SetPlaneState(plane, x, shiftIn);
                }
            }
            else if (offset < 0)
            {
                // Start at the bottom, and shift each layer down by the offset.
                for (int x = 0; (x - offset) < sc[0]; x++)
                {
                    for (int y = 0; y < sc[1]; y++)
                    {
                        for (int z = 0; z < sc[2]; z++)
                        {
                            pCubeState[x][y][z] = pCubeState[x-offset][y][z];
                        }
                    }
                }
                // Finally, shift in as many new layers as we need.
                for (int x = 1; (x <= (0 - offset)) && (x <= sc[0]); x++)
                {
                    SetPlaneState(plane, sc[0] - x, shiftIn);
                }
            }
            break;
    }
}


///
/// \brief AND Operator
///
/// Performs a logical AND of all elements in the current TCCube with the passed one.
///
/// \param ref The reference TCCube object to AND the current object's voxels with.
/// \remarks Note that this can be used to "mask off" certain voxels.
///
void TCCube::OP_AND(const TCCube &ref)
{
    assert( (sc[0] <= ref.sc[0]) && (sc[1] <= ref.sc[1]) && (sc[2] <= ref.sc[2]) );
    for (int x = 0; x < sc[0]; x++)
    {
        for (int y = 0; y < sc[1]; y++)
        {
            for (int z = 0; z < sc[2]; z++)
            {
                pCubeState[x][y][z] &= ref.pCubeState[x][y][z];
            }
        }
    }
}


///
/// \brief OR Operator
///
/// Performs a logical OR of all elements in the current TCCube with the passed one.
///
/// \param ref The reference TCCube object to OR the current object's voxels with.
/// \remarks Note that this can be used to "combine" the state of two cubes.
///
void TCCube::OP_OR(const TCCube &ref)
{
    assert( (sc[0] <= ref.sc[0]) && (sc[1] <= ref.sc[1]) && (sc[2] <= ref.sc[2]) );
    for (int x = 0; x < sc[0]; x++)
    {
        for (int y = 0; y < sc[1]; y++)
        {
            for (int z = 0; z < sc[2]; z++)
            {
                pCubeState[x][y][z] |= ref.pCubeState[x][y][z];
            }
        }
    }
}


///
/// \brief XOR Operator
///
/// Performs a logical XOR of all elements in the current TCCube with the passed one.
///
/// \param ref The reference TCCube object to XOR the current object's voxels with.
/// \remarks Note that this can be useful when performing animations.
///
void TCCube::OP_XOR(const TCCube &ref)
{
    assert( (sc[0] <= ref.sc[0]) && (sc[1] <= ref.sc[1]) && (sc[2] <= ref.sc[2]) );
    for (int x = 0; x < sc[0]; x++)
    {
        for (int y = 0; y < sc[1]; y++)
        {
            for (int z = 0; z < sc[2]; z++)
            {
                pCubeState[x][y][z] ^= ref.pCubeState[x][y][z];
            }
        }
    }
}


///
/// \brief NOT Operator
///
/// Performs a logical NOT to all elements in the current TCCube.
///
/// \remarks Note that this will literally "invert" the state of the cube.
///
void TCCube::OP_NOT()
{
    for (int x = 0; x < sc[0]; x++)
    {
        for (int y = 0; y < sc[1]; y++)
        {
            for (int z = 0; z < sc[2]; z++)
            {
                pCubeState[x][y][z] = !pCubeState[x][y][z];
            }
        }
    }
}


///
/// \brief Allocate Cube
///
/// Called by the TCCube constructor.  This method allocates memory by iterating through
/// each "dimension" in the \ref pCubeState array and allocating a new block of memory
/// big enough to store the voxels for that dimension.
///
/// When the memory is initialized, each size is stored into the private cube size
/// attribute array \ref sc, and the \ref ResetCubeState method is called.
///
/// \param sizeX The size (in voxels) of the x-dimension.
/// \param sizeY The size (in voxels) of the y-dimension.
/// \param sizeZ The size (in voxels) of the z-dimension.
///
/// \see pCubeState | ResetCubeState | sc
///
void TCCube::AllocateCube(byte sizeX, byte sizeY, byte sizeZ)
{
    // Initially, pCubeState is an array of pointers to pointers of booleans.
    // First, we need to initialize the x-dimension (pointers to pointers of booleans).
    pCubeState = new bool**[sizeX];
    for (int x = 0; x < sizeX; x++)
    {
        // Next, we initialize the y-dimension (pointers to booleans).
        pCubeState[x] = new bool*[sizeY];
        for (int y = 0; y < sizeY; y++)
        {
            // Finally, we initialize the z-dimension (booleans).
            pCubeState[x][y] = new bool[sizeZ];
        }
    }
    sc[0] = sizeX; sc[1] = sizeY; sc[2] = sizeZ;   // Finally we store the cube dimensions,
    ResetCubeState();                              // and reset all of the voxel states.
}


///
/// \brief Check Voxel Bounds
///
/// Validates the passed coordinates with the internally held cube size.  If the values
/// passed are not less then the cube size, an assert() is called and the program quits.
/// This method should be called before directly accessing the \ref pCubeState array to
/// avoid any unallocated or memory access errors.
///
/// \param x     The x-coordinate to validate.
/// \param y     The y-coordinate to validate.
/// \param z     The z-coordinate to validate.
///
/// \see sc
///
void TCCube::CheckVoxelBounds(byte x, byte y, byte z)
{
    // Since all dimensions are defined as byte-type (unsigned char), we don't need to
    // check the lower-bound.  So, we check if the passed sizes exceed the size bounds.
    assert( (x < sc[0]) && (y < sc[1]) && (z < sc[2]) );
}
