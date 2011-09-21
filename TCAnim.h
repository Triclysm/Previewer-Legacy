/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                            Animation Object Header File                             *
 *                                      TRICLYSM                                       *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *  This file contains the definition of the TCAnim class as implemented by the        *
 *  TCAnim.cpp sourcefile.  This class is the base for any cube animation objects,     *
 *  and should be inhereted for implementing any animations.  This class also uses     *
 *  the TCCube class to hold the animation's current cube state.                       *
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

#pragma once
#ifndef TC_ANIM_
#define TC_ANIM_

#include "TCCube.h"

// Color Definitions
#define TC_COLOR_R 0                ///< Specifies the red color.
#define TC_COLOR_G 1                ///< Specifies the green color.
#define TC_COLOR_B 2                ///< Specifies the blue color.

typedef unsigned long int ulint;    ///< Used to store 24-bit color values.  The long 
                                    ///  keyword is used to specify at least 32-bits.


///
/// \brief Triclysm Animation Base Object
///
/// This class is the base for any cube animation objects, and is used in conjunction
/// with the TCAnimHandler object class.
///
/// \remarks This class also uses a TCCube object to hold/provide the animation's state.
///
class TCAnim    // Base Animation Class
{
public:
    TCAnim(byte cubeSize, byte colors = 0);     // Default, cube constructor.
    TCAnim(byte sizeX, byte sizeY,
           byte sizeZ, byte colors = 0);        // Variable size constructor.
    TCAnim(byte tccSize[3], byte colors = 0);   // Same as above, with array.
    virtual ~TCAnim();                          // Destructor.
    
    void         Tick();            // Increments tick counter & calls the Update method.
    unsigned int GetIterations();   // Gets # of times animation has run.
    unsigned int GetTicks();        // Gets # of ticks (animation updates).
    byte         GetNumColors();    // Returns the number of colors in the animation.

    // Voxel color setting functions:
    void  SetVoxelColor(byte x, byte y, byte z, byte grey);
    void  SetVoxelColor(byte x, byte y, byte z, byte r, byte g, byte b);
    void  SetVoxelColor(byte x, byte y, byte z, ulint rgbColorValue);
    ulint GetVoxelColor(byte x, byte y, byte z);
    
    void SetColumnColor(byte axis, byte dim1, byte dim2, byte grey);
    void SetColumnColor(byte axis, byte dim1, byte dim2, byte r, byte g, byte b);
    void SetColumnColor(byte axis, byte dim1, byte dim2, ulint rgbColorValue);
    bool CompareColumnColor(byte axis, byte dim1, byte dim2, byte grey);
    bool CompareColumnColor(byte axis, byte dim1, byte dim2, byte r, byte g, byte b);
    bool CompareColumnColor(byte axis, byte dim1, byte dim2, ulint rgbColorValue);
    
    void SetPlaneColor(byte plane, byte offset, byte grey);
    void SetPlaneColor(byte plane, byte offset, byte r, byte g, byte b);
    void SetPlaneColor(byte plane, byte offset, ulint rgbColorValue);
    bool ComparePlaneColor(byte plane, byte offset, byte grey);
    bool ComparePlaneColor(byte plane, byte offset, byte r, byte g, byte b);
    bool ComparePlaneColor(byte plane, byte offset, ulint rgbColorValue);
    
    void Shift(byte plane, sbyte offset);

    /// \brief TCCube object holding the current state of the animation.
    ///
    /// Dynamically allocated when the TCAnim object constructor is called.
    TCCube **cubeState;
protected:
    /// \brief Update
    ///
    /// Advances the cube state by one step.  This method is defined by any animations
    /// which inheret this base class.
    virtual void Update(){}
    byte         sc[3],         ///< Number of cube voxels in each dimension.
                 numColors;     ///< Number of colors in the current animation.
    unsigned int iterations;    ///< Number of times the animation has run.
private:
    unsigned int ticks;         ///< Number of times the animation's state was updated.
};


#endif
