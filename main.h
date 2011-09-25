/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                              Main Program Header File                               *
 *                                      TRICLYSM                                       *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *  This file contains the definitions of variables and functions that control the     *
 *  flow of logic for Triclysm Previewer, including all settings related to the        *
 *  program (these are implemented in the main.cpp source file).                       *
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
/// \file  main.h
/// \brief This file contains the definitions of setting/state variables that relate
///        to the imlementation of the main.cpp file.
///


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                               PREPROCESSOR DIRECTIVES                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once
#ifndef TC_MAIN_
#define TC_MAIN_

#include "TCAnim.h"     // The Triclysm Animation Library.
#include "SDL.h"        // The main SDL include file.

#define TC_NAME                "Triclysm"
#define TC_VERSION             "0.20"
#define TC_WINDOW_TITLE        "Triclysm (Alpha)"  // The window title.

// Various error strings used in the initialization functions.
#define TC_ERROR_SDL_INIT      "Error - SDL initialization failed:\n%s\n"
#define TC_ERROR_SDL_VIDINFO   "Error - could not obtain SDL video information:\n%s\n"
#define TC_ERROR_SDL_GLVIDMODE "Error - could not set the SDL/OpenGL video mode:\n%s\n"
#define TC_ERROR_THREAD_INIT   "Error - could not create animation thread object:\n%s\n"
#define TC_ERROR_MUTEX_INIT    "Error - could not create animation mutex object:\n%s\n"
#define TC_ERROR_MUTEX_LOCK    "Error - could not lock animation mutex:\n%s\n"
#define TC_ERROR_MUTEX_UNLOCK  "Error - could not unlock animation mutex:\n%s\n"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                  GLOBAL VARIABLES                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

extern TCAnim       *currAnim;      // Pointer to the current animation.
extern bool          showFps,       // True to render the FPS counter, false to hide it.
                     showCube,      // True to render the actual cube, false to hide it.
                     runAnim,       // True to update the current animation, false to stop.
                     runProgram,    // Set to false to quit the program.
                     nullAnim;      // Set to true if the current animation is blank.

extern byte          cubeSize[3];   // The current size of the cube.

extern SDL_Surface  *screen;        // Pointer to the current SDL screen.
extern int           scrBpp;        // The colour depth (bits per pixel) of the screen.
extern Uint32        scrFlags;      // the flags to use when setting the video mode.


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                 FUNCTION PROTOTYPES                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int  main(int argc, char *argv[]);              // The main program entry point.
bool InitSDL();                                 // Initializes all SDL subsystems.
void CleanupSDL();                              // Cleans up all SDL objects.
void DisplayInitMessage();                      // Writes initialization info to console.
void SetTickRate(Uint32 newRate);               // Sets the animation tick rate.
void SetCubeSize(byte sx, byte sy, byte sz);    // Updates the current cube size.
byte *GetCubeSize();                            // Returns an array of the cube size.
void SetAnim(TCAnim *newAnim);                  // Sets the current animation.

// Thread specific functions:
int  UpdateAnim(void *unused);  // Updates the current animation at the current rate.
bool InitAnimThread();          // Initializes the animation thread and mutex.
void LockAnimMutex();           // Locks the animation mutex (for use with currAnim).
void UnlockAnimMutex();         // Unlocks the animation mutex.


#endif
