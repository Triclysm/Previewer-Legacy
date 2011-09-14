/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                                Rendering Source Code                                *
 *                                      TRICLYSM                                       *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *  This file contains the main implementation of the OpenGL rendering functions and   *
 *  variables for Triclysm Previewer, including all initialization of 2D/3D objects    *
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
/// \file  render.cpp
/// \brief This file contains the implementation of all OpenGL-related functions for the
///        Triclysm program itself, including any texturing or object creation. Most of
///        the functions in this file are defined in (but not limited to) render.h.
///

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                               PREPROCESSOR DIRECTIVES                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <string>               // Used when rendering text strings.
#include <sstream>              // Needed for the FPS counter.

#include "TCAnim.h"             // TCAnim object definition.
#include "SDL.h"                // Base SDL library header.
#include "SDL_opengl.h"         // SDL OpenGL header (includes GL.h and GLU.h).

#include "render.h"             // Complimentary header to this source file.
#include "console.h"            // Used to reference console strings and variables.
#include "main.h"               // Holds our cube size and current animation variables.
#include "font.h"               // Defines the actual font texture itself.


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                  GLOBAL VARIABLES                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
         
// Colour Definitions (must regenerate display lists for LEDs, can change others anytime).
GLclampf colClear[4]   = {0.15f, 0.15f, 0.15f, 1.00f},  ///< The clear/background colour.
         colLedOn[4]   = {0.20f, 0.20f, 1.00f, 1.00f},  ///< Colour of an LED that is on.
         colLedOff[4]  = {0.00f, 0.00f, 0.00f, 0.50f};  ///< Colour of an LED that is off.

GLclampf colConInputBg[4] = {0.2f, 0.2f, 0.2f, 0.5f},   ///< Input background colour.
         colConBg[4]      = {0.1f, 0.1f, 0.1f, 0.5f},   ///< Console background colour.
         colStrFps[3]     = {0.90f, 0.05f, 0.05f},      ///< FPS counter text colour.
         colStrConsIn[3]  = {1.00f, 1.00f, 0.60f},      ///< Console input text colour.
         colStrConsOut[3] = {0.90f, 0.90f, 0.90f};      ///< Console output text colour.

// LED-sphere specific parameters (can also be used to control quality/performance).
GLuint   dlistLedOn,        ///< The LED on  display list.
         dlistLedOff;       ///< The LED off display list.
GLfloat  ledSpacing = 0.5f, ///< The space between LEDs.
         sphRadius  = 0.1f; ///< The radius of the LED spheres.
GLint    sphSlices  = 12,   ///< Number of slices used when creating the sphere.
         sphStacks  = 12;   ///< Number of stacks used when creating the sphere.
GLfloat  ledStartPos[3];    ///< Array holding the starting position to draw the LEDs at.

// Camera-specific variables (these are applied to the cube, not the viewport).
GLfloat  viewRotX =  30.0f, ///< The current horizontal rotation, about the y-axis.
         viewRotY =  20.0f, ///< The current vertical   rotation, about the x-axis.
         viewPosZ = -40.0f; ///< The current z-position of the camera.

// Font specific variables
GLuint   fontTex;           ///< Texture reference to the console font.
GLfloat  relCharH,          ///< The relative height of a single character on the screen.
         relCharW;          ///< The relative width  of a single character on the screen.
size_t   totalLines,        ///< The total amount of lines that can fit on the screen.
         charsPerLine;      ///< The number of characters that can fit across the screen.

Uint16   fpsMax,            ///< The maximum framerate to render at (0 to disable).
         fpsRateCap;        ///< The current delay value (0 to disable).
Uint32   fpsCurrTicks;      ///< Holds the current number of ticks for the FPS counter.

const Uint32 cursorFlashRate = 600; ///< Rate at which the console cursor is flashed.


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                FUNCTION DEFINITIONS                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///
/// \brief Initialize OpenGL
///
/// Performs all OpenGL-related initialization, including generating the LED display lists,
/// creating the font texture, setting the clear colour, and calling \ref Resize.
///
/// \see InitDisplayLists | InitFont | Resize | colClear
///
void InitGL()
{
    glEnable(GL_BLEND);             // Next, we enable blending and set the blending mode.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    dlistLedOn  = glGenLists(1);    // Before we initialize the display lists, we set the
    dlistLedOff = glGenLists(1);    // unique display list identifiers for each.
    InitDisplayLists();             // Finally, we initialize the two display lists.
    InitFont();                     // We also prepare the font for drawing.
    // We also set the clear colour of the scene to the background colour.
    glClearColor(colClear[0], colClear[1], colClear[2], colClear[3]);
    Resize(screen->w, screen->h);   // Then, we call Resize to setup the viewport.
    SetFpsLimit(60);                // We also initialize the FPS counter.
}


///
/// \brief Initialize Display Lists
///
/// Initializes (or replaces) the LED display lists with the proper models (including the
/// colour and quality).  The sphere parameters are set by \ref sphRadius, \ref sphSlices,
/// and \ref sphStacks.
///
/// \remarks If any of the sphere parameters are changed, this function must be called in
///          order for those changes to take effect.
/// \see     dlistLedOn | dlistLedOff | colLedOn | colLedOff
///
void InitDisplayLists()
{
    // We need to generate a display list for an "on" LED and an "off" LED.
    glNewList(dlistLedOn, GL_COMPILE);              // First, we create/replace the list.
    glColor4fv(colLedOn);                           // Next, we set the sphere's colour.
    glBegin(GL_LINE_LOOP);                          // Now, we start drawing.
    GLUquadricObj* tmpquad = gluNewQuadric();       // First, we setup a new quadratic.
    gluQuadricDrawStyle(tmpquad, GLU_FILL);         // Then, we set the quad's draw style.
    gluSphere(tmpquad, sphRadius, sphSlices, sphStacks); // We then create a sphere,
    gluDeleteQuadric(tmpquad);                      // delete the quad,
    glEnd();                                        // and end drawing.
    glEndList();                                    // Finally, we exit display list mode.
    // Now, we just repeat the same steps, except using the LED's "off" colour.
    glNewList(dlistLedOff, GL_COMPILE);
    glColor4fv(colLedOff);
    glBegin(GL_LINE_LOOP);
    tmpquad = gluNewQuadric();
    gluQuadricDrawStyle(tmpquad, GLU_FILL);
    gluSphere(tmpquad, sphRadius, sphSlices, sphStacks);
    gluDeleteQuadric(tmpquad);
    glEnd();
    glEndList();
}


///
/// \brief Initialize Font
///
/// Creates an OpenGL texture from the \ref fontBmp array in font.h, and binds the texture
/// to the \ref fontTex variable.  This function also scales the texture matrix to the
/// size of the image, to avoid artifacts when rendering the texture.
///
/// \see fontTex | fontBmp | FONT_IMG_W | FONT_IMG_H
///
void InitFont()
{
    glGenTextures(1, &fontTex);
    glBindTexture(GL_TEXTURE_2D, fontTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FONT_IMG_W, FONT_IMG_H, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, fontBmp);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScaled(1.0l / FONT_IMG_W, 1.0l / FONT_IMG_H, 1.0l);
    glMatrixMode(GL_MODELVIEW);
}


///
/// \brief Resize
///
/// Scales the viewport to the passed width and height values, and resets the projection
/// matrix to reflect the new sizes (while maintaining the scale of the objects).  This
/// function also re-computes the relative font character sizes (\ref relCharH and
/// \ref relCharW), and updates the \ref totalLines variable as well.
///
/// \see relCharH | relCharW | totalLines
///
void Resize(GLint width, GLint height)
{
    // First, we calculate the new relative size of characters on the screen.
    relCharH = (GLfloat) FONT_CHAR_H / height;
    relCharW = (GLfloat) FONT_CHAR_W / width;
    // We also compute the total number of lines and characters that can fit on the screen.
    totalLines   = (size_t)  height / FONT_CHAR_H;
    charsPerLine = (size_t)(1 / relCharW);
    // Now, we make the viewport fill the window, calculate the window's aspect ratio.
    glViewport(0, 0, width, height);
    GLfloat aspectRatio = (GLfloat) height / width;
    // Next, we set the projection matrix to match the new size of the viewport.
    glMatrixMode(GL_PROJECTION);    // First, we switch the matrix mode,
    glLoadIdentity();               // and load the identity matrix.
    // Next, we multiply the projection identity matrix by a view frustum matrix. The
    // width extends from -1 to +1, while the height is variable (set by the aspect ratio).
    glFrustum(-1.0f, 1.0f, -aspectRatio, aspectRatio, 5.0f, 60.0f);
    glMatrixMode(GL_MODELVIEW);     // Finally, we switch back to the modelview matrix.
}


///
/// \brief Change LED Color
///
/// Changes the color of the specified LED, and regenerates the LED display lists to
/// reflect the change.
/// 
/// \param numColors The number of colors in the newColor array (valid values are 3 or 4).
/// \param newColor  An array of integers from 0-255 
/// \param off       Set true to set the color of the off LED (false by default).
///
void ChangeLedColor(int numColors, int newColor[], bool off)
{
    if (numColors == 3 || numColors == 4)
    {
        if (off)
        {
            for (int i = 0; i < numColors; i++)
            {
                colLedOff[i] = newColor[i] / 255.0f;
            }
            if (numColors == 3) colLedOff[3] = 1.0f;

        }
        else
        {
            for (int i = 0; i < numColors; i++)
            {
                colLedOn[i] = newColor[i] / 255.0f;
            }
            if (numColors == 3) colLedOn[3] = 1.0f;
        }
        InitDisplayLists();
    }
}


///
/// \brief Render Scene
///
/// Performs the main flow of control when rendering the OpenGL scene.  This function
/// first calls glClear, performs all applicable drawing/rendering, and then calls
/// the SDL_GL_SwapBuffers function to display the image.
///
/// \remarks The current matrix mode should be GL_MODELVIEW before calling this function.
/// \see     DrawCube | DrawConsoleBg | DrawConsoleText | DrawFpsCounter
///
void RenderScene()
{
    // If there is an FPS limit get the current number of ticks to compute the delay below.
    if (fpsRateCap > 0) fpsCurrTicks = SDL_GetTicks();
    // Next, we clear the OpenGL scene with the specified clear colour.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Now, we can begin to draw everything on the screen in the proper order.
    DrawCube();                     // First, we draw the LED cube itself.
    if (showFps || consoleEnabled)  // Next, do we need to show the FPS counter or console?
    {
        ProjModeBegin();                        // If so, we first enter projection mode.
        if (consoleEnabled) DrawConsoleBg();    // Draw the console background if needed.
        FontModeBegin();                        // Next, enter font mode to render text.
        if (consoleEnabled) DrawConsoleText();  // Draw the console text if needed.
        if (showFps)        DrawFpsCounter();   // Draw the FPS counter text if needed.
        FontModeEnd();                          // Finally, we can exit the font and
        ProjModeEnd();                          // projection modes.
    }
    if (fpsRateCap > 0)             // Then, if there is an FPS limit...
    {
        static Uint32 fpsLastDraw = 0;  // The time the scene was last rendered.
        fpsLastDraw = SDL_GetTicks();
        // Delay the rendering engine by the appropriate amount to limit the framerate.
        SDL_Delay(fpsRateCap - (fpsLastDraw - fpsCurrTicks));
    }
    SDL_GL_SwapBuffers();           // Finally, swap the buffers to display the new image.
}


///
/// \brief Begin Projection Mode
///
/// Loads an identity matrix onto the projection matrix stack, and then loads a new matrix
/// onto the modelview stack.  This matrix is then scaled so that (0,0) is the bottom left
/// corner, and (1,1) is the upper right corner.  Text or 2D graphics can then be drawn
/// onto the viewport.
///
/// \see ProjModeEnd
///
void ProjModeBegin()
{
    glMatrixMode(GL_PROJECTION);        // First, we switch to the projection matrix stack,
    glPushMatrix();                     // load a new matrix,
    glLoadIdentity();                   // and replace it with the identity matrix.

    glMatrixMode(GL_MODELVIEW);         // Next, in the modelview matrix stack,
    glPushMatrix();                     // we also load a new matrix,
    glLoadIdentity();                   // and replace it with the identity matrix.
    glTranslatef(-1.0f, -1.0f, 0.0f);   // We then translate it the lower left corner,
    glScalef    ( 2.0f,  2.0f, 0.0f);   // and stretch the matrix to fill the viewport.
}


///
/// \brief End Projection Mode
///
/// Pops the last matrices on the projection and modelview stacks, which basically reverts
/// the changes made in \ref ProjModeBegin.  This function leaves the current matrix mode
/// to GL_MODELVIEW.
///
/// \see ProjModeBegin
///
void ProjModeEnd()
{
    // We just need to pop the matrices we pushed onto the projection/modelview stacks.
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);         // We should end in the modelview mode.
    glPopMatrix();
}


///
/// \brief Begin Font Mode
///
/// Changes the alpha blending function, enables 2D texturing, and begins GL_QUADS mode so
/// that text can be rendered.
///
/// \see FontModeEnd
///
void FontModeBegin()
{
    glBlendFunc(GL_ONE, GL_ONE);            // First, we change the alpha blend function.
    glEnable(GL_TEXTURE_2D);                // Then, we enable 2D texturing,
    glBindTexture(GL_TEXTURE_2D, fontTex);  // and bind the font texture.
    glBegin(GL_QUADS);                      // Finally, we begin quad vertices mode.
}


///
/// \brief Begin Font Mode
///
/// Reverts the changes made in \ref FontModeBegin by ending the GL_QUADS mode, disabling
/// 2D texturing, and restoring the alpha blending function.
///
/// \see FontModeEnd
///
void FontModeEnd()
{
    glEnd();                                            // We end the quad vertices mode,
    glDisable(GL_TEXTURE_2D);                           // disable 2D texturing mode,
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // and restore the alpha blending.
}


///
/// \brief Draw Cube
///
/// Loops through each voxel in the current animation's cube state, and draws the LEDs
/// on the screen in the proper state.  This function also rotates and translates the
/// cube in accordance with the \ref viewRotX, \ref viewRotY, and \ref viewPosZ variables.
///
/// \see currAnim | ledStartPos | dlistLedOn | dlistLedOff
///
void DrawCube()
{
    glPushMatrix();                         // First, we push a new matrix to the stack,
    glLoadIdentity();                       // and load the identity matrix.
    glTranslatef(0.0f, 0.0f, viewPosZ);     // Then, we move the viewport on the z-axis,
    glRotatef(viewRotY, 1.0f, 0.0f, 0.0f);  // and rotate the view based on the current x
    glRotatef(viewRotX, 0.0f, 1.0f, 0.0f);  // and y rotations.
    // Before drawing the voxels, we need to update the initial drawing position.
    static GLfloat ledCurrPos[3];           // Used to track the current drawing position.
    ledCurrPos[0] = ledStartPos[0];
    ledCurrPos[1] = ledStartPos[1];
    ledCurrPos[2] = ledStartPos[2];
    // Now, we can render each voxel (with the proper state, "on" or "off").
    for (byte x = 0; x < cubeSize[0]; x++)
    {
        for (byte y = 0; y < cubeSize[1]; y++)
        {
            for (byte z = 0; z < cubeSize[2]; z++)
            {
                // First, we copy the current matrix, and translate to the proper position.
                glPushMatrix();
                glTranslatef(ledCurrPos[0], ledCurrPos[2], ledCurrPos[1]);
                // Next, we call the proper display list if the LED is on or off.
                if (currAnim->cubeState->GetVoxelState(x, y, z) == true)
                {
                    glCallList(dlistLedOn);
                }
                else
                {
                    glCallList(dlistLedOff);
                }
                // Finally, we restore our original matrix, and increment the z-coordinate.
                glPopMatrix();
                ledCurrPos[2] += ledSpacing;
            }
            ledCurrPos[1] += ledSpacing;            // Increment the current y-coordinate,
            ledCurrPos[2]  = ledStartPos[2];        // and reset the z-coordinate.
        }
        ledCurrPos[0] += ledSpacing;            // Increment the current x-coordinate,
        ledCurrPos[1]  = ledStartPos[1];        // and reset the y-coordinate.
    }

    glPopMatrix();                          // Lastly, we restore the matrix stack.
}


///
/// \brief Draw Console Background
///
/// Draws the background for the console, overlayed onto the current cube projection.
///
/// \remarks The can only be called when in projection mode.
/// \see     ProjModeBegin
///
void DrawConsoleBg()
{
    glColor4fv(colConBg);                       // First, we set the background colour,
    glBegin(GL_QUADS);                          // enter GL_QUADS mode,
    glVertex3i(0, 0, 0); glVertex3i(1, 0, 0);   // and draw a quad that fills the entire
    glVertex3i(1, 1, 0); glVertex3i(0, 1, 0);   // screen (from 0,0 to 1,1).
    glEnd();
    
    glColor4fv(colConInputBg);                  // Next we set the input background colour,
    glBegin(GL_QUADS);                          // again enter GL_QUADS mode.
    // Now, we just draw the input background as tall as the current input should be.
    GLfloat inputY = relCharH * GetNumLines(currInput.length() + inputPrefix.length() + 1);
    glVertex3i(0,      0, 0); glVertex3i(1,      0, 0);
    glVertex3f(1, inputY, 0); glVertex3d(0, inputY, 0);
    glEnd();
}


///
/// \brief Draw Console Text
///
/// Renders all console text (assuming the console background has been already rendered).
///
/// \remarks The can only be called when in both the projection and font mode.
/// \see     ProjModeBegin | FontModeBegin
///
void DrawConsoleText()
{   
    static Uint32 lastUpdate = SDL_GetTicks();
    static bool   showCursor = true;
    if ((SDL_GetTicks() - lastUpdate) > cursorFlashRate)
    {
        showCursor = !showCursor;
        lastUpdate = SDL_GetTicks();
    }

    glColor3fv(colStrConsIn);
    DrawStringWrapped(&(inputPrefix + currInput), 0.0f);
    if (showCursor) DrawChar('_', relCharW * (cursorPos + 3), 0.0f);

    GLfloat currY = relCharH * GetNumLines(currInput.length() + inputPrefix.length() + 1);
    glColor3fv(colStrConsOut);
    for (outputIt = outputList.begin(); outputIt != outputList.end(); outputIt++)
    {
        // First, we simply draw the string (wrapped) at the current y-position.
        DrawStringWrapped(&(*outputIt), currY);
        // Then, we increment Y with the number of lines it will take to draw.
        currY += relCharH * GetNumLines(outputIt->length());
        // Finally, if we are now off the screen, then we can just return.
        if (currY > 1.0f) break;
    }
}


///
/// \brief Set FPS Limit
/// 
/// Sets the FPS limiter to the passed amount.
/// 
/// \param maxFps The maximum FPS to limit the rendering engine to (set to 0 to disable).
/// \see   fpsMax | fpsRateCap
///
void SetFpsLimit(Uint16 maxFps)
{
    if (maxFps > 0)
    {
        fpsMax     = maxFps;
        fpsRateCap = 1000 / maxFps;
    }
    else
    {
        fpsMax     = 0;
        fpsRateCap = 0;
    }
}


///
/// \brief Draw FPS Counter
///
/// Renders an FPS counter at the top right corner of the viewport.  The FPS is calculated
/// at least every 0.6 seconds, only when this function is being called.  Note that the
/// FPS count will only be accurate following the first update.
///
/// \remarks The can only be called when in both the projection and font mode.
/// \see     colStrFps | ProjModeBegin | FontModeBegin
///
void DrawFpsCounter()
{
    static Uint32 lastUpdate = SDL_GetTicks(),
                  currUpdate = 0,
                  frameCount = 0;
    static std::stringstream fpsStr;
    glColor3fv(colStrFps);
    frameCount++;
    currUpdate = SDL_GetTicks();
    if ((currUpdate - lastUpdate) > 600)
    {
        
        fpsStr.precision(1);
        fpsStr.setf(std::ios::fixed);
        fpsStr.str("");
        fpsStr << (float) 1000 * frameCount / (currUpdate - lastUpdate);
        fpsStr << " FPS";
        frameCount = 0;
        lastUpdate = SDL_GetTicks();
    }
    glColor3fv(colStrFps);
    DrawString(&fpsStr.str(), 1.0f, 1.0f - (relCharH + relCharH), false);
}


///
/// \brief Draw Character
///
/// Draws the specified character at the passed screen coordinates.
///
/// \param c The character to display on the screen.
/// \param x The x-coordinate to begin drawing at.
/// \param y The y-coordinate to begin drawing at.
///
/// \remarks The can only be called when in both the projection and font mode.
/// \see     ProjModeBegin | FontModeBegin
///
void DrawChar(char c, GLfloat x, GLfloat y)
{
    c = c - FONT_FCHAR;                         // First, we compute the position offset.
    if (c < 0) return;                          // If we have an invalid character, return.
    GLint texX = (c % FONT_CPL) * FONT_CHAR_W;  // From the position offset, we find the
    GLint texY = (c / FONT_CPL) * FONT_CHAR_H;  // texture coordinates of the character.
    // Now we just make a quad at the proper position (using texture/relative coordinates).
    glTexCoord2i(texX,               texY + FONT_CHAR_H      );
    glVertex3f  (x,                  y,                  0.0f);
    glTexCoord2i(texX + FONT_CHAR_W, texY + FONT_CHAR_H      );
    glVertex3f  (x + relCharW,       y,                  0.0f);
    glTexCoord2i(texX + FONT_CHAR_W, texY                    );
    glVertex3f  (x + relCharW,       y + relCharH,       0.0f);
    glTexCoord2i(texX,               texY                    );
    glVertex3f  (x,                  y + relCharH,       0.0f);
}


///
/// \brief Draw String
///
/// Draws the specified string at the passed screen coordinates.
///
/// \param toDraw Pointer to the std::string object to draw.
/// \param y      The x-coordinate to begin drawing from.
/// \param y      The y-coordinate to begin drawing from.
/// \param ltr    Optional.  Set to true to draw left-to-right (default), or false to draw
///               the string from right-to-left.
///
/// \remarks The can only be called when in both the projection and font mode.
/// \see     ProjModeBegin | FontModeBegin
///
void DrawString(std::string *toDraw, GLfloat x, GLfloat y, bool ltr)
{
    size_t strLen = toDraw->length();
    if (ltr)
    {
        for (size_t i = 0; i < strLen; i++)
        {
            DrawChar((*toDraw)[i], x + (i * relCharW), y);
        }
    }
    else
    {
        for (size_t i = 0; i < strLen; i++)
        {
            DrawChar((*toDraw)[i], x - ((1 + strLen - i) * relCharW), y);
        }
    }
}


///
/// \brief Draw String Wrapped
///
/// Draws a string that is wrapped between the passed coordinates.  This draws the string
/// across the entire screen (i.e. from x = 0 to x = 1).
///
/// \param toDraw Pointer to the std::string object to draw.
/// \param y      The y-coordinate to begin drawing from.
///
/// \remarks The can only be called when in both the projection and font mode.
/// \see     relCharW | relCharH
///
void DrawStringWrapped(std::string *toDraw, GLfloat y)
{
    size_t  strLen = toDraw->length();
    GLfloat x      = 0.0f;
    y += relCharH * (GetNumLines(strLen) - 1);
    for (size_t i = 0; i < strLen; i++)
    {
        DrawChar((*toDraw)[i], x, y);
        x += relCharW;
        if ((x + relCharW) > 1.0f)
        {
            x  = 0.0f;
            y -= relCharH;
        }
    }
}


///
/// \brief Draw String Wrapped
///
/// Draws a string that is wrapped between the passed coordinates.
///
/// \param toDraw Pointer to the std::string object to draw.
/// \param xMin   The x-coordinate to begin drawing from.
/// \param xMax   The x-coordinate to trigger a line break.
/// \param y      The y-coordinate to begin drawing from.
///
/// \remarks The can only be called when in both the projection and font mode.
/// \see     relCharW | relCharH
///
void DrawStringWrapped(std::string *toDraw, GLfloat xMin, GLfloat xMax, GLfloat y)
{
    size_t  strLen = toDraw->length();
    GLfloat x      = xMin;
    y += (relCharH * GetNumLines(strLen, xMin, xMax));
    for (size_t i = 0; i < strLen; i++)
    {
        DrawChar((*toDraw)[i], x, y);
        x += relCharW;
        if ((x + relCharW) > xMax)
        {
            x  = xMin;
            y -= relCharH;
        }
    }
}


///
/// \brief Get Number of Lines
///
/// Gets the total number of lines it will take to draw the passed string between the
/// entire screen (i.e. x = 0 to x = 1).
///
/// \param toDraw Pointer to the std::string object to compute the number of lines for.
///
/// \returns The number of lines it will take to draw the string.
///
/// \see     relCharW
///
size_t GetNumLines(size_t strLen)
{
    // We can just calculate the number of lines based on the characters per line count.
    size_t numLines = strLen / charsPerLine;
    // Finally, we return the number (and correct it if there was any decimal rounding).
    return (strLen % charsPerLine == 0) ? (numLines) : (numLines + 1);
}

///
/// \brief Get Number of Lines
///
/// Gets the total number of lines it will take to draw the passed string between the
/// two specified x-coordinates.
///
/// \param toDraw Pointer to the std::string object to compute the number of lines for.
/// \param xMin   The x-coordinate to begin computing from.
/// \param xMax   The x-coordinate to trigger a line break.
///
/// \returns The number of lines it will take to draw the string.
///
/// \see     relCharW
///
size_t GetNumLines(size_t strLen, GLfloat xMin, GLfloat xMax)
{
    // First, we get the number of characters we can fit on a single line.
    size_t charsPerLine = (size_t)((xMax - xMin) / relCharW);
    // Then, we calculate the number of lines.
    size_t numLines = strLen / charsPerLine;
    // Finally, we return the number (and correct it if there was any decimal rounding).
    return (strLen % charsPerLine == 0) ? (numLines) : (numLines + 1);
}
