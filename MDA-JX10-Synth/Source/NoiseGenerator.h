/*
  ==============================================================================

    NoiseGenerator.h
    Created: 25 Oct 2023 3:32:09pm
    Author:  JOEL

  ==============================================================================
*/

#pragma once

class NoiseGenerator
{
public:
    void reset()
    {
        noiseSeed = 22222;

    }
    float nextValue()
    {
        //generates a random pseudorandom number
        noiseSeed = noiseSeed * 196314165 + 907633515;

        //convert the integer to a float, to get a number between 2 and 4
        unsigned int r = (noiseSeed & 0x7FFFFF) + 0x40000000;
        float noise = *(float*)&r;
        //subtract 3 to get the float into the range [-1,1]
        return noise - 3.0f;
    }
private: 
    unsigned int noiseSeed;
};