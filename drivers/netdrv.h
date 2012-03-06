
#ifndef TC_ANIM_LUA_
#define TC_ANIM_LUA_

#include "../TCDriver.h"         // Base driver class to override.
#include "SDL.h"
#include "SDL_net.h"

#define TC_FF_0C_888_BITPACK        0x01
#define TC_FF_1C_888_CD4_BYTEPACK   0x02
#define TC_FF_1C_888_CD8            0x03


class TCDriver_netdrv : public TCDriver
{
  public:
    // This should ask for an IP, port, asynch/synch.
    // Everything else should be negotiated from there.
    // SDL_Net must be init'd before constructing this.
    TCDriver_netdrv(
        Uint32 cube_ip,      Uint16 cube_port, Uint16 listen_port,
        Uint8  frame_format, bool &connected,
        Uint32 rate = 0);
    ~TCDriver_netdrv();
    void Poll();

  private:
    bool      GetFrameFormat();
    int       SendPacket(const std::string &toSend);

    IPaddress cubeIp;
    UDPsocket sckSend,
              sckRecv;

    UDPpacket *udpPkt;
    Uint8     frameFormat;
};


#endif