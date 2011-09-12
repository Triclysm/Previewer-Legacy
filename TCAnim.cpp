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
TCAnim::TCAnim(byte cubeSize)
{
    cubeState = new TCCube(cubeSize);
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
TCAnim::TCAnim(byte sizeX, byte sizeY, byte sizeZ)
{
    cubeState = new TCCube(sizeX, sizeY, sizeZ);
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
TCAnim::TCAnim(byte tccSize[3])
{
    cubeState = new TCCube(tccSize);
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
    delete cubeState;
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
