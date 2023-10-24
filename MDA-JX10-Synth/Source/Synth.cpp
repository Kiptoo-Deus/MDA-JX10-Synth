/*
  ==============================================================================

    Synth.cpp
    Created: 24 Oct 2023 11:07:33am
    Author:  JOEL

  ==============================================================================
*/

#include "Synth.h"


    Synth::Synth() 
    {
    sampleRate = 44100.0f;
    }
    void Synth::allocateResources(double sampleRate_, int /*samplesPerBlock*/)
    {
        sampleRate = static_cast<float>(sampleRate);
    }
    void Synth::deallocateResources()
    {

    }
    void Synth::reset()
    {
                     
    }
    void Synth::render(float** outputBuffers, int sampleCount)
    {

    }
    void Synth::MidiMessage(uint8_t data0, uint8_t data1, uint8_t data2)
    {

    }
