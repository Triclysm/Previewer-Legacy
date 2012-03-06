
#include "SDL.h"
#include "SDL_net.h"

#include "netdrv.h"

#include "../main.h"
#include "../console.h"

TCDriver_netdrv::TCDriver_netdrv(
    Uint32 cube_ip,  Uint16 cube_port, Uint16 listen_port, Uint8 frame_format,
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

    frameFormat = frame_format;

    // If we get here, everything went okay, so we can set connected
    // to true, and write a message to the console stating that the
    // driver is loaded.
}

TCDriver_netdrv::~TCDriver_netdrv()
{
    SDLNet_FreePacket(udpPkt);
    SDLNet_UDP_Close(sckRecv);
    SDLNet_UDP_Close(sckSend);
}


int TCDriver_netdrv::SendPacket(const std::string &toSend)
{
    // Prepare packet to send.
    udpPkt->address.host = cubeIp.host;
    udpPkt->address.port = cubeIp.port;
    udpPkt->data         = (Uint8*)toSend.c_str();
    udpPkt->len          = toSend.length();
    return SDLNet_UDP_Send(sckSend, -1, udpPkt);
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

    udpPkt->data = (Uint8*)toSend.c_str();
    udpPkt->len  = toSend.length();
    SDLNet_UDP_Send(sckSend, -1, udpPkt);
}