/*
  ==============================================================================

    Voice.h
    Created: 24 Oct 2023 11:07:58am
    Author:  JOEL

  ==============================================================================
*/

#pragma once
#include "Oscillator.h"

struct Voice
{
    int note;
    Oscillator osc;

    void reset()
    {
        note = 0;
        osc.reset();
    }
    float render()
    {
        return osc.nextSample();
    }
};