/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                           Network Driver Object Source Code                         *
 *                                      TRICLYSM                                       *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *  This file contains the implementation of the TCDriver_netdrv class as defined by   *
 *  the netdrv.h header file.  This class inherets the base driver class (TCDriver),   *
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
/// \file  netdrv.cpp
/// \brief This file contains the implementation of the TCDriver_netdrv class as defined
///        by the netdrv.h header file.
///


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                               PREPROCESSOR DIRECTIVES                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <string>
#include <list>
#include "SDL.h"
#include "SDL_net.h"

#include "netdrv.h"

#include "../main.h"
#include "../console.h"
#include "../render.h"
#include "../format_conversion.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                  GLOBAL VARIABLES                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

std::vector<cubeInfo> cubeList; ///< Holds all devices found by the `netdrv list` command.


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              CLASS METHOD DEFINITIONS                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

TCDriver_netdrv::TCDriver_netdrv(cubeInfo &cube_params, bool &connected, Uint32 rate)
    : TCDriver(rate)
{
    connected = false;

    // Create UDP sockets.
    if (    !(sckSend = SDLNet_UDP_Open(PortToInt(cube_params.cube_listenport)))
         || !(sckRecv = SDLNet_UDP_Open(PortToInt(cube_params.localport))) )
    {
        WriteOutput("netdrv: Error - could not initialize UDP sockets!");
        return;
    }

    if (!(udpPkt = SDLNet_AllocPacket(4096)))
    {
        WriteOutput("netdrv: Error - could not allocate UDP packet!");
        return;
    }
    
    cubeIp.host = cube_params.cube_ip;
    cubeIp.port = cube_params.cube_listenport;

    udpPkt->address.host = cubeIp.host;
    udpPkt->address.port = cubeIp.port;

    // Store size & frame format.
    frameFormat = cube_params.cube_ffmt;
    remoteCubeSize[0] = cube_params.cube_size[0];
    remoteCubeSize[1] = cube_params.cube_size[1];
    remoteCubeSize[2] = cube_params.cube_size[2];


    // Update the current cube size to match the physical cube.
    byte *currSize = GetCubeSize();
    // We only need to change the size if there is a mis-match.
    if ( !(    remoteCubeSize[0] == currSize[0]
            && remoteCubeSize[1] == currSize[1]
            && remoteCubeSize[2] == currSize[2] ))
    {
        SetCubeSize(remoteCubeSize[0], remoteCubeSize[1], remoteCubeSize[2]);
    }
    delete currSize;

    // Finally, set the connected flag to true.
    connected = true;
}

TCDriver_netdrv::~TCDriver_netdrv()
{
    SDLNet_UDP_Close(sckRecv);
    SDLNet_UDP_Close(sckSend);
    //SDLNet_FreePacket(udpPkt); Need to sort out issues with this.
    // Program crashes when closing, is something still using this packet?
}


int TCDriver_netdrv::SendCommand(const std::string &toSend)
{
    // Prepare packet to send.
    udpPkt->address.host = cubeIp.host;
    udpPkt->address.port = cubeIp.port;
    udpPkt->data         = (Uint8*)toSend.c_str();
    udpPkt->len          = toSend.length();
    return SDLNet_UDP_Send(sckSend, -1, udpPkt);
}

bool TCDriver_netdrv::RecvString(std::string &toRecv)
{
    if (SDLNet_UDP_Recv(sckRecv, udpPkt))
    {
        toRecv = std::string((const char*)udpPkt->data, udpPkt->len);
        return true;
    }
    else
    {
        return false;
    }
}


void TCDriver_netdrv::Poll()
{
    std::string toSend = "*TF*";
    byte nc = currAnim->GetNumColors();
    // Finally, stream cube data.
    LockAnimMutex();
    switch (frameFormat)
    {
        //
        // Frame Format:  8x8x8 0-Colour Bit-Pack
        // Each byte represents all 8 voxel values, on/off, for a particular slice.
        //
        case TC_FF_0C_888_BITPACK:
            for (int z = 0; z < 8; z++)
            {
                for (int y = 0; y < 8; y++)
                {
                    Uint8 sliceData = 0x00;
                    for (int x = 0; x < 8; x++)
                    {
                        if (currAnim->GetVoxelColor(x, y, z))
                        {
                            sliceData |= (1 << x);
                        }
                    }
                    toSend += (char)sliceData;
                }
            }
            break;

        //
        // Frame Format:  8x8x8 1-Colour Color Depth 4 Bit-Pack
        // Each byte represents two 4-bit voxel brightness values.
        //
        case TC_FF_1C_888_CD4_BYTEPACK:
            if (nc == 1)
            {
                for (int z = 0; z < 8; z++)
                {
                    for (int y = 0; y < 8; y++)
                    {
                        for (int x = 0; x < 4; x++)
                        {
                            Uint8 toAdd = 0x00,
                                  colVal;

                            colVal = ((Uint8)currAnim->cubeState[0]->GetVoxelState((2*x), y, z)) >> 4;
                            toAdd = colVal & 0x0F;
                            colVal = ((Uint8)currAnim->cubeState[0]->GetVoxelState((2*x)+1, y, z));
                            toAdd |= (colVal & 0xF0);

                            // put 2*x in lower vox., (2*x)+1 in upper.
                            toSend += (char)toAdd;
                        }
                    }
                }
            }
            else
            {
                for (int z = 0; z < 8; z++)
                {
                    for (int y = 0; y < 8; y++)
                    {
                        for (int x = 0; x < 4; x++)
                        {
                            Uint8 toAdd = 0x00;
                            if (currAnim->GetVoxelColor((2*x), y, z))
                                toAdd |= (0x0F);

                            if (currAnim->GetVoxelColor((2*x)+1, y, z))
                                toAdd |= (0xF0);

                            toSend += (char)toAdd;
                        }
                    }
                }
            }
            break;


        //
        // Frame Format:  8x8x8 1-Colour Color Depth 6
        // Each byte represents each voxel's 6-bit brightness value.
        //
        case TC_FF_1C_888_CD6:
            switch (nc)
            {
                case 0:
                    for (int z = 0; z < 8; z++)
                    {
                        for (int y = 0; y < 8; y++)
                        {
                            for (int x = 0; x < 8; x++)
                            {
                                toSend += (char)((currAnim->cubeState[0]->GetVoxelState(x, y, z) ? 0xFF : 0x00) >> 2);
                            }
                        }
                    }
                    break;
                case 1:
                    for (int z = 0; z < 8; z++)
                    {
                        for (int y = 0; y < 8; y++)
                        {
                            for (int x = 0; x < 8; x++)
                            {
                                toSend += (char)(currAnim->cubeState[0]->GetVoxelState(x, y, z) >> 2);
                            }
                        }
                    }
                    break;

                case 3:
                    for (int z = 0; z < 8; z++)
                    {
                        for (int y = 0; y < 8; y++)
                        {
                            for (int x = 0; x < 8; x++)
                            {
                                unsigned int brightness = currAnim->cubeState[0]->GetVoxelState(x, y, z)
                                                        + currAnim->cubeState[1]->GetVoxelState(x, y, z)
                                                        + currAnim->cubeState[2]->GetVoxelState(x, y, z);
                                brightness /= (0xFF*3);
                                toSend += (char)(brightness >> 2);
                            }
                        }
                    }
                    break;
            }
            break;


        //
        // Frame Format:  4x4x4 0-Colour Bit-Pack
        // Each byte represents two slices of on/off data for each voxel (8 voxels/byte).
        //
        case TC_FF_0C_444_BITPACK:
            for (int z = 0; z < 4; z++)
            {
                Uint8 sliceData[2] = { 0x00, 0x00 };
                for (int y = 0; y < 4; y++)
                {
                    for (int x = 0; x < 4; x++)
                    {
                        if (currAnim->GetVoxelColor(x, y, z))
                            sliceData[y/2] |= (1 << (x + ( (y % 2 == 0) ? (0) : (4) )));
                    }
                }
                toSend += (char)sliceData[0];
                toSend += (char)sliceData[1];
            }
            break;


        case TC_FF_3C_444:
            switch (nc)
            {
                case 0:
                    for (int z = 0; z < 8; z++)
                    {
                        for (int y = 0; y < 8; y++)
                        {
                            for (int x = 0; x < 8; x++)
                            {
                                if (currAnim->cubeState[0]->GetVoxelState(x, y, z))
                                {
                                    toSend += (char)(0xFF * colLedOn[0]);
                                    toSend += (char)(0xFF * colLedOn[1]);
                                    toSend += (char)(0xFF * colLedOn[2]);
                                }
                                else
                                {
                                    toSend += (char)0;
                                    toSend += (char)0;
                                    toSend += (char)0;
                                }
                            }
                        }
                    }
                    break;
                case 1:
                    for (int z = 0; z < 8; z++)
                    {
                        for (int y = 0; y < 8; y++)
                        {
                            for (int x = 0; x < 8; x++)
                            {
                                toSend += (char)(currAnim->cubeState[0]->GetVoxelState(x, y, z) * colLedOn[0]);
                                toSend += (char)(currAnim->cubeState[0]->GetVoxelState(x, y, z) * colLedOn[1]);
                                toSend += (char)(currAnim->cubeState[0]->GetVoxelState(x, y, z) * colLedOn[2]);
                            }
                        }
                    }
                    break;
                case 3:
                    for (int z = 0; z < 8; z++)
                    {
                        for (int y = 0; y < 8; y++)
                        {
                            for (int x = 0; x < 8; x++)
                            {
                                toSend += (char)(currAnim->cubeState[0]->GetVoxelState(x, y, z));
                                toSend += (char)(currAnim->cubeState[1]->GetVoxelState(x, y, z));
                                toSend += (char)(currAnim->cubeState[2]->GetVoxelState(x, y, z));\
                            }
                        }
                    }
                    break;
            }
            break;

        default:
            runDriver = false;
            break;
    }
    UnlockAnimMutex();

    toSend += "*TE*";
    SendCommand(toSend);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                FUNCTION DEFINITIONS                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///
/// \brief Parse Cube Parameters
///
/// Extracts all relevant cube parameters from the passed string, and attempts to parse
/// and place the data into the passed cubeInfo data structure.
///
/// \param cubeParams String obtained from the remote device containing the relevant
///                   cube parameters.
/// \param tmpCube    Data structure to place all parsed parameters into.
///
/// \returns True if the passed cubeInfo struct is valid, false if some parameters could
///          not be parsed or were missing (and the struct should be considered invalid).
///
/// \see netdrv_GetCubeList | TCDriver_netdrv
///
bool netdrv_ParseCubeParams(std::string const& cubeParams, cubeInfo &tmpCube)
{
    std::string::size_type  tokenStart,
                            tokenEnd;
    std::string             tmpParam;

    // Attempt to parse all parameters.
    // Parse *PC* (Cube Colours)
    if (std::string::npos == (tokenStart = cubeParams.find("*PC*"))) return false;
    if (std::string::npos == (tokenEnd = cubeParams.find("*PE*", tokenStart))) return false;
    // Check that they're spaced 1 char apart (plus token length).
    if ((tokenEnd - tokenStart) != (5)) return false; // 5 = 1 + 4
    // Finally, get the # of colors (if it's valid).
    tmpCube.cube_color = (Uint8)cubeParams[tokenStart + 4] - '0';
    if (!(tmpCube.cube_color == 0 || tmpCube.cube_color == 1 || tmpCube.cube_color == 3)) return false;

    // Parse *PF* (Cube Frame Format)
    if (std::string::npos == (tokenStart = cubeParams.find("*PF*"))) return false;
    if (std::string::npos == (tokenEnd = cubeParams.find("*PE*", tokenStart))) return false;
    // Check that they're spaced 1 char apart (plus token length).
    if ((tokenEnd - tokenStart) != (5)) return false; // 5 = 1 + 4
    // Finally, get the frame format.
    tmpCube.cube_ffmt = (Uint8)cubeParams[tokenStart + 4];

    // Parse *PN* (Cube Name)
    if (std::string::npos == (tokenStart = cubeParams.find("*PN*"))) return false;
    if (std::string::npos == (tokenEnd = cubeParams.find("*PE*", tokenStart))) return false;
    // Get the cube name.
    tokenStart = tokenStart + 4;    // Add token length.
    tmpCube.cube_name = cubeParams.substr(tokenStart, tokenEnd - tokenStart);

    // Parse *PS* (Cube Size)
    if (std::string::npos == (tokenStart = cubeParams.find("*PS*"))) return false;
    if (std::string::npos == (tokenEnd = cubeParams.find("*PE*", tokenStart))) return false;
    // Check that they're spaced 3 chars apart (plus token length).
    if ((tokenEnd - tokenStart) != (7)) return false; // 7 = 3 + 4
    // Finally, get the frame format.
    tmpCube.cube_size[0] = (byte)cubeParams[tokenStart + 4] - '0';
    tmpCube.cube_size[1] = (byte)cubeParams[tokenStart + 5] - '0';
    tmpCube.cube_size[2] = (byte)cubeParams[tokenStart + 6] - '0';
    // So, if we got here, everything was parsed okay, so we can return true.
    return true;
}


///
/// \brief Get Cube List
///
/// Deletes all cubeInfo structs in the current cubeList, and re-obtains all device 
/// information from the network on the passed address information.
///
/// \param cube_ip         Integer representing the IP address (in network byte order).
/// \param cube_listenport The port that the remote devices are listening on.
/// \param localport       The port Triclysm should listen on for a response.
/// \param attempts        The number of attempts that should be made to get device info.
/// \param attempt_len_ms  The maximum time (in milliseconds) each attempt should wait
///                        to receive a response from a remote device.
///
/// \remarks If no remote devices can be found, ensure that the port/IP information is
///          correct, or try to increase the number of attempts or attempt length.
///
/// \see cubeList | netdrv_ParseCubeParams | netdrv_ConnectCube | TCDriver_netdrv
///
void netdrv_GetCubeList(Uint32 cube_ip, Uint16 cube_listenport,
    Uint16 localport, unsigned int attempts, Uint32 attempt_len_ms)
{
    // Create a UDP socket on the given broadcast IP/port.
    UDPsocket sckRecv, sckSend;
    static UDPpacket *udpPkt = NULL,
                     *pktPrm = NULL;

    if (    !(sckRecv = SDLNet_UDP_Open(PortToInt(localport)))
         || !(sckSend = SDLNet_UDP_Open(PortToInt(cube_listenport))) )
    {
        WriteOutput("netdrv: Fatal error - could not open socket .");
        return;
    }
    if (    ((udpPkt == NULL) && !(udpPkt  = SDLNet_AllocPacket(4096)))
         || ((pktPrm == NULL) && !(pktPrm  = SDLNet_AllocPacket(4096))) )
    {
        WriteOutput("netdrv: Fatal error - could not allocate packet.");
        return;

    }

    // Now attempt to retrieve cube parameters.
    Uint32       startTime;
    std::string  cubeParams;
    bool         gotParams = false;

    cubeList.clear();   // We need to clear the existing list.

    for (unsigned int i = 0; i < attempts; i++)
    {
        cubeParams = "*TP**TE*";
        udpPkt->address.host = cube_ip;
        udpPkt->address.port = cube_listenport;
        udpPkt->data         = (Uint8*)cubeParams.c_str();
        udpPkt->len          = cubeParams.length();
        SDLNet_UDP_Send(sckSend, -1, udpPkt);
        // Attempt to receive packets in 1ms intervals for the specified length.
        startTime = SDL_GetTicks();
        while ((SDL_GetTicks() - startTime) < attempt_len_ms)
        {
            if (SDLNet_UDP_Recv(sckRecv, pktPrm))
            {
                cubeInfo tmpCube;
                // Get string from packet.
                cubeParams = std::string((const char*)pktPrm->data, pktPrm->len);
                // Attempt to parse the string as the cube information.
                if (!netdrv_ParseCubeParams(cubeParams, tmpCube)) break;
                // If we get here, the string was good, so add the remaining packet
                // parameters to the temporary cube.
                tmpCube.cube_listenport = cube_listenport;
                tmpCube.localport       = localport;
                tmpCube.cube_ip         = pktPrm->address.host;
                // Finally, if the cube is not a duplicate, we can add it to the cubeList.
                bool isDuplicate = false;
                for (unsigned int i = 0; i < cubeList.size(); i++)
                {
                    // They are a match if all params agree.
                    if (    cubeList[i].cube_listenport == tmpCube.cube_listenport
                         && cubeList[i].localport       == tmpCube.localport
                         && cubeList[i].cube_ip         == tmpCube.cube_ip
                         && cubeList[i].cube_ffmt       == tmpCube.cube_ffmt
                         && cubeList[i].cube_size[0]    == tmpCube.cube_size[0]
                         && cubeList[i].cube_size[1]    == tmpCube.cube_size[1]
                         && cubeList[i].cube_size[2]    == tmpCube.cube_size[2]
                         && cubeList[i].cube_name       == tmpCube.cube_name       )
                    {
                        isDuplicate = true;
                        break;
                    }
                }
                if (!isDuplicate) cubeList.push_back(tmpCube);
            }
            SDL_Delay(1);
        }
    }
    SDLNet_UDP_Close(sckSend);
    SDLNet_UDP_Close(sckRecv);
}


///
/// \brief Connect To Cube
///
/// Attempts to connect to the passed cube held within the global cubeList object.  If the
/// connection is successful, the respective driver is loaded.
///
/// \param cube_num The cube number to connect to (matching up to the result shown after
///                 running the `netdrv list` command).
/// \param pollRate The poll rate of the driver in milliseconds (set to zero to synchronize
///                 the driver to the global tickrate).
///
/// \returns True if the cube was connected to (and loaded as a driver), false otherwise.
///
/// \remarks Relevant information will be printed to the Triclysm console before this
///          function returns false.
///
/// \see netdrv_GetCubeList | TCDriver_netdrv | SetDriver | cubeList
///
bool netdrv_ConnectCube(unsigned int cubeNum, Uint32 pollRate)
{
    TCDriver *toLoad    = NULL;
    bool     connStatus = false;

    if (cubeList.size() == 0)
    {
        WriteOutput("netdrv: Error - no cubes found. "
                    "Please run `netdrv list` to scan for remote devices.");
        return false;
    }
    else if (cubeNum < 0 || cubeNum > cubeList.size())
    {

        WriteOutput("netdrv: Error - invalid cube index.");
        return false;
    }

    // attempt to load that driver.
    cubeNum--;  // correct for 0-based index.
    toLoad = new TCDriver_netdrv(cubeList[cubeNum], connStatus, pollRate);
    if (connStatus == true)
    {
        SetDriver(toLoad);
        WriteOutput("netdrv: Successfully connected to `" + cubeList[cubeNum].cube_name + "`");
        return true;
    }
    else
    {
        WriteOutput("netdrv: Error - could not connect to cube.");
        return false;
    }
}
