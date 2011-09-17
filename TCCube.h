/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                             TCCube Object  Header File                              *
 *                                      TRICLYSM                                       *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *  This file contains the definition of the TCCube class as implemented by the        *
 *  TCCube.cpp source file.  This class allows a three-dimensional rectangular prism   *
 *  to be represented by discrete voxels, each with a particular state.  The terms     *
 *  cube and rectangular prism may be interchanged depending on the context.           *
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
/// \file  TCCube.h
/// \brief This file contains the definition of the TCCube class as implemented by the
///        TCCube.cpp source file.
///

#pragma once
#ifndef TC_CUBE_
#define TC_CUBE_

// Axis Definitions
#define TC_X_AXIS  0            ///< Specifies the x-axis.
#define TC_Y_AXIS  1            ///< Specifies the y-axis.
#define TC_Z_AXIS  2            ///< Specifies the z-axis.

// Plane Definitions
#define TC_YZ_PLANE 0           ///< Specifies the yz-plane.
#define TC_ZX_PLANE 1           ///< Specifies the xz-plane.
#define TC_XY_PLANE 2           ///< Specifies the xy-plane.

typedef unsigned char  byte;    ///< A single byte, defined as an unsigned char (0 - 255).
typedef signed   char sbyte;    ///< A single byte, defined as a signed char (-127 - 128).

const byte TC_OAXIS[3][2] = { {TC_Y_AXIS, TC_Z_AXIS},
                              {TC_Z_AXIS, TC_X_AXIS},
                              {TC_X_AXIS, TC_Y_AXIS} };


///
/// \brief Triclysm Cube Object
///
/// This class allows a three-dimensional rectangular prism to be represented by
/// discrete voxels, each with a particular state.
///
/// \remarks The terms cube and rectangular prism may be interchanged, depending on
///          the particular context.
///
class TCCube
{
  public:
    // Object constructors:
    TCCube(byte cubeSize);                          // Literal cube constructor.
    TCCube(byte sizeX, byte sizeY, byte sizeZ);     // Arbitrary size constructor.
    TCCube(byte tccSize[3]);                        // Same as above, but with an array.
    TCCube(const TCCube &toCopy);                   // Copy constructor.
    ~TCCube();                                      // TCCube destructor.

    void ResetCubeState(byte state = 0);            // Resets all voxels in the cube.

    // State setting and getting methods:
    void SetVoxelState(byte x, byte y, byte z, byte state);
    void SetVoxelState(byte cVoxel[3], byte state);
    byte GetVoxelState(byte x, byte y, byte z);
    byte GetVoxelState(byte cVoxel[3]);
    void SetColumnState(byte axis, byte dim1, byte dim2, byte state);
    bool GetColumnState(byte axis, byte dim1, byte dim2, byte cmpVal);
    void SetPlaneState(byte plane, byte offset, byte state);
    bool GetPlaneState(byte plane, byte offset, byte cmpVal);
    
    // Shifts contents of the cube in the specified plane by the specified axis.
    void Shift(byte plane, sbyte offset, byte shiftIn = 0x00);

    // Cube operators:
    void OP_AND(const TCCube &ref);
    void OP_OR(const TCCube &ref);
    void OP_XOR(const TCCube &ref);
    void OP_NOT();

  private:
    // Dynamically allocates memory for the object, and sets the sx, sy, and sz variables.
    void AllocateCube(byte x, byte y, byte z);
    // Used whenever a dimension is passed to the object to prevent memory access errors.
    void CheckVoxelBounds(byte x, byte y, byte z);

    /// \brief Three-dimensional array holding the state of each voxel.
    ///
    /// Dynamically allocated when the TCCube object constructor is called.
    byte ***pCubeState;
    /// \brief Array holding the number of cube voxels in each dimension.
    ///
    /// Each dimension is consistent with the axis definitions (e.g. TC_X_AXIS) at the top
    /// of this file (also consistent with the remaining dimension from plane definitions).
    byte sc[3];
};


#endif
