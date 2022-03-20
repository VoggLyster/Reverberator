/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Divider.h"

//==============================================================================
/**
*/
typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

class ReverberatorAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    ReverberatorAudioProcessorEditor(ReverberatorAudioProcessor&, AudioProcessorValueTreeState&);
    ~ReverberatorAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    AudioProcessorValueTreeState& valueTreeState;
    ReverberatorAudioProcessor& audioProcessor;
    Viewport scrollableView;
    Component mainComponent;

    //Label delayMaxLengthLabel;
    //Slider delayMaxLengthSlider;
    //std::unique_ptr<SliderAttachment> delayMaxLengthAttachment;
    //Label delayMinLengthLabel;
    //Slider delayMinLengthSlider;
    //std::unique_ptr<SliderAttachment> delayMinLengthAttachment;

    Divider dividers[N_LINES];

    Label bLabels[N_LINES];
    Slider bSliders[N_LINES];
    std::unique_ptr<SliderAttachment> bAttachments[N_LINES];
    Label cLabels[N_LINES];
    Slider cSliders[N_LINES];
    std::unique_ptr<SliderAttachment> cAttachments[N_LINES];
    Label eqGainLabels[N_LINES][N_EQ];
    Slider eqGainSliders[N_LINES][N_EQ];
    std::unique_ptr<SliderAttachment> eqGainAttachments[N_LINES][N_EQ];

    //Label modFreqLabels[N_LINES];
    //Slider modFreqSliders[N_LINES];
    //std::unique_ptr<SliderAttachment> modFreqAttachments[N_LINES];
    //Label modDepthLabels[N_LINES];
    //Slider modDepthSliders[N_LINES];
    //std::unique_ptr<SliderAttachment> modDepthAttachments[N_LINES];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverberatorAudioProcessorEditor)
};
