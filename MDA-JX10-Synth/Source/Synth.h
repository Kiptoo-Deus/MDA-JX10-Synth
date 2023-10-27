/*
  ==============================================================================

    Synth.h
    Created: 24 Oct 2023 11:07:33am
    Author:  JOEL

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "Voice.h"
#include "NoiseGenerator.h"

class Synth
{
public:
    Synth();

    void allocateResources(double sampleRate, int samplesPerBlock);
    void deallocateResources();
    void reset();
    void render(float** outputBuffers, int sampleCount);
    void MidiMessage(uint8_t data0, uint8_t data1, uint8_t data2);
private:
    float sampleRate;
    Voice voice;
    void noteOn(int note, int velocity);
    void noteOff(int note);
    NoiseGenerator noiseGen;

};