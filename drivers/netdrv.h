
#pragma once
#ifndef TC_DRIVER_NETDRV_
#define TC_DRIVER_NETDRV_

#include "../TCDriver.h"         // Base driver class to override.
#include "../TCCube.h"
#include "SDL.h"
#include "SDL_net.h"

#define TC_FF_0C_888_BITPACK        0x01
#define TC_FF_1C_888_CD4_BYTEPACK   0x02
#define TC_FF_1C_888_CD8            0x03


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