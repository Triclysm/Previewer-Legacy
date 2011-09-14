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
    TCAnim(byte cubeSize);                          // Default, cube constructor.
    TCAnim(byte sizeX, byte sizeY, byte sizeZ);     // Variable dimension constructor.
    TCAnim(byte tccSize[3]);                        // Same as above, but with an array.
    virtual ~TCAnim();                              // Destructor.
    
    void         Tick();            // Increments tick counter & calls the Update method.
    unsigned int GetIterations();   // Gets # of times animation has run.
    unsigned int GetTicks();        // Gets # of ticks (animation updates).

    /// \brief TCCube object holding the current state of the animation.
    ///
    /// Dynamically allocated when the TCAnim object constructor is called.
    TCCube *cubeState;
protected:
    /// \brief Update
    ///
    /// Advances the cube state by one step.  This method is defined by any animations
    /// which inheret this base class.
    virtual void Update(){}
    byte         sc[3];         ///< Number of cube voxels in each dimension.
    unsigned int iterations;    ///< Number of times the animation has run.
private:
    unsigned int ticks;         ///< Number of times the animation's state was updated.
};


#endif
