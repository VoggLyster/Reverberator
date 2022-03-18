/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ReverberatorAudioProcessorEditor::ReverberatorAudioProcessorEditor (ReverberatorAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState (vts)
{
    String name = "";
    for (int i = 0; i < N_LINES; i++) 
    {
        mainComponent.addAndMakeVisible(dividers[i]);
        name = "c" + String(i) + "_gain";
        cLabels[i].setText(name, juce::dontSendNotification);
        mainComponent.addAndMakeVisible(cLabels[i]);
        mainComponent.addAndMakeVisible(cSliders[i]);
        cAttachments[i].reset(new SliderAttachment(valueTreeState, name, cSliders[i]));
        for (int j = 0; j < N_EQ; j++) {
            name = "eq_" + String(i) + "_gain_" + String(j);
            eqGainLabels[i][j].setText(name, juce::dontSendNotification);
            mainComponent.addAndMakeVisible(eqGainLabels[i][j]);
            mainComponent.addAndMakeVisible(eqGainSliders[i][j]);
            eqGainAttachments[i][j].reset(new SliderAttachment(valueTreeState, name, eqGainSliders[i][j]));
        }
        /*name = "mod" + String(i) + "_freq";
        modFreqLabels[i].setText(name, juce::dontSendNotification);
        mainComponent.addAndMakeVisible(modFreqLabels[i]);
        mainComponent.addAndMakeVisible(modFreqSliders[i]);
        modFreqAttachments[i].reset(new SliderAttachment(valueTreeState, name, modFreqSliders[i]));
        name = "mod" + String(i) + "_depth";
        modDepthLabels[i].setText(name, juce::dontSendNotification);
        mainComponent.addAndMakeVisible(modDepthLabels[i]);
        mainComponent.addAndMakeVisible(modDepthSliders[i]);
        modDepthAttachments[i].reset(new SliderAttachment(valueTreeState, name, modDepthSliders[i]));*/
    }

    //delayMaxLengthLabel.setText("delay_length_max", juce::dontSendNotification);
    //mainComponent.addAndMakeVisible(delayMaxLengthLabel);
    //mainComponent.addAndMakeVisible(delayMaxLengthSlider);
    //delayMaxLengthAttachment.reset(new SliderAttachment(valueTreeState, "delay_length_max", delayMaxLengthSlider));
    //delayMinLengthLabel.setText("delay_length_min", juce::dontSendNotification);
    //mainComponent.addAndMakeVisible(delayMinLengthLabel);
    //mainComponent.addAndMakeVisible(delayMinLengthSlider);
    //delayMinLengthAttachment.reset(new SliderAttachment(valueTreeState, "delay_length_min", delayMinLengthSlider));

    setSize (400, 1200);
    scrollableView.setViewedComponent(&mainComponent, false);
    addAndMakeVisible(scrollableView);
    scrollableView.setScrollBarsShown(true, true);
}

ReverberatorAudioProcessorEditor::~ReverberatorAudioProcessorEditor()
{
}

//==============================================================================
void ReverberatorAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (60.0f);
}

void ReverberatorAudioProcessorEditor::resized()
{
    scrollableView.setBoundsRelative(0, 0, 1, 1);
    int componentSize = 425 * N_LINES;
    mainComponent.setBounds(0, 0, getWidth(), componentSize);

    int size = 20;
    int height = 0;
    int c = 0;

    //delayMaxLengthLabel.setBounds(5, height, 100, size);
    //delayMaxLengthSlider.setBounds(105, height, 185, size);
    //height += 25;

    //delayMinLengthLabel.setBounds(5, height, 100, size);
    //delayMinLengthSlider.setBounds(105, height, 185, size);
    //height += 25;

    for (int i = 0; i < N_LINES; i++) {
        dividers[i].setBounds(5, height, 100, size);
        dividers[i].setBounds(105, height, 185, size);
        height += 25;
        cLabels[i].setBounds(5, height, 100, size);
        cSliders[i].setBounds(105, height, 185, size);
        height += 25;
        for (int j = 0; j < N_EQ; j++) {
            eqGainLabels[i][j].setBounds(5, height, 100, size);
            eqGainSliders[i][j].setBounds(105, height, 185, size);
            height += 25;
        }
        //modFreqLabels[i].setBounds(5, height, 100, size);
        //modFreqSliders[i].setBounds(105, height, 185, size);
        //height += 25;
        //modDepthLabels[i].setBounds(5, height, 100, size);
        //modDepthSliders[i].setBounds(105, height, 185, size);
        //height += 25;
    }
    DBG(String(height));
}
