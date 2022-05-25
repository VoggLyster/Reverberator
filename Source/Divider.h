/*
  ==============================================================================

    Divider.h
    Created: 1 Feb 2022 7:09:29pm
    Author:  svkly

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class Divider  : public juce::Component
{
public:
    Divider()
    {
    }

    ~Divider() override
    {
    }

    void paint (juce::Graphics& g) override
    {
        g.setColour(juce::Colours::slategrey);
        juce::Line<float> line(juce::Point<float>(10, 10), juce::Point<float>(300, 10));
        g.drawLine(line, 2.0f);
    }

    void resized() override
    {
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Divider)
};
