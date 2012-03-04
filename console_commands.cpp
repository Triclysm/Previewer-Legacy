/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                            Console Commands Source Code                             *
 *                                      TRICLYSM                                       *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *  This file contains the registration and implementation of the various console      *
 *  commands that can be used when running Triclysm Previewer.                         *
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
/// \file  console_commands.cpp
/// \brief This file contains the registration, implementation, and control of flow for
///        the Triclysm command console.  Any command functions that are added should
///        also be added to the RegisterCommands function at the bottom of this file.
///

#include <string>                   // Required to pass arguments to commands.
#include <vector>                   // Required to pass arguments to commands.
#include <sstream>                  // Useful for creating strings or converting values.
#include <cstring>                  // Required to use memcpy in the screenshot command.

#include "console.h"
#include "events.h"
#include "format_conversion.h"
#include "render.h"
#include "main.h"
#include "TCAnim.h"
#include "TCAnimLua.h"

typedef std::vector<std::string> vectStr;


namespace TC_Console_Error
{
    const std::string INVALID_NUM_ARGS      = "Error - invalid number of arguments passed.",
                      INVALID_NUM_ARGS_LESS = "Error - not enough arguments passed.",
                      INVALID_NUM_ARGS_MORE = "Error - too many arguments passed.",
                      INVALID_ARG_VALUE     = "Error - argument has invalid value.";

    void WrongArgCount(size_t actual, size_t expected)
    {
        if (actual < expected)
        {
            WriteOutput(TC_Console_Error::INVALID_NUM_ARGS_LESS);
        }
        else if (actual > expected)
        {
            WriteOutput(TC_Console_Error::INVALID_NUM_ARGS_MORE);
        }
    }
}


namespace TC_Console_Commands
{

void bind(vectStr const& argv)
{
    switch (argv.size())
    {
        case 1:         // One argument - output what they key is bound to.
        {
            SDLKey keySymbol = (SDLKey)0;
            if (StringToKeySym(argv[0], keySymbol))
            {
                bool found = false;
                // Next, search the list for the key.
                std::list<KeyBind*>::iterator kbIt;
                for (kbIt = kbList.begin(); kbIt != kbList.end(); kbIt++)
                {
                    if ((*kbIt)->ksym == keySymbol)
                    {
                        if (!found)
                        {
                            WriteOutput("The key '" + argv[0] + "' is bound to:");
                            found = true;
                        }
                        std::string strKey;
                        if (KeyBindToString((*kbIt), strKey))
                        {
                            WriteOutput("  " + strKey + " = " + (*kbIt)->cmdStr);
                        }
                    }
                }
            }
            else
            {
                WriteOutput("Error - '" + argv[0] + "' is not a valid key.");
            }
            break;
        }

        case 2:         // Two to five arguments - set the key bind.
        case 3:
        case 4:
        case 5:
        {
            std::string strCmd;
            SDLKey keySymbol = (SDLKey)0;
            bool modShift    = false,
                 modCtrl     = false,
                 modAlt      = false;
            size_t i,
                   numFlags  = argv.size() - 2;
            // First, we parse any flags and modify their boolean equivalents.
            for (i = 0; i < numFlags; i++)
            {
                // Is the current argument a flag/switch?
                if (argv[i][0] == '-')
                {
                    if (argv[i] == "-s" || argv[i] == "-shift")
                    {
                        modShift = true;
                    }
                    else if (argv[i] == "-c" || argv[i] == "-ctrl")
                    {
                        modCtrl = true;
                    }
                    else if (argv[i] == "-a" || argv[i] == "-alt")
                    {
                        modAlt = true;
                    }
                    else
                    {
                        WriteOutput(TC_Console_Error::INVALID_ARG_VALUE);
                        return;
                    }
                }
            }
            // Next, we need to determine which keysym was pressed.
            // argv[numFlags] -> string representing key.
            // argv[numFlags + 1] ->
            if (StringToKeySym(argv[numFlags], keySymbol))
            {
                if (AddKeyBind(keySymbol, modShift, modCtrl, modAlt, argv[numFlags + 1]))
                {
                    WriteOutput("Overwrote existing key bind.");
                }
                // Now re-call this function with a list containing only the key so it's
                // current binds are re-printed.
                vectStr newArgs;
                newArgs.push_back(argv[numFlags]);
                bind(newArgs);
            }
            else
            {
                WriteOutput("Error - '" + argv[numFlags] + "' is not a recognized key.");
            }
            break;
        }

        default:
            TC_Console_Error::WrongArgCount(argv.size(), 5);
            break;
    }
}

void clear(vectStr const& argv)
{
    if (argv.size() == 0)
    {
        ClearOutput();
    }
    else if (argv.size() == 1)
    {
        if (argv[0] == "-o" || argv[0] == "-output")
        {
            ClearOutput();
        }
        else if (argv[0] == "-h" || argv[0] == "-history")
        {
            ClearHistory();
        }
        else
        {
            WriteOutput(TC_Console_Error::INVALID_ARG_VALUE);
        }
    }
    else
    {
        WriteOutput(TC_Console_Error::INVALID_NUM_ARGS);
    }
}

void color(vectStr const& argv)
{
    int argOffset = 0;
    bool offLed   = false;
    // So, if we have 4 or 5 arguments, and the first argument is a switch...
    if ((argv.size() == 4 || argv.size() == 5)      &&
        (argv[0].length() > 0 && argv[0][0] == '-') )
    {
        argOffset = 1;
        if (argv[0] == "-on")
        {
            offLed = false;
        }
        else if (argv[0] == "-off")
        {
            offLed = true;
        }
        else
        {
            WriteOutput(TC_Console_Error::INVALID_ARG_VALUE);
        }
    }
    // So, if we have the proper number of colours...
    int numColors = argv.size() - argOffset;
    if (numColors == 3 || numColors == 4)
    {
        bool validColor = true;
        int *newColor = new int[numColors];
        for (int i = 0; i < numColors; i++)
        {
            
            if (!StringToInt(argv[i + argOffset], newColor[i]) ||
                newColor[i] < 0 || newColor[i] > 255)
            {
                validColor = false;
                break;
            }
        }
        if (validColor)
        {
            if (offLed)
            {
                for (int i = 0; i < numColors; i++)
                {
                    colLedOff[i] = newColor[i] / 255.0f;
                }
            }
            else
            {
                for (int i = 0; i < numColors; i++)
                {
                    colLedOn[i] = newColor[i] / 255.0f;
                }
            }
        }
        else
        {
            WriteOutput(TC_Console_Error::INVALID_ARG_VALUE);
        }
    }
    else
    {
        WriteOutput(TC_Console_Error::INVALID_NUM_ARGS);
    }
}

void cubesize(vectStr const& argv)
{
    switch (argv.size())
    {
        case 0:
        {
            byte *currCubeSize = GetCubeSize();
            std::stringstream ssOutput;
            ssOutput << "The current cube size is: ";
            ssOutput << (int)currCubeSize[0] << "x";
            ssOutput << (int)currCubeSize[1] << "x";
            ssOutput << (int)currCubeSize[2] << " voxels.";
            WriteOutput(ssOutput.str());
            delete[] currCubeSize;
            break;
        }
        case 1:
        {
            std::stringstream strSize(argv[0]);
            Uint16 newSize;
            if (!(strSize >> newSize) || newSize == 0 || newSize > 100)
            {
                WriteOutput(TC_Console_Error::INVALID_ARG_VALUE);
            }
            else
            {
                SetCubeSize((byte)newSize, (byte)newSize, (byte)newSize);
            }
            break;
        }
        case 3:
        {
            std::stringstream strSizeX(argv[0]),
                              strSizeY(argv[1]),
                              strSizeZ(argv[2]);
            Uint16 newSize[3];
            if (    !(strSizeX >> newSize[0]) || newSize[0] == 0 || newSize[0] > 100
                 || !(strSizeY >> newSize[1]) || newSize[1] == 0 || newSize[1] > 100
                 || !(strSizeZ >> newSize[2]) || newSize[2] == 0 || newSize[2] > 100 )
            {
                WriteOutput(TC_Console_Error::INVALID_ARG_VALUE);
            }
            else
            {
                SetCubeSize((byte)newSize[0], (byte)newSize[1], (byte)newSize[2]);
            }
            break;
        }
        default:
            WriteOutput(TC_Console_Error::INVALID_NUM_ARGS);
            break;
    }
}

void echo(vectStr const& argv)
{
    // So, as long as we have some arguments...
    if (argv.size() > 0)
    {
        // If the first argument is a switch...
        if (argv[0].length() > 0 && argv[0][0] == '-')
        {
            // If we are in verbose mode, print each argument on a different line.
            if (argv[0] == "-v" || argv[0] == "-verbose")
            {
                size_t i = 0;
                // If we need to omit the first two arguments...
                if (argv.size() > 1 && argv[1].length() > 0 &&
                   (argv[1] == "-o" || argv[1] == "-omit")  )
                {
                    i = 2;
                }
                for (; i < argv.size(); i++)
                {
                    std::string output;
                    output = "Argument ";
                    output += ('0' + i);
                    WriteOutput(output + " = " + argv[i]);
                }
            }
        }
        else
        {
            std::string output;
            for (size_t i = 0; i < argv.size(); i++)
            {
                if (i > 0) output += ' ';
                output += argv[i];
            }
            WriteOutput(output);
        }
    }
    else
    {
        WriteOutput("");
    }
}

void fpsmax(vectStr const& argv)
{
    switch (argv.size())
    {
        case 0:
        {
            std::string outputStr = "Current FPS limit: ";
            if (fpsMax == 0)
            {
                outputStr += "unlimited.";
            }
            else
            {
                std::stringstream ssFpsMax;
                ssFpsMax << fpsMax;
                outputStr += ssFpsMax.str();
            }
            WriteOutput(outputStr);
            break;
        }
        case 1:
        {
            std::stringstream fpsStr(argv[0]);
            Uint16 newFpsLimit;
            if (!(fpsStr >> newFpsLimit) || newFpsLimit < 0)
            {
                WriteOutput(TC_Console_Error::INVALID_ARG_VALUE);
            }
            else
            {
                SetFpsLimit(newFpsLimit);
            }
            break;
        }
        default:
            WriteOutput(TC_Console_Error::INVALID_NUM_ARGS_MORE);
            break;
    }
}

void help(vectStr const& argv)
{
    if (argv.size() == 0)       // If the user wants to show the quick help...
    {
        WriteOutput("Welcome to " TC_NAME " version " TC_VERSION "!");
        WriteOutput("To get help for a command, type \"help [command]\".");
        WriteOutput("To see a list of commands, type \"list -c\" (or type \"help list\" for more information about the list command).");
    }
    else if (argv.size() == 1)  // Else, if the user wants to get help for a command...
    {
        ConsoleCommand *command = GetCommand(argv[0]);  // Parse the argument as a command.
        if (command != NULL)                            // If a command was found...
        {
            WriteOutput(command->help);                     // Output it's help message,
            WriteOutput("");                                // and a blank line afterwards.
        }
        else                                            // Else, if it couldn't be found...
        {
            std::string output = "Error - the command \"";
            output += argv[0];
            output += "\" could not be found.";
            WriteOutput(output);
        }
    }
    else                        // Else, if the user passed an invaid number of arguments...
    {
        WriteOutput(TC_Console_Error::INVALID_NUM_ARGS);
    }
}

void list(vectStr const& argv)
{
    if (argv.size() == 1)
    {
        if (argv[0] == "-c" || argv[0] == "-commands")
        {
            WriteOutput("The following is a list of all available console commands:");
            std::list<ConsoleCommand*>::iterator cmdIt;
            for (cmdIt = cmdList.begin(); cmdIt != cmdList.end(); cmdIt++)
            {
                WriteOutput((*cmdIt)->name);
            }
        }
        else if (argv[0] == "-a" || argv[0] == "-aliases")
        {
            WriteOutput("The following is a list of all aliases and their mapped commands (alias : command):");
            std::list<CommandAlias*>::iterator aliasIt;
            for (aliasIt = aliasList.begin(); aliasIt != aliasList.end(); aliasIt++)
            {
                WriteOutput((*aliasIt)->alias + " : " + (*aliasIt)->name);
            }
        }
        else if (argv[0] == "-b" || argv[0] == "-binds")
        {
            WriteOutput("The following is a list of all keys bound to console commands:");
            std::list<KeyBind*>::iterator kbIt;
            for (kbIt = kbList.begin(); kbIt != kbList.end(); kbIt++)
            {
                std::string keyStr;
                if (KeyBindToString((*kbIt), keyStr))
                {
                    WriteOutput("  " + keyStr + " = " + (*kbIt)->cmdStr);
                }
            }
        }
        else
        {
            WriteOutput(TC_Console_Error::INVALID_ARG_VALUE);
        }
    }
    else
    {
        TC_Console_Error::WrongArgCount(argv.size(), 1);
    }
}

void loadanim(vectStr const& argv)
{
    if (argv.size() == 0)   // If there were no arguments passed, show an error and return.
    {
        WriteOutput(TC_Console_Error::INVALID_NUM_ARGS_LESS);
        return;
    }
    int *argVals = NULL;    // Array holding the values of each argument.
    if (argv.size() > 1)    // If we have arguments to pass to the Lua script...
    {
        argVals = new int[argv.size() - 1];         // Initialize the argument value array.
        for (size_t i = 1; i < argv.size(); i++)    // So, looping through each argument...
        {
            // First, we determine if the argument represents a constant.
            int argVal;
            if (StringToConst(argv[i], argVal)) // If the argument value was correctly parsed...
            {
                argVals[i-1] = argVal;  // Update the argument vector.
            }
            else            // Else, we try to convert it into an integer...
            {
                std::stringstream argStr(argv[i]);      // Contains the argument.
                int argVal;                             // The argument value itself.    
                if (!(argStr >> argVal))                // So if we could not convert it...
                {
                    WriteOutput(TC_Console_Error::INVALID_ARG_VALUE);  // Output an error, delete
                    delete argVals;                             // the argument vector,
                    return;                                     // and return.
                }
                else                                    // Else, if we did convert it...
                {
                    argVals[i-1] = argVal;                  // Update the argument vector.
                }
            }
        }
    }
    // Now, we get the animation from the Lua animation loader (defined in TCAnimLua.h)
    // and directly pass the new animation to SetAnim (defined in main.h).
    SetAnim(LuaAnimLoader(argv[0].c_str(), argv.size() - 1, argVals));
    // Finally, we delete the memory used for the argument values (if applicable).
    delete argVals;
}

void loadscript(vectStr const& argv)
{
    if (argv.size() == 1)
    {
        if (!LoadScript(argv[0].c_str()))
        {
            WriteOutput("Error - could not load file '" + argv[0] + "'.  Ensure that the "
                        "file exists and is not empty.");
        }
    }
    else
    {
        TC_Console_Error::WrongArgCount(argv.size(), 1);
    }
}

void quality(vectStr const& argv)
{
    static unsigned int lastQuality = 4;    // Default quality is 4.
    switch (argv.size())
    {
        case 0:
        {
            std::stringstream ssOutput;
            ssOutput << "The current quality value is " << lastQuality << ".";
            WriteOutput(ssOutput.str());
            break;
        }

        case 1:
        {
            std::stringstream strQual(argv[0]);
            unsigned int newQuality;
            if (!(strQual >> newQuality) || newQuality < 1 || newQuality > 6)
            {
                WriteOutput(TC_Console_Error::INVALID_ARG_VALUE);
            }
            else
            {
                sphSlices = newQuality * 3;
                sphStacks = newQuality * 3;
                InitDisplayLists();
                lastQuality = newQuality;
            }
            break;
        }

        default:
            WriteOutput(TC_Console_Error::INVALID_NUM_ARGS_MORE);
            break;
    }
}

void quit(vectStr const& argv)
{
    runProgram = false;
    runAnim    = false;
}

void resolution(vectStr const& argv)
{
    if (argv.size() == 2 || argv.size() == 3)
    {
        // If the user wants to set the fullscreen switch...
        if (argv.size() == 3 && (argv[2] == "-f" || argv[2] == "-fullscreen"))
        {
            if (!runProgram)    // They can only do so before the screen is initialized.
            {
                scrFlags |= SDL_FULLSCREEN;
            }
            else                // Otherwise, we just show them an error and return.
            {
                WriteOutput("Error - you cannot switch to fullscreen mode while the "
                            "application is running. Place your command in the config.tcs "
                            "file if you want to use this switch.");
                return;
            }
        }
        int    newHeight,
               newWidth;
        if ( !StringToInt(argv[0], newWidth)  ||
             !StringToInt(argv[1], newHeight) ||
             (newWidth <= 0) || (newHeight <= 0)        )
        {
            WriteOutput(TC_Console_Error::INVALID_ARG_VALUE);
        }
        else
        {
            if (runProgram)
            {
                // First, we obtain the old screen parameters.
                int    oldHeight = screen->h,
                       oldWidth  = screen->w;
                Uint32 oldFlags  = screen->flags;
                // Then, we free the old video screen.
                SDL_FreeSurface(screen);
                // Next, we set the video mode of the new screen.
                screen = SDL_SetVideoMode(newWidth, newHeight, scrBpp, scrFlags);
                if (screen != NULL)     // So, if we got a handle to a new screen...
                {
                    Resize(screen->w, screen->h);   // Resize the OpenGL viewport.
                }
                else                    // Else, try to set the old video mode again.
                {
                    screen = SDL_SetVideoMode(oldWidth, oldHeight, scrBpp, oldFlags);
                    if (screen == NULL)
                    {
                        // Show the appropriate error to the user, and exit.
                        fprintf(stderr, TC_ERROR_SDL_GLVIDMODE, SDL_GetError());
                        exit(2);
                    }
                }
            }
            else
            {
                iScrWidth  = newWidth;
                iScrHeight = newHeight;
            }
        }
    }
    else
    {
        TC_Console_Error::WrongArgCount(argv.size(), 2);
    }
}

void runanim(vectStr const& argv)
{
    switch (argv.size())
    {
        case 0:
            runAnim = !runAnim;
            break;
        case 1:
            bool tmpResult;
            if (StringToBool(argv[0], tmpResult))
            {
                runAnim = tmpResult;
            }
            else
            {
                WriteOutput(TC_Console_Error::INVALID_ARG_VALUE);
            }
            break;
        default:
            WriteOutput(TC_Console_Error::INVALID_NUM_ARGS_MORE);
            break;
    }
}

void screenshot(vectStr const& argv)
{
    if (argv.size() > 1)
    {
        WriteOutput(TC_Console_Error::INVALID_NUM_ARGS_MORE);
        return;
    }

    std::string fname;
    if (argv.size() == 1)   // So, if the user specified a filename...
    {
        fname = argv[0];        // Set the file name as the only argument.
        // If the user did not specify an extension, add .bmp to the filename.
        if (fname.find('.') == std::string::npos) fname += ".bmp";
    }
    else                    // Else, if the user wants to use the default...
    {
        static unsigned int i = 0;      // The current number of screenshots.
        std::stringstream tmpStr;
        tmpStr << "tc" << i << ".bmp";
        fname = tmpStr.str();
        i++;                            // Finally, increment the screenshot count.
    }
    // First, we need to create a temporary SDL surface, based on the current screen.
    SDL_Surface *tmpSurface = SDL_CreateRGBSurface(
        SDL_SWSURFACE, screen->w, screen->h, 24,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    0x000000FF, 0x0000FF00, 0x00FF0000, 0
#else
    0x00FF0000, 0x0000FF00, 0x000000FF, 0
#endif
        );
    if (tmpSurface == NULL)     // If we couldn't get a surface, show an error and return.
    {
        WriteOutput("Error - could not create temporary SDL surface!");
        return;
    }
    // Next, we need to create a temporary pixel buffer.
    char *pixels = (char*)malloc(3 * tmpSurface->w * tmpSurface->h);
    if (pixels == NULL)     // If we couldn't get the memory, show an error and return.
    {
        WriteOutput("Error - could not allocate pixel buffer!");
        SDL_FreeSurface(tmpSurface);
        return;
    }
    // Now that we have a buffer, tell OpenGL to copy pixel data into our buffer.
    glReadPixels(0, 0, tmpSurface->w, tmpSurface->h,
                 GL_RGB, GL_UNSIGNED_BYTE, pixels);
    // Next, we copy the pixel data in the proper order, one horizontal line at a time.
    for (int i = 0; i < tmpSurface->h; i++)
    {
        memcpy( ((char*)tmpSurface->pixels) + tmpSurface->pitch * i,
                pixels + (3 * tmpSurface->w) * (tmpSurface->h - i - 1),
                tmpSurface->w * 3
            );
    }
    // We can free our temporary pixel buffer now, since all the data is in the surface.
    free(pixels);
    // Lastly, we attempt to save the surface as a bitmap image file.
	if (SDL_SaveBMP(tmpSurface, fname.c_str())) // If we couldn't save the image..
    {
        WriteOutput("Error - could not save " + argv[0]);   // Show an error.
    }
    // Finally, before returning, we free all memory associated with our temporary surface.
    SDL_FreeSurface(tmpSurface);
}

void showaxis(vectStr const& argv)
{
    switch (argv.size())
    {
        case 0:
            showAxis = !showAxis;
            break;
        case 1:
            bool tmpResult;
            if (StringToBool(argv[0], tmpResult))
            {
                showAxis = tmpResult;
            }
            else
            {
                WriteOutput(TC_Console_Error::INVALID_ARG_VALUE);
            }
            break;
        default:
            WriteOutput(TC_Console_Error::INVALID_NUM_ARGS_MORE);
            break;
    }
}

void showcube(vectStr const& argv)
{
    switch (argv.size())
    {
        case 0:
            showCube = !showCube;
            break;
        case 1:
            bool tmpResult;
            if (StringToBool(argv[0], tmpResult))
            {
                showCube = tmpResult;
            }
            else
            {
                WriteOutput(TC_Console_Error::INVALID_ARG_VALUE);
            }
            break;
        default:
            WriteOutput(TC_Console_Error::INVALID_NUM_ARGS_MORE);
            break;
    }
}

void showfps(vectStr const& argv)
{
    switch (argv.size())
    {
        case 0:
            showFps = !showFps;
            break;
        case 1:
            bool tmpResult;
            if (StringToBool(argv[0], tmpResult))
            {
                showFps = tmpResult;
            }
            else
            {
                WriteOutput(TC_Console_Error::INVALID_ARG_VALUE);
            }
            break;
        default:
            WriteOutput(TC_Console_Error::INVALID_NUM_ARGS_MORE);
            break;
    }
}

void tick(vectStr const& argv)
{
    switch (argv.size())
    {
        case 0:
            LockAnimMutex();
            currAnim->Tick();
            UnlockAnimMutex();
            break;
        case 1:
            int tmpResult;
            if (StringToInt(argv[0], tmpResult) || tmpResult > 0)
            {
                LockAnimMutex();
                for (int i = 0; i < tmpResult; i++)
                {
                    currAnim->Tick();
                }
                UnlockAnimMutex();
            }
            else
            {
                WriteOutput(TC_Console_Error::INVALID_ARG_VALUE);
            }
            break;
        default:
            WriteOutput(TC_Console_Error::INVALID_NUM_ARGS_MORE);
            break;
    }
}

void tickrate(vectStr const& argv)
{
    if (argv.size() == 1)
    {
        std::stringstream strTickRate(argv[0]);
        Uint16 newTickRate;
        if (!(strTickRate >> newTickRate) || newTickRate == 0 || newTickRate > 1000)
        {
            WriteOutput(TC_Console_Error::INVALID_ARG_VALUE);
        }
        else
        {
            SetTickRate(newTickRate);
        }
    }
    else
    {
        TC_Console_Error::WrongArgCount(argv.size(), 1);
    }
}

void wait(vectStr const& argv)
{
    if (argv.size() == 2)
    {
        unsigned int currMode = 0;
        // First, we parse the current wait mode.
        if (argv[0] == "-ms" || argv[0] == "-milliseconds")
        {
            currMode = 1;
        }
        else if (argv[0] == "-s" || argv[0] == "-seconds")
        {
            currMode = 2;
        }
        else if (argv[0] == "-t" || argv[0] == "-ticks")
        {
            currMode = 3;
        }
        else if (argv[0] == "-i" || argv[0] == "-iterations")
        {
            if (!nullAnim)  // So long as we have a non-blank animation...
            {
                currMode = 4;
            }
            else            // Else, if we do have a blank animation...
            {
                WriteOutput("Error - you cannot use iteration mode without an "
                    "animation loaded.");
                SetWaitMode(0, 0);
                return;
            }
        }
        else
        {
            WriteOutput("Error - mode '" + argv[0] + "' is unrecognized.");
            SetWaitMode(0, 0);
            return;
        }
        // We also do a check to see if the animation is paused, and we are waiting
        // for a certain iteration or tick amount.
        if ((currMode == 3 || currMode == 4) && !runAnim)
        {
            WriteOutput("Error - you cannot wait for a tick or iteration value "
                "while the animation is not being updated.");
            SetWaitMode(0, 0);
            return;
        }
        // If we get here, then we can parse the second argument as the delay value.
        int currDelay;
        // So, if we couldn't convert the value, or the delay value was negative...
        if (!StringToInt(argv[1], currDelay) || currDelay <= 0)
        {
            // Write the appropriate error to the console, and reset the wait mode/amount.
            WriteOutput(TC_Console_Error::INVALID_ARG_VALUE);
            SetWaitMode(0, 0);
        }
        // Otherwise, if we got a valid delay value, we simply set the wait mode.
        else
        {
            SetWaitMode(currMode, currDelay);
        }
    }
    else
    {
        TC_Console_Error::WrongArgCount(argv.size(), 2);
    }
}


///
/// \brief Register Commands
///
/// This function performs the initial console command and alias registration, by creating
/// a new \ref ConsoleCommand object for each function listed in this namespace, and adding
/// the command to the cmdList object.
///
/// \remarks When adding a new console command, it must be registered in this function.
///
void RegisterCommands()
{
    cmdList.push_back(new ConsoleCommand("bind", bind,
        "Assigns a key combination to a particular console command. Usage:\n\n"
        "    bind [flags] key cmd     Where each argument is as follows:\n\n"
        "    [flags] Any combination (or none) of the following modifiers:\n"
        "              -a, -alt    The Alt key.\n"
        "              -c, -ctrl   The Ctrl key.\n"
        "              -s, -shift  The Shift key.\n"       
        "    key     Clears the console history.\n"
        "    cmd     The command to be bound to the key (use quotes for arguments).\n\n"
        "To unbind a key, see the unbind command. To list all key binds, use the list "
        "command."));

    cmdList.push_back(new ConsoleCommand("clear", clear,
        "Clears the console output (default) or the console history. Usage:\n\n"
        "    clear [arg]     Where [arg] is one of the following:\n"
        "    -o, -output     Clears the console output.\n"
        "    -h, -history    Clears the console history.\n\n"
        "If [arg] is omitted, the command will default to -output."));

    cmdList.push_back(new ConsoleCommand("color", color,
        "Used to change the color of the on or off LEDs. Has no effect on RGB animations. "
        "Usage:\n\n"
        "    color [state] red green blue [alpha]\n\n"
        "Where [state] can be -on or -off (if omitted, defaults to -on), and red, green, "
        "blue, and the optional alpha components are integer values from 0 to 255. If the "
        "alpha component is omitted, its value is unmodified.  Examples:\n\n"
        "    color 255 0 0             Sets the color of the on LEDs to red.\n"
        "    color -on 0 0 255         Sets the color of the on LEDs to blue.\n"
        "    color -off 0 255 0 127    Sets the color of the off LEDs to green at 50% "
        "transparency."));

    cmdList.push_back(new ConsoleCommand("cubesize", cubesize,
        "Resets the dimensions of the cube. You can pass a single value to make a cube, "
        "or pass three (in the order x, y, z) to make a rectangular prism. Usage:\n\n"
        "    cubesize 8        Sets the size to an 8*8*8 cube.\n"
        "    cubesize 4 6 8    Sets the size to an 4*6*8 (x*y*z) rectangular prism.\n\n"
        "Calling this function without any arguments displays the current cube size. "
        "Note that this function will close the currently running animation."));

    cmdList.push_back(new ConsoleCommand("echo", echo,
        "Outputs each passed command line argument as it is parsed. Usage:\n\n"
        "    echo [-v | -verbose] [-o | -omit] [arg1, arg2, ...]\n\n"
        "If the flags are omitted, each argument is written to the console output in the "
        "order they were passed to the function, with a space character between them.\n\n"
        "If -v or -verbose is specified, each argument is numbered on a new line.  If -o "
        "or -omit is specified, the first two arguments (e.g. the two flags) are omitted "
        "from the output. The omit flag has no effect if verbose mode is not specified."));

    cmdList.push_back(new ConsoleCommand("fpsmax", fpsmax,
        "Sets the maximum framerate of the rendering engine. Usage:\n\n"
        "    fpsmax 60    Sets the maximum framerate to 60 frames per second (FPS).\n"
        "    fpsmax 0     Disables the engine framerate limiter (may cause high CPU usage).\n\n"
        "Call this command without any arguments to display the current FPS limit."));

    cmdList.push_back(new ConsoleCommand("help", help,
        "Used to obtain help information about a particular command, or display a quick "
        "help guide. Usage:\n\n"
        "    help [cmd]    Where [cmd] is the name of a particular command (e.g. help "
        "list).\n\nIf [cmd] is omitted, the quick help guide is shown."));

    cmdList.push_back(new ConsoleCommand("list", list,
        "Used to list the available console commands or command aliases.  Usage:\n\n"
        "    list arg         Where arg is one of the following\n"
        "    -c, -commands    Lists all available console commands.\n"
        "    -a, -aliases     Lists all aliases mapped to other console commands."));

    cmdList.push_back(new ConsoleCommand("loadanim", loadanim,
        "Loads an animation from a file in the /animations directory.  Usage:\n\n"
        "    loadanim filename [arg1, arg2, arg3, ...]\n\n"
        "Where filename is the name of the animation file (including extension), and the "
        "remaining arguments are any arguments required by the animation (extra arguments "
        "are ignored, but passing too few may result in an error).  Examples:\n\n"
        "    loadanim sendplane.lua    Loads the sendplane.lua animation.\n"
        "    loadanim rain.lua 4       Loads the rain.lua animation with 4 rain drops.\n\n"
        "Note that the .lua extension is optional (i.e. \"loadanim rain\" will load the "
        "file rain.lua, unless the file rain exists - which will be executed instead)."));

    cmdList.push_back(new ConsoleCommand("loadscript", loadscript,
        "Loads a script from a file. Usage:\n\n"
        "    loadscript filename\n\n"
        "Where filename is the name of the script (including extension, usually .tcs)."));

    cmdList.push_back(new ConsoleCommand("quality", quality,
        "Changes the polygon count of the individual LED spheres making up the cube. "
        "Lowering the quality may result in higher performance at the cost of visual "
        "appearance. Usage:\n\n"
        "    quality q    Where q is an integer from 1 (lowest quality) to 6 (highest)."
        "\n\nThe default quality is 4."));
    
    cmdList.push_back(new ConsoleCommand("quit", quit,
        "Quits/closes Triclysm immediately.  Any passed arguments are ignored."));

    cmdList.push_back(new ConsoleCommand("resolution", resolution,
        "Sets the screen resolution of the program.  Usage:\n\n"
        "    resolution width height    Where width and height are the new resolutions "
        "(positive integer values) for the screen.\n\n"
        "If the screen mode could not be set, the screen is set back to the old one. "
        "Before the program is initialized, you can call this program with a fullscreen "
        "switch (e.g. resolution 640 480 -f).  Most people place it in the config.tcs "
        "file, but it can be used any time before the screen is initialized."));

    cmdList.push_back(new ConsoleCommand("runanim", runanim,
        "Toggles or sets the animation from updating.  Usage:\n\n"
        "    runanim [bool]    Where [bool] is an optional boolean parameter.\n\n"
        "If [bool] evaluates to true, the animation will begin updating.  If [bool] "
        "evaluates to false, the animation will stop.  If [bool] is omitted, the running "
        "state of the animation is toggled."));

    cmdList.push_back(new ConsoleCommand("screenshot", screenshot,
        "Saves a bitmap image of the current screen, at the running resolution.  Usage:\n\n"
        "    screenshot [filename]    Where [filename] is an optional string parameter.\n\n"
        "If [filename] is omitted, screenshots are saved in increasing numbers prefixed "
        "with tc (tc0.bmp, tc1.bmp, tc2.bmp, etc...).  If no file extension is specified, "
        ".bmp is appended to the file name automatically.  To avoid saving the console "
        "text when taking screenshots, consider binding this command to a key."));

    cmdList.push_back(new ConsoleCommand("showaxis", showaxis,
        "Toggles or sets the three coordinate axes from being rendered.  Usage:\n\n"
        "    showaxis [bool]    Where [bool] is an optional boolean parameter.\n\n"
        "If [bool] evaluates to true, the axes will be rendered.  If [bool] evaluates to "
        "false, the axes will not be rendered.  If [bool] is omitted, the axes rendering "
        "state is toggled."));

    cmdList.push_back(new ConsoleCommand("showcube", showcube,
        "Toggles or sets the LED cube from being rendered.  Usage:\n\n"
        "    showcube [bool]    Where [bool] is an optional boolean parameter.\n\n"
        "If [bool] evaluates to true, the cube will be rendered.  If [bool] evaluates to "
        "false, the cube will not be rendered.  If [bool] is omitted, the cube rendering "
        "state is toggled."));

    cmdList.push_back(new ConsoleCommand("showfps", showfps,
        "Toggles or sets the FPS counter from being displayed.  Usage:\n\n"
        "    showfps [bool]    Where [bool] is an optional boolean parameter.\n\n"
        "If [bool] evaluates to true, the FPS counter is shown.  If [bool] evaluates to "
        "false, the FPS counter will not be drawn.  If [bool] is omitted, the state of "
        "the FPS counter is toggled."));
        
    cmdList.push_back(new ConsoleCommand("tick", tick,
        "Advances the animation state by the set number of ticks.  Usage:\n\n"
        "    tick [amount]    Where [amount] is an optional integer parameter.\n\n"
        "If [amount] is omitted, the animation state advances by one tick."));
        

    cmdList.push_back(new ConsoleCommand("tickrate", tickrate,
        "Sets the current tickrate for running animations (or updates per second). Usage:\n\n"
        "    tickrate [newrate]    Where [newrate] is an optional integer parameter.\n\n"
        "If omitted, the current tickrate is displayed.  If set, [newrate] must be a valid "
        "integer between 1 and 1000."));

    cmdList.push_back(new ConsoleCommand("wait", wait,
        "Delays execution of any further console commands by the set amount.  Usage:\n\n"
        "    wait mode delay\n\n"
        "Where mode is one of the following flags (specifying the units of delay):\n"
        "    -ms, -milliseconds    The value of delay is in milliseconds.\n"
        "    -s,  -seconds         The value of delay is in seconds.\n"
        "    -t,  -ticks           The value of delay is in ticks.\n"
        "    -i,  -iterations      The value of delay is in iterations.\n"
        "And where delay is a positive integer representing the delay value based on the "
        "passed mode flag.\n\nUse caution when specifying a delay value in iterations. If "
        "the current animation does not increment the internal iteration counter, the "
        "wait condition may never bet met, and you may have to restart the program."));
    
    // Finally, we make sure the command and alias lists are sorted (alphabetically).
    cmdList.sort(cmpConsoleCmd);
    aliasList.sort(cmpCmdAlias);
}

}
