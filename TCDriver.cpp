/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                             TCDriver Object Source Code                             *
 *                                      TRICLYSM                                       *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *  This file contains the implementation of the TCDriver class as defined by the      *
 *  TCDriver.h header file.  This class allows the current cube's voxel data to be     *
 *  retrieved synchronously or asynchronously, to stream it to a physical LED cube.    *
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
/// \file  TCDriver.cpp
/// \brief This file contains the implementation of the TCDriver class as defined by the
///        TCDriver.h header file.
///

#include "SDL.h"
#include "TCDriver.h"
#include <string>


///
/// \brief Driver Constructor (Synchronous)
///
/// Creates a basic TCDriver object, just storing the passed variables. 
///
/// \param cubeSize The size (in voxels) of each dimension.
/// \see AllocateCube | pCubeState
///
TCDriver::TCDriver(std::string name)
{
    driverName = name;
    driverType = TC_DRIVER_TYPE_SYNCHRONOUS;
}

///
/// \brief Driver Constructor (Asynchronous)
///
/// Creates a basic TCDriver object, just storing the passed variables. 
///
/// \param cubeSize The size (in voxels) of each dimension.
/// \see AllocateCube | pCubeState
///
TCDriver::TCDriver(std::string name, Uint32 rate)
{

    driverName = name;
    driverType = TC_DRIVER_TYPE_SYNCHRONOUS;
}

void TCDriver::Poll()
{

}

void TCDriver::SetPollRate(Uint32 rate)
{
    driverRate = rate;
}

Uint32 TCDriver::GetPollRate()
{
    return driverRate;
}

Uint8 TCDriver::GetDriverType()
{
    return driverType;
}
