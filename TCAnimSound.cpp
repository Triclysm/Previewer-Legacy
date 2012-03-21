// just update voxels continously

// use the MAX value, put it in a tmp buffer
// Set swap flag when buff. is 
// Double should be fine... No, tripple. :)

#include <cmath>
#include "main.h"
#include "console.h"
#include "SDL.h"
#include "SDL_mixer.h"
#include "TCAnim.h"
#include "TCAnimSound.h"
#include "kiss_fft/kiss_fftr.h"

float* doFFT(int nPoints, short* data)
{
    int oPoints = (nPoints / 2) + 1;
    kiss_fft_scalar *inSamples  = new kiss_fft_scalar[nPoints];
    kiss_fft_cpx    *outSamples = new kiss_fft_cpx   [oPoints];

    kiss_fftr_cfg cfg = kiss_fftr_alloc(nPoints, 0, NULL, NULL);
    // COPY SAMPLES INTO inSample (conv. to floating point)
    for (int i = 0; i < nPoints; i++)
    {
        inSamples[i] = (kiss_fft_scalar)data[i];
    }
    kiss_fftr(cfg, inSamples, outSamples);

    float *toReturn = new float[oPoints];
    for (int i = 0; i < oPoints; i++)
    {
        toReturn[i] = sqrt((outSamples[i].r * outSamples[i].r) + (outSamples[i].i * outSamples[i].i));
    }

    free(cfg);
    delete inSamples;
    delete outSamples;
    return toReturn;    
}


TCAnimSound::TCAnimSound(byte tccSize[3], byte colors, byte plane, byte direction)
    : TCAnim(tccSize, colors)
{
    swapBuffs = 0;
    cPlane     = plane;
    cDirection = (direction >= 0) ? 1 : -1;
    buffer    = new byte**[2];
    numFreq   = tccSize[TC_OAXIS[cPlane][0]];
    numLevels = tccSize[TC_OAXIS[cPlane][1]];
    // allocate buffers
    buffer[0] = new byte*[numFreq];
    buffer[1] = new byte*[numFreq];
    for (byte i = 0; i < numFreq; i++)
    {
        buffer[0][i] = new byte[numLevels];
        buffer[1][i] = new byte[numLevels];
    }
    // initialize pointers
    readBuffer  = buffer[0];
    writeBuffer = buffer[1];

}

TCAnimSound::~TCAnimSound()
{
    sndanim_stop();
    //if (usingBuff) UnlockAnimMutex();
    //while (usingBuff) {}
    //LockAnimMutex();

    for (byte i = 0; i < numFreq; i++)
    {
        delete[] buffer[0][i];
        delete[] buffer[1][i];
    }
    delete[] buffer[0];
    delete[] buffer[1];
    delete[] buffer;
}

void TCAnimSound::Update()
{
    this->Shift(cPlane, cDirection);

    // Anim Mutex is locked here already!
    // Draw what's in buffer 0.
    for (byte i = 0; i < numFreq; i++)
    {
        for (byte j = 0; j < numLevels; j++)
        {
            // THIS IS SLOW, ALTHOUGH VIABLE FOR NOW.
            switch (cPlane)
            {
                case TC_XY_PLANE:
                    this->SetVoxelColor(i, j, (cDirection == 1) ? 0 : sc[3]-1, readBuffer[i][j]);
                    break;
                case TC_YZ_PLANE:
                    this->SetVoxelColor((cDirection == 1) ? 0 : sc[0]-1, i, j, readBuffer[i][j]);
                    break;
                case TC_ZX_PLANE:
                    this->SetVoxelColor(i, (cDirection == 1) ? 0 : sc[2]-1, j, readBuffer[i][j]);
                    break;
            }

        }
    }
    
    // Finally, indicate that we need to swap.
    swapBuffs = 1;
}

void TCAnimSound::DoneIteration()
{

}

void TCAnimSound::Pause()
{

}

void TCAnimSound::ParseSample(int chan, void *stream, int len, void *udata)
{

    static bool  clearBuff = false,
                 usingBuff = false,
                 zeroVals  = false;
    static float maxAmpl   = 0.0f;

    if (usingBuff || len <= 0) return;
    usingBuff = true;

    // Do FFT...
    short        *pStr = (short*)stream;
    unsigned int pLen = len / sizeof(short);
    unsigned int cLen = pLen / 2;

    // * * SPLIT STREAMS * * //
    short *oStrR = new short[cLen];
    short *oStrL = new short[cLen];
    for (unsigned int i = 0; i < pLen; i++)
    {
        oStrL[i >> 1] = pStr[i];
        i++;
        oStrR[i >> 1] = pStr[i];
    }

    float *fftL = doFFT(cLen, oStrL),
          *fftR = doFFT(cLen, oStrR),
          *fftBuff;
    int    fLen = (cLen / 2) + 1;

    // Update buffer w/ values of parsed data.

    //////////////////////

    fftBuff = new float[numFreq];

    // Keep running tally of max. vol
    int binLen  = cLen / numFreq;
    
    for (int i = 0; i < numFreq; i++)
    {
        int   offset = i * binLen;
        //fftBuff[i] = 0.0f;    // Zeroed when the buffer is swapped already.
        for (int j = 0; j < binLen; j++)
        {
            if (abs(fftL[offset+j]) > fftBuff[i]) fftBuff[i] = abs(fftL[offset+j]);
            if (abs(fftR[offset+j]) > fftBuff[i]) fftBuff[i] = abs(fftR[offset+j]);
        }
        if (fftBuff[i] > maxAmpl) maxAmpl = fftBuff[i];
    }

    // Divide all samples by max. amplitude (scales so max. is 1.0f),
    // and update the writeBuffer.
    float height_per_level = 1.0 / numLevels;
    for (int i = 0; i < numFreq; i++)
    {
        fftBuff[i] /= 30480.0f;//maxAmpl;
        
        if (fftBuff[i] > 1.0f) fftBuff[i] = 1.0f;
        if (fftBuff[i] < 0.0f) fftBuff[i] = 0.0f;
        int j = 0;
        while (fftBuff[i] > height_per_level && j < numLevels)
        {
            writeBuffer[i][j++]  = 0xFF;
            fftBuff[i]     -= height_per_level;
        }
        // update last voxel
        if (fftBuff[i] > 0 && j < numLevels)
        {
            writeBuffer[i][j] = (byte)(fftBuff[i] * 0xFF);
        }
    }

    ///////////////////////

    // Delete buffers
    delete[] oStrR;
    delete[] oStrL;
    delete[] fftL;
    delete[] fftR;
    delete[] fftBuff;

    // If a tick was done, then swap buffers.
    
    LockAnimMutex();
    // Swap pointer references.
    if (this->swapBuffs)
    {
        byte **tmp        = this->readBuffer;
        this->readBuffer  = this->writeBuffer;
        this->writeBuffer = tmp;
        clearBuff         = true;
    }
    UnlockAnimMutex();

    // Zero all values in the buffer if we swapped it.
    if (clearBuff)
    {
        for (byte i = 0; i < numFreq; i++)
        {
            for (byte j = 0; j < numLevels; j++)
            {
                writeBuffer[i][j] = 0;
            }
        }
    }
    usingBuff = false;
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                FUNCTION DEFINITIONS                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Mix_Music   *currSound = NULL;
TCAnimSound *currSoundAnim = NULL;
bool        sndanim_mixerinit = false;

void sndanim_setvol(int newvol)
{
    if (sndanim_mixerinit)
    {
        Mix_VolumeMusic(newvol);
    }
    else
    {

    }
}

void sndanim_init()
{
    int flags  = MIX_INIT_MP3;  //MIX_INIT_OGG | MIX_INIT_MOD;
    int status = Mix_Init(flags);
    if ((status & flags) != flags)
    {
        printf("Mix_Init: %s\n", Mix_GetError());
        exit(200);
    }

    //MIX_DEFAULT_FORMAT
    if (Mix_OpenAudio(44100, AUDIO_S16LSB, 2, 1024) == -1)
    {
        printf("Mix_OpenAudio: %s\n", Mix_GetError());
        exit(300);
    }
    Mix_VolumeMusic(16);
    sndanim_mixerinit = true;
}

void sndanim_pause()
{

}


void sndanim_stop()
{
    Mix_HaltMusic();
    if (currSound != NULL) Mix_FreeMusic(currSound);
    currSoundAnim = NULL;
}

void sndanim_getsample(int chan, void *stream, int len, void *udata)
{
    if (currSoundAnim) currSoundAnim->ParseSample(chan, stream, len, udata);
}

///
/// \brief Sound Animation Loader
///
/// Loads the passed filename as a sound animation (.MP3/.WAV).
///
/// \param fname A C-string containing the filename to be loaded.
///
/// \returns A pointer to a TCAnimLua object, casted to a TCAnim object.  If the sound file
///          could not be loaded, NULL is returned.
///
void SoundAnimLoader(char const *fname)
{
    if (!sndanim_mixerinit) sndanim_init();
    if (currSoundAnim != NULL) SetAnim(NULL);
    currSound = Mix_LoadMUS(fname);
    if (!currSound)
    {
        printf("Mix_LoadMUS: %s\n", Mix_GetError());
        sndanim_stop();
        WriteOutput("sndanim:  Error - could not load file.");
        return;
    }
    
    byte *cubeSize = GetCubeSize();
    currSoundAnim = new TCAnimSound(cubeSize, 1, TC_ZX_PLANE, -1);
    delete[] cubeSize;

    if (!Mix_RegisterEffect(MIX_CHANNEL_POST, sndanim_getsample, NULL, NULL))
    {
        printf("Mix_RegisterEffect: %s\n", Mix_GetError());
        WriteOutput("sndanim:  Error - could not register animation with mixer.");
        delete currSoundAnim;
        sndanim_stop();
        return;
    }

    Mix_PlayMusic(currSound, 0);
    Mix_HookMusicFinished(sndanim_stop);

    SetAnim(currSoundAnim);
}

