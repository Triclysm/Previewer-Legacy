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

Uint32      tickRate,           ///< The current tick rate (ticks/second).
            msPerTick;          ///< Milliseconds per tick (see \ref SetTickRate).
int         iScrWidth  = 640,   ///< The initial screen width (in pixels).
            iScrHeight = 480;   ///< The initial screen height (in pixels).

TCAnim *currAnim;           ///< Pointer to the current animation.
bool    showFps    = false, ///< True to render the FPS counter, false to hide it.
        showCube   = true,  ///< True to render the actual cube, false to hide it.
        showAxis   = false, ///< True to render the coordinate axes, false to hide them.
        runAnim    = false, ///< True to update the current animation, false otherwise
        runProgram = false, ///< True to continue running the program (handling events, 
                            ///  calling the main render loop, etc...), false to quit.
        nullAnim   = true;  ///< True if currAnim is the default ("null") animation.

byte    cubeSize[3];        ///< The current size of the cube.  This variable should not
                            ///  be modified, use the \ref SetCubeSize function instead.

SDL_Surface *screen;        ///< Pointer to the current SDL screen.
int          scrBpp;        ///< The colour depth (bits per pixel) of the screen.
Uint32 scrFlags =           ///< The initial SDL screen flags.
    SDL_OPENGL | SDL_RESIZABLE | SDL_INIT_TIMER;


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
    SetTickRate(30);            // Also before initializing anything, we set the tick rate,
    SetCubeSize(8, 8, 8);       // and the initial cube size (also sets currAnim).

    InitConsole(300, 15, 200);  // Now, we can first initialize the scripting console.
    DisplayInitMessage();       // After some program info is written to the console,
    LoadScript("config.tcs");   // try to load the config.tcs script (no error is shown if
                                // it can't be found, since it's not explicitly required).

    // Now, we attempt to initialize the SDL subsystems.  If we couldn't initialize SDL...
    if (!InitSDL()) return 1;   // We cannot continue, so we have to return.
    InitGL();                   // Now that we have a screen, we can initialize the OpenGL.

    // If we get here, all subsystems have been initialized, so we can set Triclysm to run.
    runAnim    = true;          // After we set both runAnim and runProgram to true,
    runProgram = true;          // we enter the main event loop (defined in events.h).
    // The last thing we need to do is start the animation update thread (we cannot
    // continue without it). This thread calls currAnim->Update() at the current tickrate.
    if (!InitAnimThread()) return 1;    // We cannot continue without threads, so return.

    EventLoop();        // Everything is ok, so we can finally start the main event loop.
    CleanupSDL();       // Lastly, we clean up SDL when our event loop returns,
    return 0;           // and return 0 to indicate a successful program run.
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
        fprintf(stderr, TC_ERROR_SDL_INIT, SDL_GetError());
        SDL_Quit();
        return false;
    }
    // Next, we try to get the information about the current video settings.
    const SDL_VideoInfo *vinfo = SDL_GetVideoInfo();
    if (vinfo == NULL)              // So, if we couldn't obtain the video information...
    {
        // Show the appropriate error to the user, shut down SDL, and return false.
        fprintf(stderr, TC_ERROR_SDL_VIDINFO, SDL_GetError());
        SDL_Quit();
        return false;
    }
    // Before we set the video mode, we need to enable double buffering.
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    // We also store the bits per pixel from the screen.
    scrBpp = vinfo->vfmt->BitsPerPixel;
    // Now, we attempt to set the video mode using the appropriate bpp and flags.
    screen = SDL_SetVideoMode(iScrWidth, iScrHeight, scrBpp, scrFlags);
    if (screen == NULL)             // So, if we couldn't set the video mode...
    {
        // Show the appropriate error to the user, shut down SDL, and return false.
        fprintf(stderr, TC_ERROR_SDL_GLVIDMODE, SDL_GetError());
        SDL_Quit();
        return false;
    }
    // Last, we set the window title, enable key repeats, and return true.
    SDL_WM_SetCaption(TC_WINDOW_TITLE, "test");
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
/// Display Initialization Message
///
/// Writes some program information to the console, such as the version, and displays
///
///
void DisplayInitMessage()
{
    WriteOutput("" TC_NAME " [Version " TC_VERSION "]\n");
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
/// \brief Get Tick Rate
///
/// Gets the current tick rate.
///
Uint32 GetTickRate()
{
    return tickRate;
}


///
/// \brief Set Animation
///
/// Updates the current animation pointed to by \ref currAnim with the new animation
/// passed to this function, and updates the state of the nullAnim condition.
///
/// \param   newAnim A pointer to the new animation.  This should be initialized with
///          the \ref cubeSize variable to avoid cube size issues.
///
/// \returns True if the thread and mutex were initialized successfully, false otherwise.
/// \see     cubeSize | currAnim | nullAnim
///
void SetAnim(TCAnim *newAnim)
{
    // We need to lock the animMutex before modifying the pointer.
    LockAnimMutex();

    // Next, we delete the current animation, and replace it with the passed newAnim.
    // If newAnim is NULL, we set currAnim to a new, default ("blank") animation.
    delete currAnim;
    currAnim = NULL;

    if (newAnim != NULL)
    {
        currAnim = newAnim;
        nullAnim = false;
    }
    else
    {
        currAnim = new TCAnim(cubeSize);
        nullAnim = true;
    }
    // Finally, we unlock the animMutex before returning.
    UnlockAnimMutex();
}


///
<<<<<<< HEAD
/// \brief Set Driver
///
/// Updates the current driver pointed to by \ref currDriver with the new driver
/// passed to this function, and updates the state of the nullAnim condition.
///
/// \param   newDriver  A pointer to the new cube driver.  This object needs to be
///                     fully initialized before passing it to this function.  To
///                     unload a driver, set newDriver as NULL.
///
/// \see     currDriver
///
void SetDriver(TCDriver *newDriver)
{
    // First, we gracefully stop the current driver.
    LockAnimMutex(); LockDriverMutex();
    runDriver = false;
    UnlockDriverMutex(); UnlockAnimMutex();
    if (driverThread != NULL)
    {
        SDL_WaitThread(driverThread, NULL);
        driverThread = NULL;
    }
    // Now, we can safely delete the driver object, after we re-lock
    // the driver mutex.
    LockDriverMutex();
    delete currDriver;
    currDriver = NULL;

    if (newDriver != NULL)  // So, if we were passed a valid driver...
    {
        currDriver = newDriver;
        // If the driver is synchronous...
        if (currDriver->GetDriverType() == TC_DRIVER_TYPE_SYNCHRONOUS)
        {
            runDriver    = true;
            driverThread = NULL;
        }
        // Else, if the driver is to be run asynchronously...
        else
        {
            // Instead, we create a new thread (based on the UpdateDriver
            // function) and have it continually loop in the thread.
            runDriver    = true;
            driverThread = SDL_CreateThread(UpdateDriver, NULL);
            if (driverThread == NULL)   // If we couldn't create the thread...
            {
                // Print an error, and delete the driver object (since it's invalid now).
                runDriver = false;
                delete currDriver;
                currDriver = NULL;
                WriteOutput("Error - could not create driver thread!");
            }
        }
    }

    UnlockDriverMutex();
}


///
=======
>>>>>>> master
/// \brief Set Cube Size (Rectangular)
///
/// Sets the global cube size array (\ref cubeSize), updates the drawing position origin
/// (using the current LED spacing), and sets the current animation to the default one.
/// This function will also scale the coordinate axes in all directions relative to the
/// size of the new cube.
///
/// \param sx The new size (in voxels) of the cube in the x-direction.
/// \param sy The new size (in voxels) of the cube in the y-direction.
/// \param sz The new size (in voxels) of the cube in the z-direction.
///
/// \remarks  If invalid values are passed (i.e. a size is equal to 0), this function does
///           not modify the cubeSize array.
/// \see      cubeSize | ledStartPos | ledSpacing | axisLength
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
    // We also update the coordinate axes lengths.
    axisLength[0]  = (GLfloat)((ledSpacing * 1.5) * (cubeSize[0] - 1));
    axisLength[1]  = (GLfloat)((ledSpacing * 1.5) * (cubeSize[1] - 1));
    axisLength[2]  = (GLfloat)((ledSpacing * 1.5) * (cubeSize[2] - 1));
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
        fprintf(stderr, TC_ERROR_MUTEX_INIT, SDL_GetError());
        SDL_Quit();
        return false;
    }
    // First, we create the animation thread (with the passed data as NULL).
    animThread = SDL_CreateThread(UpdateAnim, NULL);
    if (animThread == NULL)         // If we couldn't create the thread...
    {
        // Show the appropriate error to the user, shut down SDL, and return false.
        fprintf(stderr, TC_ERROR_THREAD_INIT, SDL_GetError());
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
<<<<<<< HEAD
        // Lastly, we delay by the proper amount before the next Tick.
        updateTime = SDL_GetTicks() - updateTime;
        if (updateTime < msPerTick) SDL_Delay(msPerTick - updateTime);
    }
    return 0;
}


///
/// \brief Update Driver
///
/// This function is run in a seperate thread, which continuously calls the Poll
/// method of the current driver.
///
/// \returns Unused return value.
/// \see     InitAnimThread | msPerTick | runAnim | runProgram | currAnim
///
int UpdateDriver(void *unused)
{
    while (runDriver)      // So, looping while the driver is still running...
    {
        Uint32 delayVal,
               pollTime = SDL_GetTicks();
        // First, we lock the driver mutex.
        LockDriverMutex();
        // Now, we can poll the driver, and get the polling rate.
        currDriver->Poll();
        delayVal = currDriver->GetPollRate();
        // Now, we can unlock the driver mutex, and delay for the appropriate time.
        UnlockDriverMutex();
        pollTime = SDL_GetTicks() - pollTime;
        if (pollTime < delayVal) SDL_Delay(delayVal - pollTime);
=======
        SDL_Delay(msPerTick);   // Finally, we wait until we need to for the next Tick.
>>>>>>> master
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
    if (runProgram && SDL_mutexP(animMutex) == -1)
    {
        fprintf(stderr, TC_ERROR_MUTEX_LOCK, SDL_GetError());
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
    if (runProgram && SDL_mutexV(animMutex) == -1)
    {
        fprintf(stderr, TC_ERROR_MUTEX_UNLOCK, SDL_GetError());
        exit(1);
    }
}
