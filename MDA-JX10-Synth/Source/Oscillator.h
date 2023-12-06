/*
  ==============================================================================

    Oscillator.h
    Created: 1 Nov 2023 11:47:52am
    Author:  JOEL

  ==============================================================================
*/

#pragma once
const float TWO_PI = 6.2831853071795864f;
class Oscillator
{
public:
    float amplitude;
    float inc;
    float phase;
    float phaseBL;
    float freq;
    float sampleRate;

    void reset()
    {
        phase = 0.0F;
        phaseBL = -0.5f;
    }

    float nextBandlimitedSample()
    {
        phaseBL += inc;

        if (phase >= 1.0f) {
            phase -= 1.0f;
        }
        float output = 0.0f;
        float nyquist = sampleRate / 2.0F;
        float h = freq;
        float i = 1.0f;
        float m = 0.6366197724f; //this is 2/pi

        while (h<nyquist)
        {
            output += m * std::sin(TWO_PI * phaseBL * i) / i;
            h += freq;
            i += 1.0f;
            m = -m;
         }

        return output;
    }

        float nextSample()
    {
        phase += inc;
        if (phase >= 1.0f) {
            phase -= 1.0f;
        }
        float aliased = 2.0f * phase - 1.0f;

        return amplitude * (aliased - nextBandlimitedSample());
    }
};