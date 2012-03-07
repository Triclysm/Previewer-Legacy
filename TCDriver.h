/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                             TCDriver Object Header File                             *
 *                                      TRICLYSM                                       *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *  This file contains the definition of the TCDriver class as implemented by the      *
 *  TCDriver.cpp source file.  This class allows the current cube's voxel data to be   *
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
/// \file  TCDriver.h
/// \brief This file contains the definition of the TCDriver class as implemented by the
///        TCDriver.cpp source file.
///

#pragma once
#ifndef TC_DRIVER_BASE_
#define TC_DRIVER_BASE_

#include "SDL.h"            // The main SDL include file.
#include "SDL_thread.h"     // SDL threading header.
#include <string>           // Strings library.

#define TC_DRIVER_TYPE_ASYNCHRONOUS 0x00
#define TC_DRIVER_TYPE_SYNCHRONOUS  0x01

class TCDriver
{
  public:
    // Object constructors:
    TCDriver(Uint32 rate = 0);          // Constructor
    virtual ~TCDriver();                // Destructor.

    virtual void Poll();                // Driver poll method.
    virtual int  SendCommand(const std::string &toSend);

    void   SetPollRate(Uint32 rate);    // Need to keep these as discrete
    Uint32 GetPollRate();               // functions because of threading.
    Uint8  GetDriverType();             // Gets the driver type.

  private:
    Uint8  driverType;  // Type of driver (see TC_DRIVER_TYPE_ defines).
    Uint32 driverRate;  // Poll rate for asynchronous drivers.
};


#endif
