/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ReverberatorAudioProcessor::ReverberatorAudioProcessor()
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
    , parameters(*this, nullptr, Identifier("Reverberator"), createParameterLayout())
{
    juce::String parameterName = "";
    //parameterName = "delay_length_max";
    //delayLengthMaxParameter = parameters.getRawParameterValue(parameterName);
    //parameters.addParameterListener(parameterName, this);
    //parameterName = "delay_length_min";
    //delayLengthMinParameter = parameters.getRawParameterValue(parameterName);
    //parameters.addParameterListener(parameterName, this);
    for (int i = 0; i < N_LINES; i++) {
        //parameterName = "b" + juce::String(i) + "_gain";
        //cParameters[i] = parameters.getRawParameterValue(parameterName);
        //parameters.addParameterListener(parameterName, this);
        //parameterName = "c" + juce::String(i) + "_gain";
        //bParameters[i] = parameters.getRawParameterValue(parameterName);
        //parameters.addParameterListener(parameterName, this);
        parameterName = "c_hp" + juce::String(i);
        filterCoeffParameters[i][0] = parameters.getRawParameterValue(parameterName);
        parameters.addParameterListener(parameterName, this);
        parameterName = "c_bp" + juce::String(i);
        filterCoeffParameters[i][1] = parameters.getRawParameterValue(parameterName);
        parameters.addParameterListener(parameterName, this);
        parameterName = "c_lp" + juce::String(i);
        filterCoeffParameters[i][2] = parameters.getRawParameterValue(parameterName);
        parameters.addParameterListener(parameterName, this);
        parameterName = "R" + juce::String(i);
        filterCoeffParameters[i][3] = parameters.getRawParameterValue(parameterName);
        parameters.addParameterListener(parameterName, this);
        parameterName = "g" + juce::String(i);
        filterCoeffParameters[i][4] = parameters.getRawParameterValue(parameterName);
        parameters.addParameterListener(parameterName, this);
        //parameterName = "mod" + juce::String(i) + "_freq";
        //modFreqParameters[i] = parameters.getRawParameterValue(parameterName);
        //parameters.addParameterListener(parameterName, this);
    }

    reverbProcessor = std::make_unique<ReverbProcessor>();
    DBG("ReverbProcessor created");
}

ReverberatorAudioProcessor::~ReverberatorAudioProcessor()
{
}

//==============================================================================
const juce::String ReverberatorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ReverberatorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ReverberatorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ReverberatorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ReverberatorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ReverberatorAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ReverberatorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ReverberatorAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ReverberatorAudioProcessor::getProgramName (int index)
{
    return {};
}

void ReverberatorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ReverberatorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    reverbProcessor->prepare(sampleRate, samplesPerBlock);
    reverbProcessor->setParameters(filterCoeffParameters);
}

void ReverberatorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ReverberatorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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

void ReverberatorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    /*stereo = (totalNumInputChannels > 1) ? true : false;*/
    stereo = false;

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    DBG("input channels: " + String(totalNumInputChannels) + ", output channels: " + String(totalNumOutputChannels));

    auto* leftChannelData = buffer.getWritePointer(0);
    auto* rightChannelData = buffer.getWritePointer(1);

    for (int n = 0; n < buffer.getNumSamples(); n++) {
        float output = reverbProcessor->process(leftChannelData[n]);
        leftChannelData[n] = output;
    }
}

//==============================================================================
bool ReverberatorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ReverberatorAudioProcessor::createEditor()
{
    return new ReverberatorAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void ReverberatorAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void ReverberatorAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

void ReverberatorAudioProcessor::parameterChanged(const String& parameterID, float newValue)
{
    reverbProcessor->setParameters(filterCoeffParameters);
}

juce::AudioProcessorValueTreeState::ParameterLayout ReverberatorAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout params;
    
    //params.add(std::make_unique<AudioParameterFloat>("delay_length_max", "delay_length_max", 0.0f, 1.0f, 0.5f)); //Max delay length = [2500,5000]
    //params.add(std::make_unique<AudioParameterFloat>("delay_length_min", "delay_length_min", 0.0f, 1.0f, 0.5f)); //Min delay length = [100, 2500]
    juce::String name = "";
    for (int i = 0; i < N_LINES; i++) {
        //name = "b" + String(i) + "_gain";
        //params.add(std::make_unique<AudioParameterFloat>(name, name, 0.0f, 1.0f, 0.9f));
        //name = "c" + String(i) + "_gain";
        //params.add(std::make_unique<AudioParameterFloat>(name, name, 0.0f, 1.0f, 0.9f));
        name = "c_hp" + String(i);
        params.add(std::make_unique<AudioParameterFloat>(name, name, 0.0f, 1.0f, 0.5f));
        name = "c_bp" + String(i);
        params.add(std::make_unique<AudioParameterFloat>(name, name, 0.0f, 1.0f, 0.5f));
        name = "c_lp" + String(i);
        params.add(std::make_unique<AudioParameterFloat>(name, name, 0.0f, 1.0f, 0.5f));
        name = "R" + String(i);
        params.add(std::make_unique<AudioParameterFloat>(name, name, 0.0f, 1.0f, 0.5f));
        name = "g" + String(i);
        params.add(std::make_unique<AudioParameterFloat>(name, name, 0.0f, 1.0f, 0.5f));
        //name = "mod" + String(i) + "_freq";
        //params.add(std::make_unique<AudioParameterFloat>(name, name, 0.0f, 1.0f, 0.5f));
    }


    return params;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ReverberatorAudioProcessor();
}
