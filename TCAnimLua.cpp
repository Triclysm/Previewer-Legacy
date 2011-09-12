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
#include "TCAnimLua.h"


///
/// \brief Triclysm Lua Functions
///
/// A namespace containing all of the functions to be exposed to Lua animations.  These
/// functions can be registered to a given Lua state by calling the RegisterLuaCommands
/// function in this namespace.  These functions should only be called by the Lua state.
///
/// \remarks Some of these functions make references to the current animation's cube state.
///
namespace TC_Lua_Functions
{
    TCAnimLua *currAnim;    // Pointer to the currently registered TCAnimLua object.

    int SetVoxelState(lua_State *L)
    {
        int argc = lua_gettop(L);
        if (currAnim != NULL && argc == 4)
        {
            currAnim->cubeState->SetVoxelState(
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
            lua_pushboolean(L, currAnim->cubeState->GetVoxelState(
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
            currAnim->cubeState->SetColumnState(
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
            lua_pushboolean(L, currAnim->cubeState->GetColumnState(
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
            currAnim->cubeState->SetPlaneState(
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
            lua_pushboolean(L, currAnim->cubeState->GetPlaneState(
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

    int Shift(lua_State *L)
    {
        int argc = lua_gettop(L);
        if (currAnim != NULL && argc == 3)
        {
            currAnim->cubeState->Shift(
                (byte)lua_tointeger(L, 1), 
                (byte)lua_tointeger(L, 2),
                ((lua_toboolean(L, 3)) ? true : false) );
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
    
    ///
    /// \brief Register Lua Commands
    ///
    /// Registers Lua function bindings for all of the functions present in the
    /// TC_Lua_Functions namespace.
    ///
    /// \param L The Lua state to register the functions to.
    ///
    void RegisterLuaCommands(lua_State *L)
    {
        lua_register(L, "SetVoxelState",  SetVoxelState);
        lua_register(L, "GetVoxelState",  GetVoxelState);
        lua_register(L, "SetColumnState", SetColumnState);
        lua_register(L, "GetColumnState", GetColumnState);
        lua_register(L, "SetPlaneState",  SetPlaneState);
        lua_register(L, "GetPlaneState",  GetPlaneState);
        lua_register(L, "Shift",          Shift);
        lua_register(L, "DoneIteration",  DoneIteration);
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
    if (luaL_loadfile(pLuaState, fpath.c_str()))
    {
        // If we couldn't open the file, show an error in the console, cleanup, and return.
        std::string errMsg = "Error - could not load file \"";
        errMsg += fname;
        errMsg += "\"!";
        WriteOutput(errMsg);
        lua_close(pLuaState);
        return NULL;
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
    // We can also register the Lua functions to the Lua state.
    TC_Lua_Functions::RegisterLuaCommands(pLuaState);
    // Next, we initialize the object so that the registered functions are valid.  We also
    // delete the object if we need to quit (since lua_close is called in the destructor).
    toReturn = new TCAnimLua(cubeSize, pLuaState);
    // We also store the pointer for use with the registered commands in TC_Lua_Functions.
    TC_Lua_Functions::currAnim = toReturn;
    // Now, we attempt to call the InitSize function (which is in animbase.lua).
    lua_getglobal(pLuaState, "InitSize");
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
        WriteOutput("Error - call to Initialize failed (returned false). "
                    "Ensure that the values of the arguments are correct.");
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
TCAnimLua::TCAnimLua(byte tccSize[3], lua_State *luaStateAnim) : TCAnim(tccSize)
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
