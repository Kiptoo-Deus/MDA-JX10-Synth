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

        // Convert to a signed value.
        int temp = int(noiseSeed >> 7) - 16777216;

        // Convert to a floating-point number between -1.0 and 1.0.
        return float(temp) / 16777216.0f;
    }

private:
    unsigned int noiseSeed;
};