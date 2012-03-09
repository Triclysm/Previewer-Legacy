/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                           Network Driver Object Header File                         *
 *                                      TRICLYSM                                       *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *  This file contains the definition of the TCDriver_netdrv class as implemented by   *
 *  the netdrv.cpp source file.  This class inherets the base driver class (TCDriver), *
 *  allowing the network driver to be registered using the global SetDriver function.  *
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
/// \file  netdrv.h
/// \brief This file contains the definition of the TCDriver_netdrv class as implemented
///        by the netdrv.cpp source file, as well as definitions of other relevant
///        functions to connect to networked devices.
///

#pragma once
#ifndef TC_DRIVER_NETDRV_
#define TC_DRIVER_NETDRV_

#include "../TCDriver.h"         // Base driver class to override.
#include "../TCCube.h"
#include "SDL.h"
#include "SDL_net.h"
#include <vector>

#define TC_FF_0C_888_BITPACK        0x01
#define TC_FF_1C_888_CD4_BYTEPACK   0x02
#define TC_FF_1C_888_CD8            0x03
#define TC_FF_0C_444_BITPACK        0x04


void netdrv_GetCubeList(Uint32 cube_ip, Uint16 cube_listenport,
    Uint16 listenPort, unsigned int attempts = 3, Uint32 attempt_len_ms = 100);
bool netdrv_ConnectCube(unsigned int cubeNum, Uint32 pollRate);


///
/// \brief Cube Info Data Structure
///
/// Used to hold all relevant information for a given LED cube.
///
struct cubeInfo
{
  public:
    Uint16      cube_listenport;    ///< The port that the cube listens on.
    Uint16      localport;          ///< The port that Triclysm should listen on.
    Uint32      cube_ip;            ///< The cube IP address (in network byte order).
    Uint8       cube_ffmt;          ///< The cube frame format.
    Uint8       cube_color;         ///< The number of colours the cube supports.
    byte        cube_size[3];       ///< Number of cube voxels in each dimension.
    std::string cube_name;          ///< The name of the remote cube.
};

extern std::vector<cubeInfo> cubeList;

class TCDriver_netdrv : public TCDriver
{
  public:
    TCDriver_netdrv(cubeInfo &cube_params, bool &connected, Uint32 rate = 0);
    ~TCDriver_netdrv();
    int  SendCommand(const std::string &toSend);
    bool RecvString(std::string &toRecv);

    void Poll();

  private:

    IPaddress cubeIp;
    UDPsocket sckSend,
              sckRecv;

    UDPpacket *udpPkt;
    Uint8     frameFormat;
    byte      remoteCubeSize[3];
};


#endif
