/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                            Format Conversion Header File                            *
 *                                      TRICLYSM                                       *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *  This file contains the definitions of the various conversion functions defined in  *
 *  format_conversion.cpp.  These functions are used for converting between data       *
 *  types (mostly to and from strings).                                                *
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
/// \file  format_conversion.h
/// \brief This file contains the definitions of all format conversion functions, which
///        assist switching between various data types.
///


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                               PREPROCESSOR DIRECTIVES                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once
#ifndef TC_FORMAT_CONVERSION_
#define TC_FORMAT_CONVERSION_

#include <string>
#include <cctype>
#include <sstream>
#include "events.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                 FUNCTION PROTOTYPES                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// String modification functions:
void StringRemoveSpaces(std::string &toRemove, char spaceChar = ' ');
void StringToLowercase(std::string &toLower);
void StringToUppercase(std::string &toUpper);

// String conversion functions:
bool StringToInt(std::string const& toConvert, int &result);
bool StringToBool(std::string const& toConvert, bool &result);
bool StringToConst(std::string const& toConvert, int &result);
bool StringToKeySym(std::string const& toConvert, SDLKey &result);

// Other conversion functions:
bool KeySymToString(SDLKey const& toConvert, std::string &result);
bool KeyBindToString(KeyBind *toConvert, std::string &result);

#endif
