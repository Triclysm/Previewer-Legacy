#include <string>
#include "SDL.h"
#include "SDL_net.h"

#include "netdrv.h"

#include "../main.h"
#include "../console.h"

TCDriver_netdrv::TCDriver_netdrv(
    Uint32 cube_ip,  Uint16 cube_port, Uint16 listen_port,
    bool &connected, Uint32 rate )
    : TCDriver(rate)
{
    connected = false;

    // Create UDP sockets.
    if (    !(sckSend = SDLNet_UDP_Open(cube_port))
         || !(sckRecv = SDLNet_UDP_Open(listen_port)) )
    {
        WriteOutput("netdrv: Error - could not initialize UDP sockets!");
        return;
    }

    if (!(udpPkt = SDLNet_AllocPacket(4096)))
    {
        WriteOutput("netdrv: Error - could not allocate UDP packet!");
        return;
    }
    
    cubeIp.host = cube_ip;
    cubeIp.port = cube_port;

    udpPkt->address.host = cubeIp.host;
    udpPkt->address.port = cubeIp.port;


    // Now attempt to retrieve cube parameters.
    unsigned int attempts   = 3;
    Uint32       attemptLen = 100,
                 startTime;
    std::string  cubeParams;
    bool         gotParams = false;
    for (unsigned int i = 0; i < attempts; i++)
    {
        SendCommand("*TP**TE*");
        startTime = SDL_GetTicks();
        while ((SDL_GetTicks() - startTime) < attemptLen)
        {
            if (RecvString(cubeParams) && ParseCubeParams(cubeParams))
            {
                // If we got the cube parameters, and they are valid,
                // then we can break out of both loops.
                gotParams = true;
                i = attempts;
                break;
            }
        }
    }
    // If we get here, everything went okay, so we can set connected
    // to true, and write a message to the console stating that the
    // driver is loaded.
    if (gotParams)
    {
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

        // Finally, output that we are connected, and set the flag to true.
        WriteOutput("netdrv: Connected to LED cube.");
        connected = true;
    }
    else
    {
        WriteOutput("netdrv: Error - could not connect to LED cube.");
        WriteOutput("netdrv: Could not get parameters from remote IP.");
        connected = false;
    }
}

TCDriver_netdrv::~TCDriver_netdrv()
{
    SDLNet_FreePacket(udpPkt);
    SDLNet_UDP_Close(sckRecv);
    SDLNet_UDP_Close(sckSend);
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

bool TCDriver_netdrv::ParseCubeParams(const std::string &cube_params)
{
    // Parameter listing:
    //  PN = Parameter Name
    //  PD = Parameter Dimensions
    //  PC = Parameter Color
    //  PF = Parameter Frame Format
    //  PE = Parameter End

    // First, ensure that all parameters were received, and record the position
    // of the last parameter.
    return false;
}


void TCDriver_netdrv::Poll()
{
    std::string toSend = "*TF*";
    // Finally, stream cube data.
    LockAnimMutex();
    switch (frameFormat)
    {
        case TC_FF_0C_888_BITPACK:
            for (int z = 0; z < cubeSize[2]; z++)
            {
                for (int y = 0; y < cubeSize[1]; y++)
                {
                    Uint8 sliceData = 0x00;
                    for (int x = 0; x < cubeSize[0]; x++)
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

        default:
            runDriver = false;
            break;
    }
    UnlockAnimMutex();

    toSend += "*TE*";
    SendCommand(toSend);
}