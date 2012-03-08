#include <string>
#include <list>
#include "SDL.h"
#include "SDL_net.h"

#include "netdrv.h"

#include "../main.h"
#include "../console.h"

TCDriver_netdrv::TCDriver_netdrv(cubeInfo &cube_params, bool &connected, Uint32 rate)
    : TCDriver(rate)
{
    connected = false;

    // Create UDP sockets.
    if (    !(sckSend = SDLNet_UDP_Open(cube_params.cube_listenport))
         || !(sckRecv = SDLNet_UDP_Open(cube_params.localport)) )
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


std::vector<cubeInfo> cubeList;



void netdrv_GetCubeList(Uint32 cube_ip, Uint16 cube_listenport,
    Uint16 localport, unsigned int attempts, Uint32 attempt_len_ms)
{
    // Create a UDP socket on the given broadcast IP/port.
    UDPsocket sckRecv, sckSend;
    static UDPpacket *udpPkt = NULL,
                     *pktPrm = NULL;

    if (    !(sckRecv = SDLNet_UDP_Open(2582))//localport))
         || !(sckSend = SDLNet_UDP_Open(2581))//))
         || ((udpPkt == NULL) && !(udpPkt  = SDLNet_AllocPacket(4096)))
         || ((pktPrm == NULL) && !(pktPrm  = SDLNet_AllocPacket(4096))) )
    {
        WriteOutput("netdrv: Fatal error - could not open socket or allocate packet.");
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

        startTime = SDL_GetTicks();
        while ((SDL_GetTicks() - startTime) < attempt_len_ms)
        {
            if (SDLNet_UDP_Recv(sckRecv, pktPrm))
            {
                int tokenStart,
                                       tokenEnd;
                cubeInfo               tmpCube;
                std::string            tmpParam;
                // Get string from packet.
                cubeParams = std::string((const char*)pktPrm->data, pktPrm->len);
                // Attempt to parse all parameters.
                // Parse *PC* (Cube Colours)
                if (std::string::npos == (tokenStart = cubeParams.find("*PC*"))) break;
                if (std::string::npos == (tokenEnd = cubeParams.find("*PE*", tokenStart))) break;
                // Check that they're spaced 1 char apart (plus token length).
                if ((tokenEnd - tokenStart) != (5)) break; // 5 = 1 + 4
                // Finally, get the # of colors (if it's valid).
                tmpCube.cube_color = (Uint8)cubeParams[tokenStart + 4] - '0';
                if (!(tmpCube.cube_color == 0 || tmpCube.cube_color == 1 || tmpCube.cube_color == 3)) break;

                // Parse *PF* (Cube Frame Format)
                if (std::string::npos == (tokenStart = cubeParams.find("*PF*"))) break;
                if (std::string::npos == (tokenEnd = cubeParams.find("*PE*", tokenStart))) break;
                // Check that they're spaced 1 char apart (plus token length).
                if ((tokenEnd - tokenStart) != (5)) break; // 5 = 1 + 4
                // Finally, get the frame format.
                tmpCube.cube_ffmt = (Uint8)cubeParams[tokenStart + 4];

                // Parse *PN* (Cube Name)
                if (std::string::npos == (tokenStart = cubeParams.find("*PN*"))) break;
                if (std::string::npos == (tokenEnd = cubeParams.find("*PE*", tokenStart))) break;
                // Get the cube name.
                tokenStart = tokenStart + 4;    // Add token length.
                tmpCube.cube_name = cubeParams.substr(tokenStart, tokenEnd - tokenStart);

                // Parse *PS* (Cube Size)
                if (std::string::npos == (tokenStart = cubeParams.find("*PS*"))) break;
                if (std::string::npos == (tokenEnd = cubeParams.find("*PE*", tokenStart))) break;
                // Check that they're spaced 3 chars apart (plus token length).
                if ((tokenEnd - tokenStart) != (7)) break; // 7 = 3 + 4
                // Finally, get the frame format.
                tmpCube.cube_size[0] = (byte)cubeParams[tokenStart + 4] - '0';
                tmpCube.cube_size[1] = (byte)cubeParams[tokenStart + 5] - '0';
                tmpCube.cube_size[2] = (byte)cubeParams[tokenStart + 6] - '0';

                // Finally, if we get here, add the remaining packet parameters to the
                // temporary cube, and we can then add it to the list.
                tmpCube.cube_listenport = cube_listenport;
                tmpCube.localport       = localport;
                tmpCube.cube_ip = udpPkt->address.host;
                bool isDuplicate = false;
                for (unsigned int i = 0; i < cubeList.size(); i++)
                {
                    // They are a match if all params agree.
                    if (   cubeList[i].cube_listenport == tmpCube.cube_listenport
                        && cubeList[i].localport       == tmpCube.localport
                        && cubeList[i].cube_ip         == tmpCube.cube_ip
                        && cubeList[i].cube_ffmt       == tmpCube.cube_ffmt
                        && cubeList[i].cube_size[0]    == tmpCube.cube_size[0]
                        && cubeList[i].cube_size[1]    == tmpCube.cube_size[1]
                        && cubeList[i].cube_size[2]    == tmpCube.cube_size[2]
                        && cubeList[i].cube_name       == tmpCube.cube_name         )
                    {
                        isDuplicate = true;
                        break;
                    }
                }
                if (!isDuplicate) cubeList.push_back(tmpCube);
            }
        }
    }
    SDLNet_UDP_Close(sckSend);
    SDLNet_UDP_Close(sckRecv);
}

bool netdrv_ConnectCube(unsigned int cubeNum)
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
    toLoad = new TCDriver_netdrv(cubeList[cubeNum], connStatus, 0);
    if (connStatus == true)
    {
        SetDriver(toLoad);
        WriteOutput("netdrv: Connected to LED cube.");
        return true;
    }
    else
    {
        WriteOutput("netdrv: Error - could not connect to cube.");
        return false;
    }
}
