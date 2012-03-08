
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


void netdrv_GetCubeList(Uint32 cube_ip, Uint16 cube_listenport,
    Uint16 listenPort, unsigned int attempts = 3, Uint32 attempt_len_ms = 100);
bool netdrv_ConnectCube(unsigned int cubeNum);

struct cubeInfo
{
  public:
    Uint16      cube_listenport;
    Uint16      localport;
    Uint32      cube_ip;
    Uint8       cube_ffmt;
    Uint8       cube_color;
    byte        cube_size[3];
    std::string cube_name;
};

extern std::vector<cubeInfo> cubeList;

class TCDriver_netdrv : public TCDriver
{
  public:
    // This should ask for an IP, port, asynch/synch.
    // ALSO DO NEGOTIATION HERE - this is what the &connected flag is for!!!!!
    TCDriver_netdrv(
        Uint32 cube_ip,      Uint16 cube_port, Uint16 listen_port,
        bool &connected,
        Uint32 rate = 0);
    ~TCDriver_netdrv();
    int  SendCommand(const std::string &toSend);
    bool RecvString(std::string &toRecv);

    void Poll();

  private:
    bool      GetFrameFormat();
    bool      ParseCubeParams(const std::string &cube_params);

    IPaddress cubeIp;
    UDPsocket sckSend,
              sckRecv;

    UDPpacket *udpPkt;
    Uint8     frameFormat;
    byte      remoteCubeSize[3];
};


#endif