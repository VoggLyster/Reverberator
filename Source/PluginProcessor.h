/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Reverb.h"

//#define N_LINES 16

//==============================================================================
/**
*/
class ReverberatorAudioProcessor  : public juce::AudioProcessor, public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    ReverberatorAudioProcessor();
    ~ReverberatorAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void parameterChanged(const String& parameterID, float newValue) override;

private:
    AudioProcessorValueTreeState parameters;
    std::atomic<float>* cParameters[N_LINES];
    std::atomic<float>* bParameters[N_LINES];
    //std::atomic<float>* MParameters[N_LINES];
    std::atomic<float>* filterCoeffParameters[N_LINES][5];
    int M[16] = { 2927, 2593, 2273, 3697, 1877, 3877, 2477, 3461, 1609, 3779, 3541, 4259, 1669, 3539, 3637, 4013, };

    std::unique_ptr<ReverbProcessor> reverbProcessor;
    
    bool stereo = false;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverberatorAudioProcessor)
};
