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
/// \brief Driver Constructor
///
/// Creates a base TCDriver object, and sets the driver type based on the passed rate.
///
/// \param rate The driver's poll rate, in milliseconds.  If this is set to zero (or
///             omitted), the driver is synchronous with the animation tickrate.
///
/// \see driverType | driverRate
///
TCDriver::TCDriver(Uint32 rate)
{
    if (!rate)
    {
        driverType = TC_DRIVER_TYPE_SYNCHRONOUS;
        driverRate = 0;
    }
    else
    {
        driverType = TC_DRIVER_TYPE_ASYNCHRONOUS;
        driverRate = rate;
    }
}


///
/// \brief Destructor
///
/// Not used in the base driver class, but is declared as virtual for drivers
/// which extend the base TCDriver class.
///
TCDriver::~TCDriver()
{

}


///
/// \brief Poll
///
/// Called intermittently at the set polling rate (if asynchronous), or called
/// at every animation tick (if synchronous).
///
void TCDriver::Poll()
{

}


///
/// \brief Send Command
///
/// Used to send a string of characters to a remote device (including to send frames of
/// voxel data, or remote commands).
///
/// \param toSend The string to send to the remote device.
///
/// \remarks The global driver mutex should be locked before calling this method.
///
int  TCDriver::SendCommand(const std::string &toSend)
{
    return 0;
}


///
/// \brief Set Poll Rate
///
/// Used to update the polling rate for asynchronous drivers.  This method has no effect
/// on synchronous drivers, since they are polled at the current tickrate instead.
///
/// \param rate The driver's poll rate, in milliseconds.  This must be greater than zero.
///
/// \remarks The global driver mutex should be locked before calling this method.
///
void TCDriver::SetPollRate(Uint32 rate)
{
    if (rate > 0) driverRate = rate;
}


///
/// \brief Get Poll Rate
///
/// Used to retrieve the driver's current polling rate for asynchronous drivers.
///
/// \returns The current driver polling rate in milliseconds.
///
/// \remarks The global driver mutex should be locked before calling this method.
///
Uint32 TCDriver::GetPollRate()
{
    return driverRate;
}


///
/// \brief Get Driver Type
///
/// Used to retrieve the driver's type as an 8-bit integer.
///
/// \returns An 8-bit value representing the driver type.
///
/// \remarks See the #define directives in TCDriver.h for more details.
///
Uint8 TCDriver::GetDriverType()
{
    return driverType;
}
