/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                                Rendering Header File                                *
 *                                      TRICLYSM                                       *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *  This file contains the main definitions of the OpenGL rendering functions and      *
 *  variables for Triclysm Previewer, including all initialization of 2D/3D object     *
 *  and textures (these are implemented in the render.cpp source file).                *
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
/// \file  render.h
/// \brief This file contains the definitions of the OpenGL rendering functions that
///        relate to the imlementation of the render.cpp file.
///


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                               PREPROCESSOR DIRECTIVES                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once
#ifndef TC_RENDER_
#define TC_RENDER_

#include "SDL.h"            // The main SDL include file.
#include "SDL_opengl.h"     // Includes all OpenGL-related files.
#include <string>           // Strings library.

// Font-specific constants:
#define FONT_IMG_H  128     // The height (in pixels) of the font bitmap.
#define FONT_IMG_W  128     // The width  (in pixels) of the font bitmap.
#define FONT_CHAR_H  14     // The height (in pixels) of a single character.
#define FONT_CHAR_W   8     // The width  (in pixels) of a single character.
#define FONT_CPL     16     // The amount of characters per line in the font.
#define FONT_FCHAR  '!'     // The first character to appear in the font.


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                  GLOBAL VARIABLES                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
 
extern size_t   totalLines;         // Total number of lines that can fit in the console.
extern GLfloat  ledStartPos[3];     // Array holding the position of the (0,0,0) voxel.

// Colour Definitions (must regenerate display lists for LEDs, text can change on-the-fly).
extern GLclampf colClear[4],        // Clear colour
                colLedOn[4],        // LED on colour
                colLedOff[4],       // LED off colour
                colStrFps[3],       // FPS counter colour
                colConBg[4],        // Console background colour.
                colConInputBg[4],   // Console input background colour.
                colStrConsIn[3],    // Console input text colour.
                colStrConsOut[3];   // Console output text colour.

// LED-specific parameters (can be used to control quality):
extern GLfloat  ledSpacing,         // LED-specific parameters, which can be used to
                sphRadius;          // control quality.  After changing these parameters,
extern GLint    sphSlices,          // the display lists must be re-created for the
                sphStacks;          // changes to take effect.

// Camera rotation angles:
extern GLfloat  viewRotX,           // The current view rotation in the x-dimension.
                viewRotY,           // The current view rotation in the y-dimension.
                viewPosZ;           // The current location in the z-dimension.

// Framerate limiting variables:
extern Uint16   fpsMax,             // Maximum FPS for the frame limiter.
                fpsRateCap;         // Current delay value (in ms).
extern Uint32   fpsLastDraw,        // Last time the FPS counter was drawn.
                fpsCurrTicks;       // Number of frames rendered since fpsLastDraw.


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                 FUNCTION PROTOTYPES                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void   InitGL();
void   InitDisplayLists();
void   InitFont();
void   Resize(int width, int height);
void   RenderScene();
void   SetFpsLimit(Uint16 maxFps);
void   ChangeLedColor(int numColors, int newColor[], bool off);

void   ProjModeBegin();
void   ProjModeEnd();
void   FontModeBegin();
void   FontModeEnd();

void   DrawCube();
void   DrawChar(char c, GLfloat x, GLfloat y);
void   DrawConsoleBg();
void   DrawConsoleText();
void   DrawFpsCounter();
void   DrawString(std::string *toDraw, GLfloat x, GLfloat y, bool ltr = true);
void   DrawStringWrapped(std::string *toDraw, GLfloat y);
void   DrawStringWrapped(std::string *toDraw, GLfloat xMin, GLfloat xMax, GLfloat y);

size_t GetNumLines(size_t strLen);
size_t GetNumLines(size_t strLen, GLfloat xMin, GLfloat xMax);

#endif
