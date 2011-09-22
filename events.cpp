/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                                Rendering Source Code                                *
 *                                 TRICLYSM PREVIEWER                                  *
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
/// \file  events.cpp
/// \brief This file contains the implementation of the main event loop and all associated
///        functions required for handling and directing user input.
///


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                               PREPROCESSOR DIRECTIVES                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "console.h"
#include "events.h"
#include "main.h"
#include "render.h"
#include "TCAnimLua.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                  GLOBAL VARIABLES                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

GLfloat mRotRate  = 0.5f,   ///< The mouse rotation rate (used when left clicking).
        mMoveRate = 0.1f,   ///< The mouse movement rate (used when right clicking).
        kRotRate  = 0.1f,   ///< The keyboard rotation rate.
        kMoveRate = 0.1f;   ///< The keyboard move rate.
        
int     mouseLastX,         ///< Last updated x-position of the mouse when the left mouse
                            ///  button is held down (-1 when the left button is up).
        mouseLastY,         ///< Last updated y-position of the mouse when the left mouse
                            ///  button is held down (-1 when the left button is up).
        mouseLastZ;         ///< Last updated y-position of the mouse when the right mouse
                            ///  button is held down (-1 when the right button is up).


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                FUNCTION DEFINITIONS                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///
/// \brief Main Event Loop
///
/// The main event loop of the program.  This function returns the control \ref main when
/// it returns, and the program then quits.  This function handles all mouse and keyboard
/// input, and continually polls the \ref RenderScene function.
///
/// \remarks This function stops looping and returns returns when the \ref runProgram
///          variable is set to false.
///
void EventLoop()
{
    while (runProgram)  // So, while we need to run the program...
    {
        SDL_Event event;                // Our general event (should this be outside the while!?!?!)
        while (SDL_PollEvent(&event))   // So, while we still have events to handle...
        {
            switch (event.type)         // First, we determine the event type.
            {
                case SDL_KEYDOWN:           // If the user pressed a key...
                    // If the pressed the '~' or '`' key, then toggle the console.
                    if (event.key.keysym.sym == SDLK_BACKQUOTE)
                    {
                        consoleEnabled = !consoleEnabled;
                    }
                    // Else, if they pressed escape...
                    else if (event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        // We record the time they press the key, and only quit if they press
                        // escape twice within 0.5 seconds.
                        static Uint16 lastPress = 0;
                        Uint16 currPress = SDL_GetTicks();
                        if (currPress - lastPress < 500)
                        {
                            runProgram = false;
                            return;
                        }
                        else
                        {
                            lastPress = currPress;
                        }                        
                    }
                    // Else, if they pressed another key...
                    else
                    {
                        // If the console is enabled, use the console key handler.
                        if (consoleEnabled)
                        {
                            HandleConsoleKey(event.key.keysym.sym, event.key.keysym.mod);
                        }
                        // Else, use the normal key handler.
                        else
                        {
                            HandleNormalKey(event.key.keysym.sym, event.key.keysym.mod);
                        }
                    }
                    break;

                case SDL_MOUSEBUTTONDOWN:   // If the user pressed a mouse button...
                    if (event.button.button == SDL_BUTTON_LEFT)         // Left button:
                    {
                        mouseLastX = event.button.x;    // Record the x and y coordinates
                        mouseLastY = event.button.y;    // when the button was pressed.
                    }
                    else if (event.button.button == SDL_BUTTON_RIGHT)   // Right button:
                    {
                        mouseLastZ = event.button.y;    // Record the mouse's y coordinate.
                    }
                    break;

                case SDL_MOUSEBUTTONUP:     // If the user released a mouse button...
                    if (event.button.button == SDL_BUTTON_LEFT)         // Left button:
                    {
                        mouseLastX = -1;                // Reset the last recorded x and y
                        mouseLastY = -1;                // coordinates of the mouse.
                    }
                    else if (event.button.button == SDL_BUTTON_RIGHT)   // Right button:
                    {
                        mouseLastZ = -1;                // Reset the last y position.
                    }
                    break;

                case SDL_MOUSEMOTION:       // If the user moved the cursor...
                    // If the left mouse button is held down...
                    if (mouseLastX > 0) // && mouseLastY > 0)
                    {
                        // We increase the x and y rotations of the camera by the mouse
                        // rotation rate times how much the mouse moved.
                        viewRotX += (mRotRate)*(event.motion.x - mouseLastX);
                        viewRotY += (mRotRate)*(event.motion.y - mouseLastY);
                        // Next, we record the mouse's new x and y position.
                        mouseLastX = event.motion.x;
                        mouseLastY = event.motion.y;
                        // Lastly, we ensure that the rotation in the y-direction does not
                        // exceed 90 degrees (otherwise the rotations would be backwards).
                        if (viewRotY >   90) viewRotY =   90;
                        if (viewRotY <  -90) viewRotY =  -90;
                        if (viewRotX >  180) viewRotX = -180;
                        if (viewRotX < -180) viewRotX =  180;
                    }
                    // If the right mouse button is held down...
                    if (mouseLastZ > 0)
                    {
                        // We increase the z-position of the camera by the mouse move rate
                        // times how much the mouse moved, and record it's new position.
                        viewPosZ += (mMoveRate)*(event.motion.y - mouseLastZ);
                        mouseLastZ = event.motion.y;
                    }
                    break;

                case SDL_VIDEORESIZE:       // If the user resized the window...
                    // First, we free the old video screen.
                    SDL_FreeSurface(screen);
                    // Next, we set the video mode of the new SDL screen.
                    screen = SDL_SetVideoMode(event.resize.w, event.resize.h,
                                              scrBpp, scrFlags);
                    // Then, we call the Resize function to setup the OpenGL viewport.
                    Resize(screen->w, screen->h);
                    break;

                case SDL_QUIT:              // If the user closed the window...
                    runProgram = false;         // Stop running the program,
                    return;                     // and return from this function.
                    break;

                default:                    // Lastly, if we have any other event...
                    break;                      // Just ignore it.
                
            }
        }
        RenderScene();  // Finally, if all our events were handled, render the scene.
    }
}


///
/// \brief Handle Console Key Press
///
/// Called whenever a key is pressed and the console is enabled.
///
/// \param ksym The SDLKey object representing the key symbol that was pressed.
/// \param kmod The SDLMod object representing any modifiers that were pressed.
///
void HandleConsoleKey(SDLKey ksym, SDLMod kmod)
{
    // First, we handle all "special" (i.e. non-typing) characters.
    switch (ksym)
    {
        case SDLK_BACKSPACE:
            InputBackspace();
            break;
        case SDLK_LEFT:
            MoveCursor(true);
            break;
        case SDLK_RIGHT:
            MoveCursor(false);
            break;
        case SDLK_UP:
            ScrollHistory(true);
            break;
        case SDLK_DOWN:
            ScrollHistory(false);
            break;
        case SDLK_SPACE:
            InputAddChar(' ');
            break;
        case SDLK_RETURN:
            ParseInput();
            break;
        case SDLK_TAB:
            SuggestCommand();
            break;
        case SDLK_PERIOD:
            InputAddChar('.');
            break;
    }
    // Did the user type a letter?
    if (ksym >= SDLK_a && ksym <= SDLK_z)
    {
        if (kmod & KMOD_LSHIFT || kmod & KMOD_RSHIFT)
            InputAddChar('A' + ksym - SDLK_a);
        else
            InputAddChar('a' + ksym - SDLK_a);
    }
    // A number?
    if  (ksym >= SDLK_0 && ksym <= SDLK_9)
    {
        /*if (kmod & KMOD_SHIFT)
            InputAddChar('.' + ksym - SDLK_0);
        else*/
        InputAddChar('0' + ksym - SDLK_0);
    }
    if (ksym == SDLK_QUOTE)
    {
        if (kmod & KMOD_SHIFT)
            InputAddChar('\"');
        else
            InputAddChar('\'');
    }
    if (ksym == SDLK_BACKSLASH)
    {
        if (kmod & KMOD_SHIFT)
            InputAddChar('|');
        else
            InputAddChar('\\');
    }
    if (ksym == SDLK_SEMICOLON)
    {
        if (kmod & KMOD_SHIFT)
            InputAddChar(':');
        else
            InputAddChar(';');
    }
    if (ksym == SDLK_SLASH)
        InputAddChar('/');
    if (ksym == SDLK_MINUS)
    {
        if (kmod & KMOD_SHIFT)
            InputAddChar('_');
        else
            InputAddChar('-');
    }
}


///
/// \brief Handle Normal Key Press
///
/// Called whenever a key is pressed and the console is disabled.
///
/// \param ksym The SDLKey object representing the key symbol that was pressed.
/// \param kmod The SDLMod object representing any modifiers that were pressed.
///
void HandleNormalKey(SDLKey ksym, SDLMod kmod)
{
    switch (ksym)
    {
        case SDLK_UP:
            if (kmod & KMOD_LSHIFT || kmod & KMOD_RSHIFT)
            {
                viewPosZ += kMoveRate;
            }
            else
            {
                viewRotY += kRotRate;
            }
            break;

        case SDLK_DOWN:
            if (kmod & KMOD_LSHIFT || kmod & KMOD_RSHIFT)
            {
                viewPosZ -= kMoveRate;
            }
            else
            {
                viewRotY -= kRotRate;
            }
            break;

        case SDLK_RIGHT:
            viewRotX += kRotRate;
            break;

        case SDLK_LEFT:
            viewRotX -= kRotRate;
            break;

        case SDLK_0:
            viewRotX = 0.0f;
            viewRotY = 0.0f;
            break;

        case SDLK_1:
            viewRotX = 30.0f;
            viewRotY = 20.0f;
            break;

/*          brek
        case SDLK_a:
            //int args[1] = { TC_XY_PLANE };
            //SetAnim(new TCAnimLua(cubeSize, "tcal/sendplane.lua", 0, args));
            SetAnim(LuaAnimLoader("animations/sendplane.lua", 0, NULL));
            break;*/

        case SDLK_a:
            CallCommand("loadanim sendplane.lua");
            break;
    }
}
