/*
  ==============================================================================

    Synth.cpp
    Created: 24 Oct 2023 11:07:33am
    Author:  JOEL

  ==============================================================================
*/

#include "Synth.h"
#include "Utils.h"

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
        voice.reset();
        noiseGen.reset();
    }
    void Synth::render(float** outputBuffers, int sampleCount)
    {
        float* outputBufferLeft = outputBuffers[0];
        float* outputputBufferRight = outputBuffers[1];

        for (int sample = 0; sample < sampleCount, ++sample;)    //loop through the samples in the buffer one by one..samplecount is the number of samples to be rendered
        {
            float noise = noiseGen.nextValue();//capture output from the noise generator

            float output = 0.0f;
            if (voice.note > 0) {
                output = voice.render();
            }
            outputBufferLeft[sample] = output;
            if (outputBufferLeft != nullptr) {
                outputputBufferRight[sample] = output;
            }

        }
        protectYourEars(outputBufferLeft, sampleCount);
        protectYourEars(outputputBufferRight, sampleCount);
    }
    void Synth::noteOn(int note, int velocity)//registers the note number and velocity of the most recently played key
    {
        voice.note = note;
        
        voice.osc.amplitude = (velocity / 127.0f) * 0.5f;
        voice.osc.freq = 261.63f;   //this is the pitch of C on a piano
        voice.osc.sampleRate = sampleRate;
        voice.osc.phaseOffset = 0.0f;
        voice.osc.reset();
    }

    void Synth::noteOff(int note)
    {
        if (voice.note == note) {
            voice.note = 0;

        }
    }

    void Synth::MidiMessage(uint8_t data0, uint8_t data1, uint8_t data2)
    {
        switch (data0 & 0xF0) {
            //Note off
        case 0x80:
            noteOff(data1 & 0x7F);
            break;
            //Note on
        case 0x90:
        {
            uint8_t note = data1 & 0x7F;
            uint8_t velo = data2 & 0x7F;
            if (velo > 0) {
                noteOn(note, velo);
            }
            else
            {
                noteOff(note);
            }

            break;
        }
        }
    }
