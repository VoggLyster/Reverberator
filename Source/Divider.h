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
        // In your constructor, you should add any child components, and
        // initialise any special settings that your component needs.

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
        // This method is where you should set the bounds of any child
        // components that your component contains..

    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Divider)
};
