/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utils.h"
//==============================================================================
MDAJX10SynthAudioProcessor::MDAJX10SynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
{
  /*  noiseParam = dynamic_cast<juce::AudioParameterFloat*>(
        apvts.getParameter(ParameterId::noise.getParamID()));*/

    castParameter(apvts, ParameterId::oscMix, oscMixParam);
    castParameter(apvts, ParameterId::oscTune, oscTuneParam);
    castParameter(apvts, ParameterId::oscFine, oscFineParam);
    castParameter(apvts, ParameterId::glideMode, glideModeParam);
    castParameter(apvts, ParameterId::glideRate, glideRateParam);
    castParameter(apvts, ParameterId::glideBend, glideBendParam);
    castParameter(apvts, ParameterId::filterFreq, filterFreqParam);
    castParameter(apvts, ParameterId::filterReso, filterResoParam);
    castParameter(apvts, ParameterId::filterEnv, filterEnvParam);
    castParameter(apvts, ParameterId::filterLFO, filterLFOParam);
    castParameter(apvts, ParameterId::filterVelocity, filterVelocityParam);
    castParameter(apvts, ParameterId::filterAttack, filterAttackParam);
    castParameter(apvts, ParameterId::filterDecay, filterDecayParam);
    castParameter(apvts, ParameterId::filterSustain, filterSustainParam);
    castParameter(apvts, ParameterId::filterRelease, filterReleaseParam);
    castParameter(apvts, ParameterId::envAttack, envAttackParam);
    castParameter(apvts, ParameterId::envDecay, envDecayParam);
    castParameter(apvts, ParameterId::envSustain, envSustainParam);
    castParameter(apvts, ParameterId::envRelease, envReleaseParam);
    castParameter(apvts, ParameterId::lfoRate, lfoRateParam);
    castParameter(apvts, ParameterId::vibrato, vibratoParam);
    castParameter(apvts, ParameterId::noise, noiseParam);
    castParameter(apvts, ParameterId::octave, octaveParam);
    castParameter(apvts, ParameterId::tuning, tuningParam);
    castParameter(apvts, ParameterId::outputLevel, outputLevelParam);
    castParameter(apvts, ParameterId::polyMode, polyModeParam);

}

MDAJX10SynthAudioProcessor::~MDAJX10SynthAudioProcessor()
{
}

//==============================================================================
const juce::String MDAJX10SynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MDAJX10SynthAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool MDAJX10SynthAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool MDAJX10SynthAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double MDAJX10SynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MDAJX10SynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int MDAJX10SynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MDAJX10SynthAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String MDAJX10SynthAudioProcessor::getProgramName(int index)
{
    return {};
}

void MDAJX10SynthAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void MDAJX10SynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth.allocateResources(sampleRate, samplesPerBlock);
    reset();
}

void MDAJX10SynthAudioProcessor::releaseResources()
{
    synth.deallocateResources();
}

void MDAJX10SynthAudioProcessor::reset()
{
    synth.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MDAJX10SynthAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void MDAJX10SynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    const juce::String& paramID = ParameterId::noise.getParamID();
    float noiseMix = noiseParam->get()/ 100.0f;
    noiseMix *= noiseMix;
    synth.noiseMix = noiseMix * 0.06f;

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any output channels that don't contain input data.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    splitBufferByEvents(buffer, midiMessages);
}

void MDAJX10SynthAudioProcessor::splitBufferByEvents(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    int bufferOffset = 0;

    // Loop through the MIDI messages, which are sorted by samplePosition.
    for (const auto metadata : midiMessages) {

        // Render the audio that happens before this event (if any).
        int samplesThisSegment = metadata.samplePosition - bufferOffset;
        if (samplesThisSegment > 0) {
            render(buffer, samplesThisSegment, bufferOffset);
            bufferOffset += samplesThisSegment;
        }

        // Handle the event. Ignore MIDI messages such as sysex.
        if (metadata.numBytes <= 3) {
            uint8_t data1 = (metadata.numBytes >= 2) ? metadata.data[1] : 0;
            uint8_t data2 = (metadata.numBytes == 3) ? metadata.data[2] : 0;
            handleMIDI(metadata.data[0], data1, data2);
        }
    }

    // Render the audio after the last MIDI event. If there were no
    // MIDI events at all, this renders the entire buffer.
    int samplesLastSegment = buffer.getNumSamples() - bufferOffset;
    if (samplesLastSegment > 0) {
        render(buffer, samplesLastSegment, bufferOffset);
    }

    midiMessages.clear();
}

void MDAJX10SynthAudioProcessor::handleMIDI(uint8_t data0, uint8_t data1, uint8_t data2)
{
    //    char s[16];
    //    snprintf(s, 16, "%02hhX %02hhX %02hhX", data0, data1, data2);
    //    DBG(s);

    synth.midiMessage(data0, data1, data2);
}

void MDAJX10SynthAudioProcessor::render(juce::AudioBuffer<float>& buffer, int sampleCount, int bufferOffset)
{
    float* outputBuffers[2] = { nullptr, nullptr };
    outputBuffers[0] = buffer.getWritePointer(0) + bufferOffset;
    if (getTotalNumOutputChannels() > 1) {
        outputBuffers[1] = buffer.getWritePointer(1) + bufferOffset;
    }

    synth.render(outputBuffers, sampleCount);
}

//==============================================================================
bool MDAJX10SynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MDAJX10SynthAudioProcessor::createEditor()
{
    auto editor = new juce::GenericAudioProcessorEditor(*this);
    editor->setSize(500, 1050);
    return editor;
}

//==============================================================================
void MDAJX10SynthAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MDAJX10SynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MDAJX10SynthAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout MDAJX10SynthAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        ParameterId::polyMode,
        "Polyphony",
        juce::StringArray{ "Mono", "Poly" },
        1));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterId::oscTune,
        "Osc Tune", //oscillator tuning
        juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
        -12.0f,     //default value
        juce::AudioParameterFloatAttributes().withLabel("cent") //expressed in semitones
        ));

    auto oscMixStringFromValue = [](float value, int)
    {
        char s[16] = { 0 };
        snprintf(s, 16, "%4.0f:%2.0f", 100.0 - 0.5f * value * value);
        return juce::String(s);
    };

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterId::oscMix,
        "OscMix",           
        juce::NormalisableRange<float>(0.0f, 100.0f),
        0.0f,
        juce::AudioParameterFloatAttributes()
        .withLabel("%")
        .withStringFromValueFunction(oscMixStringFromValue)
        ));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        ParameterId::glideMode,
        "Glide Mode",
        juce::StringArray{ "Off", "Legato", "Always" },  //choice box that offers three choices
    0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterId::glideRate,
        "Glide Rate",
        juce::NormalisableRange<float>(0.0f, 100.f, 1.0f),
        35.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));  //determines how it takes to glide from one note to the next

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterId::glideBend,
        "Glide Bend",
        juce::NormalisableRange<float>(-36.0f, 36.0f, 0.01f, 0.4f, true),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("semi")));  //lets you add an additional glide to any note that gets played between -36 and +36 semitones.

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterId::filterFreq,
        "Filter Freq",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterId::filterReso,
        "Filter Reso",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        15.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterId::filterEnv,
        "Filter Env",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f),
        50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterId::filterLFO,
        "Filter LFO",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    auto filterVelocityStringFromValue = [](float value, int)
    {
        if (value < -90.0f)
            return juce::String("OFF");
        else
            return juce::String(value);
    };

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterId::filterVelocity,
        "Velocity",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 1.0f),
        0.0f,
        juce::AudioParameterFloatAttributes()
        .withLabel("%")
        .withStringFromValueFunction(filterVelocityStringFromValue)));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterId::filterAttack,
        "Filter Attack",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterId::filterDecay,
        "Filter Decay",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        30.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterId::filterSustain,
        "Filter Sustain",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterId::filterRelease,
        "Filter Release",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        25.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterId::envAttack,
        "Env Attack",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterId::envDecay,
        "Env Decay",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterId::envSustain,
        "Env Sustain",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterId::envRelease,
        "Env Release",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        30.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    auto lfoRateStringFromValue = [](float value, int)
    {
        float lfoHz = std::exp(7.0f * value - 4.0f);
        return juce::String(lfoHz, 3);
    };

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterId::lfoRate,
        "LFO Rate",
        juce::NormalisableRange<float>(),
        0.81f,
        juce::AudioParameterFloatAttributes()
        .withLabel("Hz")
        .withStringFromValueFunction(lfoRateStringFromValue)));

    auto vibratoStringFromValue = [](float value, int)
    {
        if (value < 0.0f)
            return "PWM" + juce::String(-value, 1);
        else
            return juce::String(value, 1);
    };

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterId::vibrato,
        "Vibrato",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes()
        .withLabel("%")
        .withStringFromValueFunction(vibratoStringFromValue)));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterId::noise,
        "Noise",
        juce::NormalisableRange <float>(0.0f, 100.0f, 1.0f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterId::octave,
        "Octave",
        juce::NormalisableRange <float>(-2.0f, 2.0f, 1.0f),
        0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterId::tuning,
        "Tuning",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("cent")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterId::outputLevel,
        "Output Level",
        juce::NormalisableRange<float>(-24.0f, 6.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));    ///Volume parameter

    return layout;
}