/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                          Lua Animation Object Source Code                           *
 *                                      TRICLYSM                                       *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *  This file contains the implementation of the TCAnimLua class as defined by the     *
 *  TCAnimLua.h header file.  This class inherets the base animation class (TCAnim),   *
 *  and allows the passed Lua state to be used to override the Initialize and Update   *
 *  methods required for an animation.  Also, the functions to modify the animation's  *
 *  internal cubestate are also registered with Lua.  See the TC_Lua_Functions         *
 *  namespace and the animbase.lua file for more details.                              *
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
/// \file  TCAnimLua.cpp
/// \brief This file contains the implementation of the TCAnimLua class as defined by the
///        TCAnimLua.h header file.
///

#include <lua.hpp>          // The Lua C++ header file.
#include <string>           // String object library.
#include "main.h"           // Used to access the global cube size.
#include "console.h"        // Used to print error messages to the console.
#include "TCAnim.h"         // The base TCAnim object header.
#include "TCAnimLua.h"      // Definition of the TCAnimLua class.


///
/// \brief Triclysm Lua Functions
///
/// A namespace containing all of the functions to be exposed to Lua animations, which
/// are nested within more namespaces depending on the number of animation colors. These
/// functions can be registered to a given Lua state by calling the RegisterCommands
/// function in the appropriate sub-namespace.
///
/// \remarks Some of these functions make references to the current animation's cube state.
///
namespace TC_Lua_Functions
{
    TCAnimLua *currAnim;    // Pointer to the currently registered TCAnimLua object.

    ///
    /// \brief Common Lua Functions
    ///
    namespace Common
    {
        int Shift(lua_State *L)
        {
            int argc = lua_gettop(L);
            if (currAnim != NULL && argc == 2)
            {
                currAnim->Shift(
                    (byte)lua_tointeger(L, 1), 
                    (byte)lua_tointeger(L, 2)  );
            }
            return 0;
        }

        int DoneIteration(lua_State *L)
        {
            int argc = lua_gettop(L);
            if (currAnim != NULL && argc == 0)
            {
                currAnim->DoneIteration();
            }
            return 0;
        }

        int WriteConsole(lua_State *L)
        {
            int argc = lua_gettop(L);
            if (argc == 1)
            {
                WriteOutput(lua_tostring(L, 1));
            }
            return 0;
        }
    
        void RegisterCommands(lua_State *L)
        {
            lua_register(L, "Shift",         Shift);
            lua_register(L, "DoneIteration", DoneIteration);
            lua_register(L, "WriteConsole",  WriteConsole);
        }
    }
    
    ///
    /// \brief Black & White Lua Functions
    ///
    /// This namespace contains functions to be registered with the Lua state when the
    /// animation does not use colors.
    ///
    namespace BW
    {
        int SetVoxelState(lua_State *L)
        {
            int argc = lua_gettop(L);
            if (currAnim != NULL && argc == 4)
            {
                currAnim->cubeState[0]->SetVoxelState(
                    (byte)lua_tointeger(L, 1), 
                    (byte)lua_tointeger(L, 2),
                    (byte)lua_tointeger(L, 3),
                    ((lua_toboolean(L, 4)) ? true : false) );
            }
            return 0;
        }

        int GetVoxelState(lua_State *L)
        {
            int argc = lua_gettop(L);
            if (currAnim != NULL && argc == 3)
            {
                lua_pushboolean(L, currAnim->cubeState[0]->GetVoxelState(
                    (byte)lua_tointeger(L, 1),
                    (byte)lua_tointeger(L, 2),
                    (byte)lua_tointeger(L, 3) ));
                return 1;
            }
            else
            {
                return 0;
            }
        }

        int SetColumnState(lua_State *L)
        {
            int argc = lua_gettop(L);
            if (currAnim != NULL && argc == 4)
            {
                currAnim->cubeState[0]->SetColumnState(
                    (byte)lua_tointeger(L, 1), 
                    (byte)lua_tointeger(L, 2),
                    (byte)lua_tointeger(L, 3),
                    ((lua_toboolean(L, 4)) ? true : false) );
            }
            return 0;
        }

        int GetColumnState(lua_State *L)
        {
            int argc = lua_gettop(L);
            if (currAnim != NULL && argc == 4)
            {
                lua_pushboolean(L, currAnim->cubeState[0]->GetColumnState(
                    (byte)lua_tointeger(L, 1),
                    (byte)lua_tointeger(L, 2),
                    (byte)lua_tointeger(L, 3),
                    (lua_toboolean(L, 4) ? true : false) ));
                return 1;
            }
            else
            {
                return 0;
            }
        }

        int SetPlaneState(lua_State *L)
        {
            int argc = lua_gettop(L);
            if (currAnim != NULL && argc == 3)
            {
                currAnim->cubeState[0]->SetPlaneState(
                    (byte)lua_tointeger(L, 1), 
                    (byte)lua_tointeger(L, 2),
                    ((lua_toboolean(L, 3)) ? true : false) );
            }
            return 0;
        }

        int GetPlaneState(lua_State *L)
        {
            int argc = lua_gettop(L);
            if (currAnim != NULL && argc == 3)
            {
                lua_pushboolean(L, currAnim->cubeState[0]->GetPlaneState(
                    (byte)lua_tointeger(L, 1),
                    (byte)lua_tointeger(L, 2),
                    (lua_toboolean(L, 3) ? true : false) ));
                return 1;
            }
            else
            {
                return 0;
            }
        }

        void RegisterCommands(lua_State *L)
        {
            lua_register(L, "SetVoxelState",  SetVoxelState);
            lua_register(L, "GetVoxelState",  GetVoxelState);
            lua_register(L, "SetColumnState", SetColumnState);
            lua_register(L, "GetColumnState", GetColumnState);
            lua_register(L, "SetPlaneState",  SetPlaneState);
            lua_register(L, "GetPlaneState",  GetPlaneState);
        }
    }
    
    ///
    /// \brief Greyscale (Monochrome) Lua Functions
    ///
    /// This namespace contains functions to be registered with the Lua state when the
    /// animation needs to use only one color.
    ///
    namespace Greyscale
    {
        int SetVoxelValue(lua_State *L)
        {
            int argc = lua_gettop(L);
            if (currAnim != NULL && argc == 4)
            {
                currAnim->SetVoxelColor(
                    (byte)lua_tointeger(L, 1),
                    (byte)lua_tointeger(L, 2),
                    (byte)lua_tointeger(L, 3),
                    (byte)lua_tointeger(L, 4) );
            }
            return 0;
        }
        
        int GetVoxelValue(lua_State *L)
        {
            int argc = lua_gettop(L);
            if (currAnim != NULL && argc == 3)
            {
                lua_pushinteger(L, currAnim->cubeState[0]->GetVoxelState(
                    (byte)lua_tointeger(L, 1),
                    (byte)lua_tointeger(L, 2),
                    (byte)lua_tointeger(L, 3) ));
                return 1;
            }
            else
            {
                return 0;
            }
        }
        
        int SetColumnValue(lua_State *L)
        {
            int argc = lua_gettop(L);
            if (currAnim != NULL && argc == 4)
            {
                currAnim->SetColumnColor(
                    (byte)lua_tointeger(L, 1),
                    (byte)lua_tointeger(L, 2),
                    (byte)lua_tointeger(L, 3),
                    (byte)lua_tointeger(L, 4));
            }
            return 0;
        }
    
        int CompareColumnValue(lua_State *L)
        {
            int argc = lua_gettop(L);
            if (currAnim != NULL && argc == 4)
            {
                lua_pushboolean(L, currAnim->CompareColumnColor(
                    (byte)lua_tointeger(L, 1),
                    (byte)lua_tointeger(L, 2),
                    (byte)lua_tointeger(L, 3),
                    (byte)lua_tointeger(L, 4) ));
                return 1;
            }
            return 0;
        }
        
        int SetPlaneValue(lua_State *L)
        {
            int argc = lua_gettop(L);
            if (currAnim != NULL && argc == 3)
            {
                currAnim->SetPlaneColor(
                    (byte)lua_tointeger(L, 1),
                    (byte)lua_tointeger(L, 2),
                    (byte)lua_tointeger(L, 3));
            }
            return 0;
        }
    
        int ComparePlaneValue(lua_State *L)
        {
            int argc = lua_gettop(L);
            if (currAnim != NULL && argc == 3)
            {
                lua_pushboolean(L, currAnim->ComparePlaneColor(
                    (byte)lua_tointeger(L, 1),
                    (byte)lua_tointeger(L, 2),
                    (byte)lua_tointeger(L, 3) ));
                return 1;
            }
            return 0;
        }

        void RegisterCommands(lua_State *L)
        {
            lua_register(L, "SetVoxelValue",      SetVoxelValue);
            lua_register(L, "GetVoxelValue",      GetVoxelValue);
            lua_register(L, "SetColumnValue",     GetVoxelValue);
            lua_register(L, "CompareColumnValue", CompareColumnValue);
            lua_register(L, "SetPlaneValue",      SetPlaneValue);
            lua_register(L, "ComparePlaneValue",  ComparePlaneValue);
        }
    }
    
    ///
    /// \brief RGB Lua Functions
    ///
    /// This namespace contains functions to be registered with the Lua state when the
    /// animation needs to use three colors.
    ///
    namespace RGB
    {
        int SetVoxelColor(lua_State *L)
        {
            int argc = lua_gettop(L);
            if (currAnim != NULL)
            {
                switch (argc)
                {
                    case 4:     // x, y, z, RGB
                        currAnim->SetVoxelColor(
                            (byte)lua_tointeger(L, 1),
                            (byte)lua_tointeger(L, 2),
                            (byte)lua_tointeger(L, 3),
                            (ulint)lua_tointeger(L, 4) );
                        break;
                    case 6:     // x, y, z, r, g, b
                        currAnim->SetVoxelColor(
                            (byte)lua_tointeger(L, 1),
                            (byte)lua_tointeger(L, 2),
                            (byte)lua_tointeger(L, 3),
                            (byte)lua_tointeger(L, 4),
                            (byte)lua_tointeger(L, 5),
                            (byte)lua_tointeger(L, 6) );
                        break;
                    default:
                        break;
                }
            }
            return 0;
        }
        
        int GetVoxelColor(lua_State *L)
        {
            int argc = lua_gettop(L);
            if (argc != 3 && argc != 4) return 0;
            // Set the mode based on the 4th argument (default to RGB_HEX).
            int mode = (argc == 4) ? lua_tointeger(L, 4) : -2;

            if (currAnim != NULL)
            {
                if (mode == -2)
                {
                    lua_pushinteger(L, currAnim->GetVoxelColor(
                        (byte)lua_tointeger(L, 1),
                        (byte)lua_tointeger(L, 2),
                        (byte)lua_tointeger(L, 3) ));
                    return 1;
                }
                else if (mode == -1)
                {
                    byte x = (byte)lua_tointeger(L, 1),
                         y = (byte)lua_tointeger(L, 2),
                         z = (byte)lua_tointeger(L, 3);
                    lua_pushinteger(L,
                        currAnim->cubeState[0]->GetVoxelState(x, y, z));
                    lua_pushinteger(L,
                        currAnim->cubeState[1]->GetVoxelState(x, y, z));
                    lua_pushinteger(L,
                        currAnim->cubeState[2]->GetVoxelState(x, y, z));
                    return 3;
                }
                else if ( mode == TC_COLOR_R ||
                          mode == TC_COLOR_G ||
                          mode == TC_COLOR_B )
                {
                    lua_pushinteger(L,
                        currAnim->cubeState[mode]->GetVoxelState(
                            (byte)lua_tointeger(L, 1),
                            (byte)lua_tointeger(L, 2),
                            (byte)lua_tointeger(L, 3) ));
                    return 1;
                }
            }
            return 0;
        }

        int SetColumnColor(lua_State *L)
        {
            int argc = lua_gettop(L);
            if (argc != 4 && argc != 6) return 0;
            if (currAnim != NULL)
            {
                if (argc == 4)
                {
                    currAnim->SetColumnColor(
                        (byte)lua_tointeger(L, 1),
                        (byte)lua_tointeger(L, 2),
                        (byte)lua_tointeger(L, 3),
                        (ulint)lua_tointeger(L, 4) );
                }
                else
                {
                    currAnim->SetColumnColor(
                        (byte)lua_tointeger(L, 1),
                        (byte)lua_tointeger(L, 2),
                        (byte)lua_tointeger(L, 3),
                        (byte)lua_tointeger(L, 4),
                        (byte)lua_tointeger(L, 5),
                        (byte)lua_tointeger(L, 6) );
                }
                return 1;
            }
            return 0;
        }

        int CompareColumnColor(lua_State *L)
        {
            int argc = lua_gettop(L);
            if (argc != 4 && argc != 6) return 0;
            if (currAnim != NULL)
            {
                if (argc == 4)
                {
                    lua_pushboolean(L, currAnim->CompareColumnColor(
                        (byte)lua_tointeger(L, 1),
                        (byte)lua_tointeger(L, 2),
                        (byte)lua_tointeger(L, 3),
                        (ulint)lua_tointeger(L, 4)));
                }
                else
                {
                    lua_pushboolean(L, currAnim->CompareColumnColor(
                        (byte)lua_tointeger(L, 1),
                        (byte)lua_tointeger(L, 2),
                        (byte)lua_tointeger(L, 3),
                        (byte)lua_tointeger(L, 4),
                        (byte)lua_tointeger(L, 5),
                        (byte)lua_tointeger(L, 6)));
                }
                return 1;
            }
            return 0;
        }

        int SetPlaneColor(lua_State *L)
        {
            int argc = lua_gettop(L);
            if (argc != 3 && argc != 5) return 0;
            if (currAnim != NULL)
            {
                if (argc == 3)
                {
                    currAnim->SetPlaneColor(
                        (byte)lua_tointeger(L, 1),
                        (byte)lua_tointeger(L, 2),
                        (ulint)lua_tointeger(L, 3) );
                }
                else
                {
                    currAnim->SetPlaneColor(
                        (byte)lua_tointeger(L, 1),
                        (byte)lua_tointeger(L, 2),
                        (byte)lua_tointeger(L, 3),
                        (byte)lua_tointeger(L, 4),
                        (byte)lua_tointeger(L, 5) );
                }
                return 1;
            }
            return 0;
        }

        int ComparePlaneColor(lua_State *L)
        {
            int argc = lua_gettop(L);
            if (argc != 3 && argc != 5) return 0;
            if (currAnim != NULL)
            {
                if (argc == 3)
                {
                    lua_pushboolean(L, currAnim->ComparePlaneColor(
                        (byte)lua_tointeger(L, 1),
                        (byte)lua_tointeger(L, 2),
                        (ulint)lua_tointeger(L, 3)));
                }
                else
                {
                    lua_pushboolean(L, currAnim->ComparePlaneColor(
                        (byte)lua_tointeger(L, 1),
                        (byte)lua_tointeger(L, 2),
                        (byte)lua_tointeger(L, 3),
                        (byte)lua_tointeger(L, 4),
                        (byte)lua_tointeger(L, 5)));
                }
                return 1;
            }
            return 0;
        }
        
        void RegisterCommands(lua_State *L)
        {
            lua_register(L, "SetVoxelColor",      SetVoxelColor);
            lua_register(L, "GetVoxelColor",      GetVoxelColor);
            lua_register(L, "SetColumnColor",     SetColumnColor);
            lua_register(L, "CompareColumnColor", CompareColumnColor);
            lua_register(L, "SetPlaneColor",      SetPlaneColor);
            lua_register(L, "ComparePlaneColor",  ComparePlaneColor);
        }
    }
}


///
/// \brief Lua Animation Loader
///
/// Loads the passed filename as a Lua animation, and validates the file.  This function
/// validates the Lua file, to ensure that the Lua file is syntactically correct, and all
/// required functions have been implemented.
///
/// \param fname A C-string containing the filename to be passed to luaL_loadfile.
/// \param argc  The number of arguments to pass to the animation when initializing.
/// \param argv  Pointer to each of the arguments. If there are no arguments, set to NULL.
///
/// \returns A pointer to a TCAnimLua object, casted to a TCAnim object.  If the Lua file
///          could not be loaded, NULL is returned.
///
TCAnim *LuaAnimLoader(char const *fname, int argc, int *argv)
{
    TCAnimLua *toReturn  = NULL;        // The TCAnimLua object to return (as a TCAnim).
    lua_State *pLuaState;               // Pointer to the current Lua state.
    TC_Lua_Functions::currAnim = NULL;  // Initialize the Lua animation pointer to NULL.
    std::string fpath = "animations/";
    fpath += fname;
    // First, we open the Lua state, and load the Lua libraries.
    pLuaState = lua_open();
    luaL_openlibs(pLuaState);
    // Next, we attempt to open the animation file from the passed filename.
    if (luaL_loadfile(pLuaState, fpath.c_str()))    // So, if Lua couldn't load the file...
    {
        // Try to load the filename with .lua appended to it.
        fpath += ".lua";
        if (luaL_loadfile(pLuaState, fpath.c_str()))
        {       
            // We couldn't open the file, so display an error, cleanup, and return.
            std::string errMsg = "Error - could not load file \"";
            errMsg += fname;
            errMsg += "\"!\n";
            errMsg += "Ensure that the file exists, and try again.";
            WriteOutput(errMsg);
            lua_close(pLuaState);
            return NULL;
        }
    }
    // Next, we see if Lua can parse the given file.
    if (lua_pcall(pLuaState, 0, 0, 0))
    {
        // If we couldn't parse the file, show an error in the console, and return.
        WriteOutput("Error - could not load animation. "
                    "Check the file for syntax errors and try again.");
        lua_close(pLuaState);
        return NULL;
    }
    // Now, we check the value of _setColors to see if the number of colors was set.
    lua_getglobal(pLuaState, "_setColors");
    if (!lua_toboolean(pLuaState, -1))
    {
        WriteOutput("Error - number of colors in animation is not set. "
                    "Ensure that you have called SetNumColors in your animation file.");
        lua_close(pLuaState);
        return NULL;
    }
    lua_pop(pLuaState, 1);  // We have to pop the value off of the Lua stack.
    // Next, we get the actual number of colors.
    lua_getglobal(pLuaState, "_numColors");
    int _numColors = lua_tointeger(pLuaState, -1);
    // So, if the number of colors was not set to 0, 1, or 3...
    if (_numColors != 0 && _numColors != 1 && _numColors != 3)
    {
        WriteOutput("Error - animation has unsupported number of colors. "
                    "Valid numbers of colors are 0, 1, and 3.");
        lua_close(pLuaState);
        return NULL;
    }
    lua_pop(pLuaState, 1);  // Again, we have to pop the value off of the Lua stack.
    // Now that we have the number of colors, we can register the appropriate Lua commands
    // (as well as the common commands) to the animation's Lua state.
    TC_Lua_Functions::Common::RegisterCommands(pLuaState);
    switch (_numColors)
    {
        case 0:
            TC_Lua_Functions::BW::RegisterCommands(pLuaState);
            break;
        case 1:
            TC_Lua_Functions::Greyscale::RegisterCommands(pLuaState);
            break;
        case 3:
            TC_Lua_Functions::RGB::RegisterCommands(pLuaState);
            break;
        default:
            break;
    }
    // Next, we initialize the object so that the registered functions are valid.  We also
    // delete the object if we need to quit (since lua_close is called in the destructor).
    toReturn = new TCAnimLua(cubeSize, _numColors, pLuaState);
    // We also store the pointer for use with the registered commands in TC_Lua_Functions.
    TC_Lua_Functions::currAnim = toReturn;
    // Now, we attempt to call the InitSize function (which is in animbase.lua).
    lua_getglobal(pLuaState, "_InitSize");
    if (!lua_isfunction(pLuaState, -1))     // If the function does not exist...
    {
        // We output an error to the console, close the Lua state, and return NULL.
        WriteOutput("Error - could not find the InitSize function. "
                    "Ensure that you have included animbase.lua in your animation file.");
        delete toReturn;
        TC_Lua_Functions::currAnim = NULL;
        return NULL;
    }
    // Next, we push each of the sizes onto the Lua stack, and call the InitSize function.
    lua_pushinteger(pLuaState, cubeSize[0]);
    lua_pushinteger(pLuaState, cubeSize[1]);
    lua_pushinteger(pLuaState, cubeSize[2]);
    lua_pcall(pLuaState, 3, 0, 0);
    // Now, we repeat the above steps, but with the Initialize function.
    lua_getglobal(pLuaState, "Initialize");
    if (!lua_isfunction(pLuaState, -1))     // If the function does not exist...
    {
        // We output an error to the console, close the Lua state, and return NULL.
        WriteOutput("Error - could not find the Initialize function. "
                    "Ensure that you have defined this function in your animation file.");
        delete toReturn;
        TC_Lua_Functions::currAnim = NULL;
        return NULL;
    }
    // Next, we push all of the arguments onto the stack, and call the function.
    for (int i = 0; i < argc; i++)
    {
        lua_pushinteger(pLuaState, argv[i]);
    }
    if (lua_pcall(pLuaState, argc, 1, 0))   // If there was too little/many arguments...
    {
        // We output an error to the console, close the Lua state, and return NULL.
        WriteOutput("Error - call to Initialize failed. "
                    "Check that you have passed the proper number of arguments.");
        delete toReturn;
        TC_Lua_Functions::currAnim = NULL;
        return NULL;
    }
    if (!lua_toboolean(pLuaState, -1))      // If Initialize returned false...
    {
        // We output an error to the console, close the Lua state, and return NULL.
        WriteOutput("Error - call to Initialize failed (returned false). See above for "
                    "additional information (if applicable).");
        delete toReturn;
        TC_Lua_Functions::currAnim = NULL;
        return NULL;
    }
    lua_pop(pLuaState, 1);  // We have to pop the return value off of the Lua stack.
    // Lastly, we need to check that the Update function is valid.
    lua_getglobal(pLuaState, "Update");
    if (!lua_isfunction(pLuaState, -1))     // If the function does not exist...
    {
        // We output an error to the console, close the Lua state, and return NULL.
        WriteOutput("Error - could not find the Update function. "
                    "Ensure that you have defined this function in your animation file.");
        delete toReturn;
        TC_Lua_Functions::currAnim = NULL;
        return NULL;
    }
    // Finally, we can return the TCAnimLua object.
    return toReturn;
}


///
/// \brief Constructor
///
/// Creates a TCAnim object from the given cubesize and Lua state.  This function assumes
/// that the passed Lua state was parsed properly, and has all required functions.  See
/// the \ref LuaAnimLoader function for details.
///
/// \param tccSize      Array holding the cube size (in voxels).
/// \param luaStateAnim Pointer to the initialized and valid Lua state.
///
/// \see LuaAnimLoader
///
TCAnimLua::TCAnimLua(byte tccSize[3], byte colors, lua_State *luaStateAnim)
    : TCAnim(tccSize, colors)
{
    pLuaState = luaStateAnim;
}


///
/// \brief Destructor
///
/// Closes the Lua state (all other dynamic memory is deallocated by the base destructor).
///
TCAnimLua::~TCAnimLua()
{
    lua_close(pLuaState);
}


///
/// \brief Update
///
/// Called on every tick, this function calls the Update function defined by the Lua
/// animation file.  This function assumes that the function exists and is valid.
///
void TCAnimLua::Update()
{
    lua_pcall(pLuaState, 0, 0, 0);
    lua_getglobal(pLuaState, "Update");
}


///
/// \brief Done Iteration
///
/// Increases the animation's iteration count by one.
///
/// \remarks This is a trivial method, provided only to allow incrementing the animation's
///          iteration count through Lua.  See the TC_Lua_Functions::DoneIteration
///          function for more details.
///
void TCAnimLua::DoneIteration()
{
    iterations++;
}
