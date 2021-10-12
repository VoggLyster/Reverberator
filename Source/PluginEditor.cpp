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
        name = "b" + String(i) + "_gain";
        bLabels[i].setText(name, juce::dontSendNotification);
        addAndMakeVisible(bLabels[i]);
        addAndMakeVisible(bSliders[i]);
        bAttachments[i].reset(new SliderAttachment(valueTreeState, name, bSliders[i]));
        name = "c" + String(i) + "_gain";
        cLabels[i].setText(name, juce::dontSendNotification);
        addAndMakeVisible(cLabels[i]);
        addAndMakeVisible(cSliders[i]);
        cAttachments[i].reset(new SliderAttachment(valueTreeState, name, cSliders[i]));
        name = "M" + String(i) + "_delay";
        MLabels[i].setText(name, juce::dontSendNotification);
        addAndMakeVisible(MLabels[i]);
        addAndMakeVisible(MSliders[i]);
        MAttachments[i].reset(new SliderAttachment(valueTreeState, name, MSliders[i]));
        for (int j = 0; j < 5; j++) {
            name = "filter" + String(i) + "_coeff" + String(j);
            coeffLabels[i][j].setText(name, juce::dontSendNotification);
            addAndMakeVisible(coeffLabels[i][j]);
            addAndMakeVisible(coeffSliders[i][j]);
            coeffAttachments[i][j].reset(new SliderAttachment(valueTreeState, name, coeffSliders[i][j]));
        }
    }

    setSize (1600, 820);
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
    int size = 20;
    int border = 5;
    int c = 0;

    for (int i = 0; i < N_LINES; i++) 
    {
        bLabels[i].setBounds(5, i * (border + size), 100, size);
        bSliders[i].setBounds(105, i * (border + size), 185, size);

        cLabels[i].setBounds(5, 16 * (border + size) + 20 + i * (border + size), 100, size);
        cSliders[i].setBounds(105, 16 * (border + size) + 20 + i * (border + size), 185, size);

        MLabels[i].setBounds(305, i * (border + size), 100, size);
        MSliders[i].setBounds(405, i * (border + size), 185, size);

        if (i == 0) {
            for (int j = 0; j < 5; j++) {
                coeffLabels[i][j].setBounds(305, 16 * (border + size) + 20 + c * (border + size), 100, size);
                coeffSliders[i][j].setBounds(405, 16 * (border + size) + 20 + c * (border + size), 185, size);
                c++;
            }
        }

        if (i == 1) {
            for (int j = 0; j < 5; j++) {
                coeffLabels[i][j].setBounds(305, 16 * (border + size) + 40 + c * (border + size), 100, size);
                coeffSliders[i][j].setBounds(405, 16 * (border + size) + 40 + c * (border + size), 185, size);
                c++;
            }
        }

        if (i == 2) {
            c = 0;
            for (int j = 0; j < 5; j++) {
                coeffLabels[i][j].setBounds(605, c * (border + size), 100, size);
                coeffSliders[i][j].setBounds(705, c * (border + size), 185, size);
                c++;
            }
        }

        if (i == 3) {
            for (int j = 0; j < 5; j++) {
                coeffLabels[i][j].setBounds(605, 20 + c * (border + size), 100, size);
                coeffSliders[i][j].setBounds(705, 20 + c * (border + size), 185, size);
                c++;
            }
        }

        if (i == 4) {
            for (int j = 0; j < 5; j++) {
                coeffLabels[i][j].setBounds(605, 40 + c * (border + size), 100, size);
                coeffSliders[i][j].setBounds(705, 40 + c * (border + size), 185, size);
                c++;
            }
        }

        if (i == 5) {
            for (int j = 0; j < 5; j++) {
                coeffLabels[i][j].setBounds(605, 60 + c * (border + size), 100, size);
                coeffSliders[i][j].setBounds(705, 60 + c * (border + size), 185, size);
                c++;
            }
        }

        if (i == 6) {
            for (int j = 0; j < 5; j++) {
                coeffLabels[i][j].setBounds(605, 80 + c * (border + size), 100, size);
                coeffSliders[i][j].setBounds(705, 80 + c * (border + size), 185, size);
                c++;
            }
        }

        if (i == 7) {
            c = 0;
            for (int j = 0; j < 5; j++) {
                coeffLabels[i][j].setBounds(905, c * (border + size), 100, size);
                coeffSliders[i][j].setBounds(1005, c * (border + size), 185, size);
                c++;
            }
        }

        if (i == 8) {
            for (int j = 0; j < 5; j++) {
                coeffLabels[i][j].setBounds(905, 20 + c * (border + size), 100, size);
                coeffSliders[i][j].setBounds(1005, 20 + c * (border + size), 185, size);
                c++;
            }
        }

        if (i == 9) {
            for (int j = 0; j < 5; j++) {
                coeffLabels[i][j].setBounds(905, 40 + c * (border + size), 100, size);
                coeffSliders[i][j].setBounds(1005, 40 + c * (border + size), 185, size);
                c++;
            }
        }

        if (i == 10) {
            for (int j = 0; j < 5; j++) {
                coeffLabels[i][j].setBounds(905, 60 + c * (border + size), 100, size);
                coeffSliders[i][j].setBounds(1005, 60 + c * (border + size), 185, size);
                c++;
            }
        }

        if (i == 11) {
            for (int j = 0; j < 5; j++) {
                coeffLabels[i][j].setBounds(905, 80 + c * (border + size), 100, size);
                coeffSliders[i][j].setBounds(1005, 80 + c * (border + size), 185, size);
                c++;
            }
        }

        if (i == 12) {
            c = 0;
            for (int j = 0; j < 5; j++) {
                coeffLabels[i][j].setBounds(1205, c * (border + size), 100, size);
                coeffSliders[i][j].setBounds(1305, c * (border + size), 185, size);
                c++;
            }
        }

        if (i == 13) {
            for (int j = 0; j < 5; j++) {
                coeffLabels[i][j].setBounds(1205, 20 + c * (border + size), 100, size);
                coeffSliders[i][j].setBounds(1305, 20 + c * (border + size), 185, size);
                c++;
            }
        }

        if (i == 14) {
            for (int j = 0; j < 5; j++) {
                coeffLabels[i][j].setBounds(1205, 40 + c * (border + size), 100, size);
                coeffSliders[i][j].setBounds(1305, 40 + c * (border + size), 185, size);
                c++;
            }
        }

        if (i == 15) {
            for (int j = 0; j < 5; j++) {
                coeffLabels[i][j].setBounds(1205, 60 + c * (border + size), 100, size);
                coeffSliders[i][j].setBounds(1305, 60 + c * (border + size), 185, size);
                c++;
            }
        }
    }
}
