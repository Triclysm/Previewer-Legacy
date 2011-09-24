/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                              Main Program Source Code                               *
 *                                      TRICLYSM                                       *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *  This file contains the main implementation and flow of logic for Triclysm,         *
 *  including all settings related to the program (these may be held in variables      *
 *  defined in the main.h header file).  This also initializes all SDL references,     *
 *  subsystems, and other basic objects required for the program.                      *
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
/// \file  main.cpp
/// \brief This file contains the implementation and control of flow for the Triclysm
///        Previewer program itself, including any multithreaded operations and
///        implementation of functions defined in (but not limited to) main.h.
///


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                               PREPROCESSOR DIRECTIVES                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include <cstdio>                       // The standard I/O library.
#include "TCAnim.h"                     // TCAnim object definition.
#include "SDL.h"                        // The main SDL include file.
#include "SDL_opengl.h"                 // SDL OpenGL header (includes GL.h and GLU.h).
#include "SDL_thread.h"                 // SDL threading header.

#include "main.h"                       // The complimentary header to this source file.
#include "render.h"                     // Includes all OpenGL-related rendering functions.
#include "console.h"
#include "events.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                  GLOBAL VARIABLES                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

SDL_Thread *animThread;         ///< The mutex lock for the \ref currAnim object.
SDL_mutex  *animMutex;          ///< The animation thread object.
bool        threadInit = false; ///< Flag used to check if the animation thread is running.

Uint32    tickRate,         ///< The current tick rate (ticks/second).
          msPerTick;        ///< The amount of milliseconds/tick (see \ref SetTickRate).
const int scrWidth  = 720,  ///< The initial screen width (in pixels).
          scrHeight = 480;  ///< The initial screen height (in pixels).

          
TCAnim      *currAnim;      ///< Pointer to the current animation.
bool         showFps,       ///< True to render the FPS counter, false to hide it.
             runAnim,       ///< True to update the current animation, false otherwise
             runProgram;    ///< True to continue running the program (handling events, 
                            ///  calling the main render loop, etc...), false to quit.

byte         cubeSize[3];   ///< The current size of the cube.  This variable should not
                            ///  be modified, use the \ref SetCubeSize function instead.

SDL_Surface *screen;        ///< Pointer to the current SDL screen.
int          scrBpp;        ///< The colour depth (bits per pixel) of the screen.
Uint32 scrFlags = SDL_OPENGL | SDL_RESIZABLE | SDL_INIT_TIMER;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                FUNCTION DEFINITIONS                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///
/// \brief Main
///
/// The main program entry point.  This initializes and prepares the flow of control.
/// 
/// \param argc The count of arguments.
/// \param argv The argument vector/array itself.
///
int main(int argc, char *argv[])
{
    if (argc > 0 && (!strcmp(argv[0], "-f") || !strcmp(argv[0], "-fullscreen")))
    {
        scrFlags |= SDL_FULLSCREEN;
    }
    runProgram = true;          // We set runProgram to true so the program runs.
    runAnim    = false;         // We don't run the animation until we set the cube size.
    SetTickRate(30);            // Before initializing anything, we set the tick rate.

    // Now, we attempt to initialize the SDL subsystems.  If we couldn't initialize SDL...
    if (!InitSDL()) return 1;   // We cannot continue, so we have to return.

    InitGL();                   // Next, we initialize our OpenGL viewport,
    InitConsole(300, 15, 200);  // and initialize the scripting console.

    if (!InitAnimThread()) return 1;    // We now initialize the animation thread and mutex.

    SetCubeSize(8, 8, 8);       // Now, we can set the initial cube size (and animation),
    runAnim = true;             // allow the animation (thread) to be run,
    EventLoop();                // and enter the main event loop (defined in events.h).
    CleanupSDL();               // Lastly, we clean up SDL when our event loop returns,
    return 0;                   // and return 0 to indicate a successful program run.
}


///
/// \brief Initialize SDL
///
/// Initializes all SDL subsystems required for the program to run.
///
/// \returns True if all initializations were successful, false otherwise.
/// \remarks If this function returns false, the application should exit immediately.
///
bool InitSDL()
{
    // First, we attempt to initialize the SDL video subsystem.
    if (SDL_Init(SDL_INIT_VIDEO) != 0)  // So, if the initialization failed...
    {
        // Show the appropriate error to the user, shut down SDL, and return false.
        fprintf(stderr, TCP_ERROR_SDL_INIT, SDL_GetError());
        SDL_Quit();
        return false;
    }
    // Next, we try to get the information about the current video settings.
    const SDL_VideoInfo *vinfo = SDL_GetVideoInfo();
    if (vinfo == NULL)              // So, if we couldn't obtain the video information...
    {
        // Show the appropriate error to the user, shut down SDL, and return false.
        fprintf(stderr, TCP_ERROR_SDL_VIDINFO, SDL_GetError());
        SDL_Quit();
        return false;
    }
    // Before we set the video mode, we need to enable double buffering.
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    // We also store the bits per pixel from the screen.
    scrBpp = vinfo->vfmt->BitsPerPixel;
    // Now, we attempt to set the video mode using the appropriate bpp and flags.
    screen = SDL_SetVideoMode(scrWidth, scrHeight, scrBpp, scrFlags);
    if (screen == NULL)             // So, if we couldn't set the video mode...
    {
        // Show the appropriate error to the user, shut down SDL, and return false.
        fprintf(stderr, TCP_ERROR_SDL_GLVIDMODE, SDL_GetError());
        SDL_Quit();
        return false;
    }
    // Last, we set the window title, enable key repeats, and return true.
    SDL_WM_SetCaption(TCP_WINDOW_TITLE, "test");
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
    return true;
}


///
/// \brief Cleanup SDL
///
/// Deletes all SDL-related objects not in use, and calls SDL_Quit.  This function should
/// be called before the program terminates (well, normally at least).
///
void CleanupSDL()
{
    SDL_WaitThread(animThread, NULL);
    SDL_DestroyMutex(animMutex);
    delete currAnim;
    SDL_Quit();
}


///
/// \brief Set Tick Rate
///
/// Sets the current tick rate, or the rate at which the current animation is updated.
/// 
/// \param newRate The new tick rate (ticks per/second).
///
void SetTickRate(Uint32 newRate)
{
    tickRate  = newRate;
    msPerTick = 1000 / tickRate;
}


///
/// \brief Set Animation
///
/// Updates the current animation pointed to by \ref currAnim with the new animation
/// passed to this function.
///
/// \param   newAnim A pointer to the new animation.  This should be initialized with
///          the \ref cubeSize variable to avoid cube size issues.
///
/// \returns True if the thread and mutex were initialized successfully, false otherwise.
/// \see     cubeSize | currAnim
///
void SetAnim(TCAnim *newAnim)
{
    // We need to lock the animMutex before modifying the pointer.
    LockAnimMutex();

    // Next, we delete the current animation, and replace it with the passed newAnim.
    // If newAnim is NULL, we set currAnim to a new, default ("blank") animation.
    delete currAnim;
    if (newAnim != NULL)
        currAnim = newAnim;
    else
        currAnim = new TCAnim(cubeSize);
    // Finally, we unlock the animMutex before returning.
    UnlockAnimMutex();
}


///
/// \brief Set Cube Size (Rectangular)
///
/// Sets the global cube size array (\ref cubeSize), updates the drawing position origin
/// (using the current LED spacing), and sets the current animation to the default one.
///
/// \param sx The new size (in voxels) of the cube in the x-direction.
/// \param sy The new size (in voxels) of the cube in the y-direction.
/// \param sz The new size (in voxels) of the cube in the z-direction.
///
/// \remarks  If invalid values are passed (i.e. a size is equal to 0), this function does
///           not modify the cubeSize array.
/// \see      cubeSize | ledStartPos | ledSpacing | SetAnim
///
void SetCubeSize(byte sx, byte sy, byte sz)
{
    // If any of the passed cube sizes were zero, then don't update the size.
    if (sx == 0 || sy == 0 || sz == 0) return;
    // Now, we can update the cubeSize array with the new cube size.
    cubeSize[0] = sx; cubeSize[1] = sy; cubeSize[2] = sz;
    // Next, we update the starting positions for the render loop.
    ledStartPos[0] = 0 - (cubeSize[0] - 1) * (ledSpacing / 2);
    ledStartPos[1] = 0 - (cubeSize[1] - 1) * (ledSpacing / 2);
    ledStartPos[2] = 0 - (cubeSize[2] - 1) * (ledSpacing / 2);
    // Finally, we clear the current animation.
    SetAnim(NULL);
}


///
/// \brief Get Cube Size
///
/// Returns an array containing the current cube size.
///
/// \returns An array with three elements (the x, y, and z sizes of the cube in voxels).
///
/// \remarks  You should delete[] the pointer returned by this function when you're done
///           with it.
/// \see      cubeSize | SetCubeSize
///
byte *GetCubeSize()
{
    byte *toReturn = new byte[3];
    toReturn[0] = cubeSize[0];
    toReturn[1] = cubeSize[1];
    toReturn[2] = cubeSize[2];
    return toReturn;
}


///
/// \brief Initialize Animation Thread
///
/// Creates both the animation thread (which runs the \ref UpdateAnim function in a
/// seperate thread) and the animation mutex lock.
///
/// \returns True if the thread and mutex were initialized successfully, false otherwise.
/// \remarks If this function returns false, the application should exit immediately.
/// \see     animThread | animMutex | UpdateAnimation
///
bool InitAnimThread()
{
    animMutex = SDL_CreateMutex();  // Then, we attempt to create a mutex.
    if (animMutex == NULL)          // If we couldn't create the mutex...
    {
        // Show the appropriate error to the user, shut down SDL, and return false.
        fprintf(stderr, TCP_ERROR_MUTEX_INIT, SDL_GetError());
        SDL_Quit();
        return false;
    }
    // First, we create the animation thread (with the passed data as NULL).
    animThread = SDL_CreateThread(UpdateAnim, NULL);
    if (animThread == NULL)         // If we couldn't create the thread...
    {
        // Show the appropriate error to the user, shut down SDL, and return false.
        fprintf(stderr, TCP_ERROR_THREAD_INIT, SDL_GetError());
        SDL_Quit();
        return false;
    }
    threadInit = true;
    return true;                    // If we get here, everything is fine, so return true.
}


///
/// \brief Update Animation
///
/// This function is run in a seperate thread, which continuously polls the Tick method of
/// the current animation.
///
/// \returns Unused return value.
/// \see     InitAnimThread | msPerTick | runAnim | runProgram | currAnim
///
int UpdateAnim(void *unused)
{
    while (runProgram)      // So, looping while the program is still running...
    {
        if (runAnim)            // If we are supposed to run the animation...
        {
            LockAnimMutex();        // We lock the animation mutex,
            currAnim->Tick();       // update the animation's state,
            UnlockAnimMutex();      // and unlock the mutex.
        }
        SDL_Delay(msPerTick);   // Finally, we wait until we need to for the next Tick.
    }
    return 0;
}


///
/// \brief Lock Animation Mutex
///
/// Performs a mutex lock on the \ref animMutex object.
///
/// \see animMutex
///
void LockAnimMutex()
{
    if (SDL_mutexP(animMutex) == -1)
    {
        fprintf(stderr, TCP_ERROR_MUTEX_LOCK, SDL_GetError());
        exit(1);
    }
}


///
/// \brief Unlock Animation Mutex
///
/// Unlocks the animation mutex lock.
///
/// \see animMutex
///
void UnlockAnimMutex()
{
    if (SDL_mutexV(animMutex) == -1)
    {
        fprintf(stderr, TCP_ERROR_MUTEX_UNLOCK, SDL_GetError());
        exit(1);
    }
}
