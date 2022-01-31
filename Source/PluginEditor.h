/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

class ReverberatorAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ReverberatorAudioProcessorEditor (ReverberatorAudioProcessor&, AudioProcessorValueTreeState&);
    ~ReverberatorAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    AudioProcessorValueTreeState& valueTreeState;
    ReverberatorAudioProcessor& audioProcessor;

    Label bLabels[N_LINES];
    Slider bSliders[N_LINES];
    std::unique_ptr<SliderAttachment> bAttachments[N_LINES];
    Label cLabels[N_LINES];
    Slider cSliders[N_LINES];
    std::unique_ptr<SliderAttachment> cAttachments[N_LINES];
    Label modFreqLabels[N_LINES];
    Slider modFreqSliders[N_LINES];
    std::unique_ptr<SliderAttachment> modFreqAttachments[N_LINES];
    Label coeffLabels[N_LINES][5];
    Slider coeffSliders[N_LINES][5];
    std::unique_ptr<SliderAttachment> coeffAttachments[N_LINES][5];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverberatorAudioProcessorEditor)
};
