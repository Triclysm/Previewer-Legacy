/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                            Format Conversion Source Code                            *
 *                                      TRICLYSM                                       *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *  This file contains various conversion functions for switching between data types,  *
 *  but mostly to and from strings.                                                    *
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
/// \file  format_conversion.cpp
/// \brief This file contains all format conversion functions, to assist switching between
///        various data types.
///

#include <string>
#include <cctype>
#include <sstream>
#include "main.h"
#include "events.h"

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


///
/// \brief String To Key Symbol
///
/// Attempts to convert the passed string into a SDL Key Symbol (SDLKey).
///
/// \param toConvert The string representing the key symbol.
/// \param result    The variable to store the result in (if applicable).
///
/// \returns True if the conversion was successful, false otherwise.
/// \remarks If this function returns false, the value of the result is undefined, and
///          should not be used.
///
bool StringToKeySym(std::string const& toConvert, SDLKey &result)
{
    std::string toConv(toConvert);
    StringToLowercase(toConv);
    if (toConv.length() == 1)   // If we have a single character...
    {
        if (toConv[0] >= 'a' && toConv[0] <= 'z')
        {
            result = (SDLKey)(SDLK_a + toConv[0] - 'a');
            return true;
        }
        else if (toConv[0] >= '0' && toConv[0] <= '9')
        {
            result = (SDLKey)(SDLK_0 + toConv[0] - '0');
            return true;
        }
    }
    else                        // Else, if we have a word...
    {
        if (toConv == "space")
        {
            result = SDLK_SPACE;
            return true;
        }
    }
    // If we get here, no acceptable conversion was found, so we return false.
    return false;
}


///
<<<<<<< HEAD
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
bool StringToIp(std::string const& toConvert, Uint32 &result)
{
    // "1.1.1.1"         =  7 Characters Minimum
    // "111.111.111.111" = 15 Characters Maximum
    if (toConvert.length() < 7 || toConvert.length() > 15)
        return false;

    std::string ipStr(toConvert);
    Uint32      ipRes  = 0;
    Uint32      tmpRes = 0;

    std::stringstream *ssToConv;

    // Now, split string up by occurance of .'s
    for (int i = 0; i < 3; i++)
    {
        // Each '.' should be at position 1, 2, or 3.
        // If not, the string is invalid.
        std::string::size_type ppos = ipStr.find('.');
        if (ppos == std::string::npos || ppos < 1 || ppos > 3)
            return false;

        // Now we can get the value.
        ssToConv = new std::stringstream(ipStr.substr(0, ppos));
        if (!((*ssToConv) >> tmpRes) || tmpRes < 0 || tmpRes > 255)
        {
            delete ssToConv;
            return false;
        }
        delete ssToConv;

        // Finally, update ipStr, and add the byte in the proper position.
        ipStr  =  ipStr.substr(ppos+1);
        ipRes  += (Uint32)(tmpRes << (i*8));
        tmpRes = 0;
    }
    // If we got here, all is well, so convert the remaining value.
    ssToConv = new std::stringstream(ipStr);
    if (!((*ssToConv) >> tmpRes) || tmpRes < 0 || tmpRes > 255)
    {
        delete ssToConv;
        return false;
    }
    delete ssToConv;
    ipRes += (Uint32)(tmpRes << 24);

    result = ipRes;
    return true;
}

bool StringToPort(std::string const& toConvert, Uint16 &result)
{
    // Convert string to int.
    Uint16 tmpPort;
    int    portNum;
    if (!StringToInt(toConvert, portNum) || portNum < 0 || portNum > 65535)
    {
        return false;
    }
    tmpPort = (Uint16)portNum;
    // Finally, swap bytes and store in result.
    result =  (tmpPort & 0x00FF) << 8;
    result |= (tmpPort & 0xFF00) >> 8;
    return true;
}


Uint16 PortToInt(Uint16 const& toConvert)
{
    Uint16 result;
    result =  (toConvert & 0x00FF) << 8;
    result |= (toConvert & 0xFF00) >> 8;
    return result;
}

///
/// \brief Ip To String
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
bool IpToString(Uint32 const& toConvert, std::string &result)
{
    Uint8 addrBytes[4];
    std::stringstream outVal("");
    Uint32 bitMask = 0x000000FF;
    for (int i = 0; i < 4; i++)
    {
        Uint32 tmpVal = toConvert & bitMask;
        tmpVal = tmpVal >> (i*8);
        addrBytes[i] = (Uint8)tmpVal;
        bitMask = bitMask << 8;
    }
    
    outVal << (int)addrBytes[0] << '.'
           << (int)addrBytes[1] << '.'
           << (int)addrBytes[2] << '.'
           << (int)addrBytes[3];

    result = outVal.str();
    return true;
}


///
/// \brief Port To String
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
bool PortToString(Uint16 const& toConvert, std::string &result)
{
    Uint16 portNum;
    std::stringstream outVal("");
    // Swap byte order, convert to int & print.
    portNum =  (toConvert & 0x00FF) << 8;
    portNum |= (toConvert & 0xFF00) >> 8;
    
    outVal << (int)portNum;
    result = outVal.str();
    return true;

}


///
=======
>>>>>>> master
/// \brief Key Symbol To String
///
/// Attempts to convert the passed SDL Key Symbol (SDLKey) into a string.
///
/// \param toConvert The SDL key symbol.
/// \param result    The variable to store the result in (if applicable).
///
/// \returns True if the conversion was successful, false otherwise.
/// \remarks If this function returns false, the value of the result is undefined, and
///          should not be used.
///
bool KeySymToString(SDLKey const& toConvert, std::string &result)
{
    if (toConvert >= SDLK_0 && toConvert <= SDLK_9)
    {
        result = "";
        result += (char)((toConvert - SDLK_0) + '0');
    }
    else if (toConvert >= SDLK_a && toConvert <= SDLK_z)
    {
        result = "";
        result += (char)((toConvert - SDLK_a) + 'A');
    }
    else if (toConvert == SDLK_SPACE)
    {
        result = "Space";
    }
    else    // Else, if no acceptable conversion could be found...
    {
        return false;
    }
    // If we get to this point, an acceptable conversion was found, so return true.
    return true;
}


///
/// \brief KeyBind To String
///
/// Attempts to convert the passed pointer to a KeyBind object into a string representing
/// the key (e.g. "Alt+Shift+S).
///
/// \param toConvert Pointer to a KeyBind object.
/// \param result    The string to store the result in (if applicable).
///
/// \returns True if the conversion was successful, false otherwise.
/// \remarks If this function returns false, the value of the result is undefined, and
///          should not be used.
///
bool KeyBindToString(KeyBind *toConvert, std::string &result)
{
    if (toConvert != NULL)
    {
        std::stringstream ssResult;
        bool addPlus = false;
        if (toConvert->mCtrl)
        {
            ssResult << "Ctrl";
            addPlus = true;
        }
        if (toConvert->mAlt)
        {
            if (addPlus)
            {
                ssResult << "+";
            }
            else
            {
                addPlus = true;
            }
            ssResult << "Alt";
        }
        if (toConvert->mShift)
        {
            if (addPlus)
            {
                ssResult << "+";
            }
            else
            {
                addPlus = true;
            }
            ssResult << "Shift";
        }
        if (addPlus)
        {
            ssResult << "+";
        }
        std::string strKey;
        if (KeySymToString(toConvert->ksym, strKey))
        {
            ssResult << strKey;
            result = ssResult.str();
            return true;
        }
    }
    // If we get here, no acceptable conversion was found, so we return false.
    return false;
}
