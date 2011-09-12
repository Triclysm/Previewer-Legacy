/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                          Lua Animation Object Header File                           *
 *                                      TRICLYSM                                       *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *  This file contains the definition of the TCAnimLua class as defined by the         *
 *  TCAnimLua.cpp source file. This class inherets the base animation class (TCAnim),  *
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
/// \file  TCAnim.h
/// \brief This file contains the definition of the TCAnim class as implemented by the
///        TCAnim.cpp source file.
///

#ifndef TC_ANIM_LUA_
#define TC_ANIM_LUA_

#include <lua.hpp>          // The Lua C++ header file.
#include "TCAnim.h"         // Base animation class to override.


// Function to validate and load a Lua file as a TCAnim object.
TCAnim *LuaAnimLoader(char const *fname, int argc, int *argv);

///
/// \brief Triclysm Animation Lua Object
///
/// This class represents a Triclysm Animation created with a Lua script.
///
/// \remarks This class inherets the base TCAnim class.
///
class TCAnimLua : public TCAnim
{
  public:
    TCAnimLua(byte tccSize[3], lua_State *luaStateAnim);  // Constructor.
    ~TCAnimLua();                                         // Destructor.
    void DoneIteration();                                 // Increments iteration count.
  private:
    void Update();                                        // Calls the Lua update function.
    lua_State *pLuaState;   ///< Internal pointer to the animation's Lua state.
};

#endif
