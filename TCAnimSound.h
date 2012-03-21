
#pragma once
#ifndef TC_ANIM_SOUND_
#define TC_ANIM_SOUND_


#include "TCAnim.h"


void SoundAnimLoader(char const* fname);
void sndanim_stop();

class TCAnimSound : public TCAnim
{
  public:
    TCAnimSound(byte tccSize[3], byte colors, byte plane, byte direction);
    ~TCAnimSound();
    void DoneIteration();
    
    void ParseSample(int chan, void *stream, int len, void *udata);

    void Pause();
    // etc.
  private:
    byte ***buffer;
    bool swapBuffs,
         usingBuff;
    byte **readBuffer,
         **writeBuffer;

    byte cPlane,
         cDirection;

    byte numFreq,
         numLevels;

    float *binMaxes;

    void Update();
};




#endif
