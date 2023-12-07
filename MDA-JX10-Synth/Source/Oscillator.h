/*
  ==============================================================================

    Oscillator.h
    Created: 1 Nov 2023 11:47:52am
    Author:  JOEL

  ==============================================================================
*/

#pragma once
#include <math.h>

const float PI_OVER_4 = 0.7853981633974483f;
const float PI = 3.1415926535897932f;
const float TWO_PI = 6.2831853071795864f;

class Oscillator
{
public:
    float period = 0.0f;
    float amplitude = 1.0f;

    void reset()
    {
        inc = 0.0f;
        phase = 0.0f;
     }
    float nextSample()
    {
       
    }
private:
    float phase;
    float phaseMax;
    float inc;
};