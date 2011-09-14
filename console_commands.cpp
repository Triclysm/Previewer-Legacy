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
///        the Triclysm Previewer command console.  Any commands that are added should
///        also be added to the RegisterCommands function at the bottom of this file.
///

#include <string>
#include <cctype>
#include <sstream>
#include <vector>
#include "console.h"
#include "render.h"
#include "main.h"
#include "TCAnim.h"
#include "TCAnimLua.h"

///
/// \brief String Remove Spaces
///
/// Modifies the passed string by removing each space character from the string.
///
/// \param toConvert The string to remove the spaces from.
/// \param spaceChar The character to remove from the string (' ' by default).
///
void StringRemoveSpaces(std::string &toRemove, char spaceChar = ' ')
{
    size_t spacePos = toRemove.find(spaceChar);
    while (spacePos != std::string::npos)
    {
        toRemove.erase(spacePos, 1);
        spacePos = toRemove.find(spaceChar);
    }
}


///
/// \brief String To Lowercase
///
/// Modifies the passed string and converts each character to its lowercase equivalent.
///
/// \param toConvert The string to convert to lowercase.
///
void StringToLowercase(std::string &toLower)
{
    for (size_t i = 0; i < toLower.length(); i++)
    {
        toLower[i] = std::tolower(toLower[i]);
    }
}


///
/// \brief String To Uppercase
///
/// Modifies the passed string and converts each character to its uppercase equivalent.
///
/// \param toConvert The string to convert to uppercase.
///
void StringToUppercase(std::string &toUpper)
{
    for (size_t i = 0; i < toUpper.length(); i++)
    {
        toUpper[i] = std::toupper(toUpper[i]);
    }
}


///
/// \brief String To Integer
///
/// Attempts to convert the passed string into an integer.
///
/// \param toConvert The string representing the integer value to convert.
/// \param result    The variable to store the result in (if applicable).
///
/// \returns True if the conversion was successful, false otherwise.
/// \remarks If this function returns false, the value of the result is undefined, and
///          should not be used.
///
bool StringToInt(std::string const& toConvert, int &result)
{
    std::stringstream ssToConv(toConvert);
    return (bool)(ssToConv >> result);
}


///
/// \brief String To Boolean
///
/// Attempts to convert the passed string into a boolean.  Valid string values are "true",
/// "false", or any number which evaluates to 0 or 1.
/// 
/// \param toConvert The string representing the boolean value to convert.
/// \param result    The variable to store the result in (if applicable).
///
/// \returns True if the conversion was successful, false otherwise.
/// \remarks If this function returns false, the value of the result is undefined, and
///          should not be used.
///
bool StringToBool(std::string const& toConvert, bool &result)
{
    // First, we trim any existing spaces from the string, and convert it to lowercase.
    std::string lcStr(toConvert);
    StringRemoveSpaces(lcStr);
    StringToLowercase(lcStr);
    // Does the remaining string match "true" or "false"?
    if (lcStr == "true")
    {
        result = true;
        return true;
    }
    if (lcStr == "false")
    {
        result = false;
        return true;
    }
    // Finally, try to convert the remaining string to a number.
    std::stringstream ssToConv(lcStr);
    int conVal;
    if (ssToConv >> conVal)
    {
        if (conVal == 1)
        {
            result = true;
            return true;
        }
        if (conVal == 0)
        {
            result = false;
            return true;
        }
    }
    // Lastly, if we get here, no acceptable conversion was found, so return false.
    return false;
}


///
/// \brief String To Triclysm Constant
///
/// Attempts to convert the passed string into a Triclysm constant value (e.g. "X_AXIS"
/// should be evaluated to what is #defined by the macro TC_X_AXIS).
///
/// \param toConvert The string representing the constant value to convert.
/// \param result    The variable to store the result in (if applicable).
///
/// \returns True if the conversion was successful, false otherwise.
/// \remarks If this function returns false, the value of the result is undefined, and
///          should not be used.
///
bool StringToConst(std::string const& toConvert, int &result)
{
    std::string toConv(toConvert);
    StringToUppercase(toConv);      // First, we convert the passed string to uppercase,
    StringRemoveSpaces(toConv);     // and remove all spaces from it.
    // So, if the length of the remaining string matches ?_AXIS...
    if (toConv.length() == 6)
    {
        if (toConv == "X_AXIS")
        {
            result = TC_X_AXIS;
            return true;
        }
        if (toConv == "Y_AXIS")
        {
            result = TC_Y_AXIS;
            return true;
        }
        if (toConv == "Z_AXIS")
        {
            result = TC_Z_AXIS;
            return true;
        }
    }
    // Else, if the length of the remaining string matches ??_PLANE...
    else if (toConv.length() == 8)
    {
        if (toConv == "XY_PLANE")
        {
            result = TC_XY_PLANE;
            return true;
        }
        if (toConv == "YZ_PLANE")
        {
            result = TC_YZ_PLANE;
            return true;
        }
        if (toConv == "ZX_PLANE")
        {
            result = TC_ZX_PLANE;
            return true;
        }
    }
    // If the control gets here, we didn't find a constant, so we return false.
    return false;
}


namespace TCC_ERROR
{
    const std::string INVALID_NUM_ARGS      = "Error - invalid number of arguments passed.",
                      INVALID_NUM_ARGS_LESS = "Error - not enough arguments passed.",
                      INVALID_NUM_ARGS_MORE = "Error - too many arguments passed.",
                      INVALID_ARG_VALUE     = "Error - argument has invalid value.";

    void WrongArgCount(size_t actual, size_t expected)
    {
        if (actual < expected)
        {
            WriteOutput(TCC_ERROR::INVALID_NUM_ARGS_LESS);
        }
        else if (actual > expected)
        {
            WriteOutput(TCC_ERROR::INVALID_NUM_ARGS_MORE);
        }
    }
}

namespace TCC_COMMANDS
{

void showfps(std::vector<std::string> const& argv)
{
    showFps = !showFps;
}

void runanim(std::vector<std::string> const& argv)
{
    if (argv.size() == 0)
    {
        runAnim = !runAnim;
    }
    else if (argv.size() == 1)
    {
        if (argv[0].length() == 1)
        {
            if (argv[0][0] == '0')
            {
                runAnim = false;
            }
            else if (argv[0][0] == '1')
            {
                runAnim = true;
            }
            else
            {
                WriteOutput(TCC_ERROR::INVALID_ARG_VALUE);
            }
        }
        else
        {
            WriteOutput(TCC_ERROR::INVALID_ARG_VALUE);
        }
    }
    else
    {
        WriteOutput(TCC_ERROR::INVALID_NUM_ARGS_MORE);
    }
}

void echo(std::vector<std::string> const& argv)
{
    for (size_t i = 0; i < argv.size(); i++)
    {
        std::string output;
        output = "Arg ";
        output += ('0' + i);
        WriteOutput(output + " = " + argv[i]);
    }
}

void help(std::vector<std::string> const& argv)
{
    if (argv.size() == 0)
    {
        WriteOutput("Welcome to " TCP_NAME " version " TCP_VERSION "!");
        WriteOutput("To get help for a command, type \"help [command]\".");
        WriteOutput("To see a list of commands, type \"list commands\" (type \"help list\" to see what else you can list).");
    }
    else if (argv.size() == 1)
    {
        ConsoleCommand *command = GetCommand(argv[0]);
        if (command != NULL)
        {
            WriteOutput(command->help);
        }
        else
        {
            WriteOutput("Help entry not found.");
        }
    }
}

void clear(std::vector<std::string> const& argv)
{
    if (argv.size() == 1)
    {
        if (argv[1] == "output")
        {
            ClearOutput();
        }
        else if (argv[1] == "history")
        {
            ClearHistory();
        }
        else
        {
            WriteOutput(TCC_ERROR::INVALID_ARG_VALUE);
        }
    }
    else
    {
        WriteOutput("Clears the screen or history.  Usage:");
        WriteOutput("   clear [option]");
        WriteOutput("Where [option] is either history or screen.");
    }
}


void list(std::vector<std::string> const& argv)
{
    if (argv.size() == 1)
    {
        if (argv[0] == "commands")
        {
            std::list<ConsoleCommand*>::iterator cmdIt;
            for (cmdIt = cmdList.begin(); cmdIt != cmdList.end(); cmdIt++)
            {
                WriteOutput((*cmdIt)->name);
            }
        }
        else if (argv[0] == "aliases")
        {
            WriteOutput("The following is a list of aliases and their mapped commands (alias : command):");
            std::list<CommandAlias*>::iterator aliasIt;
            for (aliasIt = aliasList.begin(); aliasIt != aliasList.end(); aliasIt++)
            {
                WriteOutput((*aliasIt)->alias + " : " + (*aliasIt)->name);
            }
        }
        else if (argv[0] == "animations")
        {
            WriteOutput("Sorry, still working on this... For now, just try \"sendplane\".");
        }
        else
        {
            WriteOutput(TCC_ERROR::INVALID_ARG_VALUE);
        }
    }
    else
    {
        TCC_ERROR::WrongArgCount(argv.size(), 1);
    }
}

/*void sendplane(std::vector<std::string> const& argv)
{
    SetAnim(new TCAL_SendPlane(cubeSize, TC_XY_PLANE, 1, true, false));
}*/

/*void rain(std::vector<std::string> const& argv)
{
    if (argv.size() == 1)
    {
        std::stringstream dropsStr(argv[0]);
        Uint16 newDrops;
        if (!(dropsStr >> newDrops) || newDrops == 0 || newDrops > 255)
        {
            WriteOutput(TCC_ERROR::INVALID_ARG_VALUE);
        }
        else
        {
            SetAnim(new TCAL_Rain(cubeSize, TC_XY_PLANE, true, (byte)newDrops));
        }
    }
    else
    {
        TCC_ERROR::WrongArgCount(argv.size(), 1);
    }
}*/

void fpsmax(std::vector<std::string> const& argv)
{
    if (argv.size() == 1)
    {
        std::stringstream fpsStr(argv[0]);
        Uint16 newFpsLimit;
        if (!(fpsStr >> newFpsLimit) || newFpsLimit < 0)
        {
            WriteOutput(TCC_ERROR::INVALID_ARG_VALUE);
        }
        else
        {
            SetFpsLimit(newFpsLimit);
        }
    }
    else
    {
        TCC_ERROR::WrongArgCount(argv.size(), 1);
    }
}

void cubesize(std::vector<std::string> const& argv)
{
    if (argv.size() == 1)
    {
        std::stringstream strSize(argv[0]);
        Uint16 newSize;
        if (!(strSize >> newSize) || newSize == 0 || newSize > 100)
        {
            WriteOutput(TCC_ERROR::INVALID_ARG_VALUE);
        }
        else
        {
            SetCubeSize((byte)newSize, (byte)newSize, (byte)newSize);
        }
    }
    else if (argv.size() == 3)
    {
        std::stringstream strSizeX(argv[0]),
                          strSizeY(argv[1]),
                          strSizeZ(argv[2]);
        Uint16 newSize[3];
        if (    !(strSizeX >> newSize[0]) || newSize[0] == 0 || newSize[0] > 100
             || !(strSizeY >> newSize[1]) || newSize[1] == 0 || newSize[1] > 100
             || !(strSizeZ >> newSize[2]) || newSize[2] == 0 || newSize[2] > 100 )
        {
            WriteOutput(TCC_ERROR::INVALID_ARG_VALUE);
        }
        else
        {
            SetCubeSize((byte)newSize[0], (byte)newSize[1], (byte)newSize[2]);
        }
    }
    else
    {
        WriteOutput(TCC_ERROR::INVALID_NUM_ARGS);
    }
}

void color(std::vector<std::string> const& argv)
{
    int argOffset = 0;
    bool offLed   = false;
    // So, if we have 4 or 5 arguments, and the first argument is a switch...
    if ((argv.size() == 4 || argv.size() == 5) && argv[0][0] == '-')
    {
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
            WriteOutput(TCC_ERROR::INVALID_ARG_VALUE);
        }
        argOffset = 1;
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
            ChangeLedColor(numColors, newColor, offLed);
        }
        else
        {
            WriteOutput(TCC_ERROR::INVALID_ARG_VALUE);
        }
    }
    else
    {
        WriteOutput(TCC_ERROR::INVALID_NUM_ARGS);
    }
    /*
    if (argv.size() == 3 || argv.size() == 4)
    {
        W
    }
    if (argv.size() == 4)
    {
        std::stringstream newColStr[4] = { std::stringstream(argv[0]),
                                           std::stringstream(argv[1]),
                                           std::stringstream(argv[2]),
                                           std::stringstream(argv[3]) };
        Uint16 newColVal[4];
        if ( !(newColStr[0] >> newColVal[0]) || newColVal[0] < 0 || newColVal[0] > 255 ||
             !(newColStr[1] >> newColVal[1]) || newColVal[1] < 0 || newColVal[1] > 255 ||
             !(newColStr[2] >> newColVal[2]) || newColVal[2] < 0 || newColVal[2] > 255 ||
             !(newColStr[3] >> newColVal[3]) || newColVal[3] < 0 || newColVal[3] > 255 )
        {
            WriteOutput(TCC_ERROR::INVALID_ARG_VALUE);
        }
        else
        {
            SetCubeSize((byte)newSize[0], (byte)newSize[1], (byte)newSize[2]);
        }
    }
    else
    {
        WriteOutput(TCC_ERROR::INVALID_NUM_ARGS);
    }*/
    //Uint16 x[] = {255, 0, 0};
    //ChangeLedColor(3, x, false);
}

void tickrate(std::vector<std::string> const& argv)
{
    if (argv.size() == 1)
    {
        std::stringstream strTickRate(argv[0]);
        Uint16 newTickRate;
        if (!(strTickRate >> newTickRate) || newTickRate == 0)
        {
            WriteOutput(TCC_ERROR::INVALID_ARG_VALUE);
        }
        else
        {
            SetTickRate(newTickRate);
        }
    }
    else
    {
        TCC_ERROR::WrongArgCount(argv.size(), 1);
    }
}

void quality(std::vector<std::string> const& argv)
{
    if (argv.size() == 1)
    {
        std::stringstream strQual(argv[0]);
        unsigned int newQuality;
        if (!(strQual >> newQuality) || newQuality < 1 || newQuality > 5)
        {
            WriteOutput(TCC_ERROR::INVALID_ARG_VALUE);
        }
        else
        {
            sphSlices = newQuality * 3;
            sphStacks = newQuality * 3;
            InitDisplayLists();
        }
    }
    else
    {
        TCC_ERROR::WrongArgCount(argv.size(), 1);
    }
}

void loadanim(std::vector<std::string> const& argv)
{
    if (argv.size() == 0)   // If there were no arguments passed, show an error and return.
    {
        WriteOutput(TCC_ERROR::INVALID_NUM_ARGS_LESS);
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
                    WriteOutput(TCC_ERROR::INVALID_ARG_VALUE);  // Output an error, delete
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
//    cmdList.push_back(new ConsoleCommand("name", func, "desc", "helpentry"));
    // Finally, we sort the command list.
    cmdList.push_back(new ConsoleCommand("echo", echo,
        "Ouptuts each passed command line argument as it is parsed."));

    cmdList.push_back(new ConsoleCommand("showfps", showfps,
        "Toggles the FPS counter on or off.  To set explicitly, 0 or 1 as the only argument."));

    cmdList.push_back(new ConsoleCommand("runanim", runanim,
        "Toggles the animation from updating.  To set explicitly, 0 or 1 as the only argument."));

    cmdList.push_back(new ConsoleCommand("help", help,
        "The help command prints the help entries for a single command (passed as the argument)."));

    cmdList.push_back(new ConsoleCommand("list", list, 
        "Used to pass one of the following (use as the only argument): commands, aliases, animations."));

    cmdList.push_back(new ConsoleCommand("fpsmax", fpsmax, "Used to set the maximum framerate."));

    cmdList.push_back(new ConsoleCommand("cubesize", cubesize,
        "Sets the size of the cube.  You can use a single value (to make a cube), "
        "or three to make a rectangular prism (passed in the order x, y, z)."));

    cmdList.push_back(new ConsoleCommand("tickrate", tickrate,
        "Sets the current tick rate (ticks per second).  The default value is 30."));

    cmdList.push_back(new ConsoleCommand("quality", quality,
        "Changes the drawing quality of the cube.  Valid values are 1 to 5."));

    cmdList.push_back(new ConsoleCommand("loadanim", loadanim,
        "Help entry not done."));

    cmdList.push_back(new ConsoleCommand("color", color,
        "Sets the color of the on or off LEDs.  Values should be passed in R G B format, from 0 to 255. Examples:\n"
        "  color 255 0 0            Sets the color of the on LEDs to red.\n"
        "  color -on 0 0 255        Sets the color of the on LEDs to blue.\n"
        "  color -off 0 255 0       Sets the color of the off LEDs to green."));

/*    cmdList.push_back(new ConsoleCommand("sendplane", sendplane,
        ""));

    cmdList.push_back(new ConsoleCommand("rain", rain,
        "One argument, the number of drops."));
*/
    aliasList.push_back(new CommandAlias("runanim", "p"));
    cmdList.sort(cmpConsoleCmd);
    aliasList.sort(cmpCmdAlias);
}

}