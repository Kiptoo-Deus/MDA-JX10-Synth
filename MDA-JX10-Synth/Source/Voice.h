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
    float saw;


    void reset()
    {
        note = 0;
        osc.reset();
        saw = 0.0f;
    }
    float render()
    {
        float sample = osc.nextSample();
        saw = saw * 0.997f + sample;
        return saw;
    }
    

};