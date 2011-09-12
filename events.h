/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                                 Events Header File                                  *
 *                                      TRICLYSM                                       *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *  This file contains the definitions of event handler loop, which includes all       *
 *  input-related parsing (keyboard and mouse).  The EventLoop function is the main    *
 *  event loop for the program.  The program will quit when the function returns.      *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *  Copyright (C) 2011 Brandon Castellano, Ryan Mantha.  All rights reserved.          *
 *  Triclysm Previewer is provided under the BSD-2-Clause license.  This program uses  *
 *  the SDL (Simple DirectMedia Layer) version 1.2, licensed under the GNU LGPL.       *
 *  See the included LICENSE file or <http://www.triclysm.com/> for more details.      *
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
/// \file  events.h
/// \brief This file contains the definitions of the main event loop and all associated
///        functions for handling user input.
///


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                               PREPROCESSOR DIRECTIVES                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once
#ifndef TC_EVENTS_
#define TC_EVENTS_

#include "SDL.h"
#include "SDL_opengl.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                  GLOBAL VARIABLES                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

extern GLfloat mRotRate,    // The mouse rotation rate (used when left-clicking).
               mMoveRate,   // The mouse move rate (used when right-clicking).
               kRotRate,    // The keyboard rotation rate.
               kMoveRate;   // The keyboard move rate.


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                 FUNCTION PROTOTYPES                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void EventLoop();                                   // The main event loop.
void HandleConsoleKey(SDLKey ksym, SDLMod kmod);    // Handles key presses for the console.
void HandleNormalKey(SDLKey ksym, SDLMod kmod);     // Handles key presses for the program.

#endif
