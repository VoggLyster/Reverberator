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
    parameterName = "delay_length_min";
    delayLengthMinParameter = parameters.getRawParameterValue(parameterName);
    parameters.addParameterListener(parameterName, this);
    parameterName = "delay_length_max";
    delayLengthMaxParameter = parameters.getRawParameterValue(parameterName);
    parameters.addParameterListener(parameterName, this);
    parameterName = "predelay_length";
    predelayLengthParameter = parameters.getRawParameterValue(parameterName);
    parameters.addParameterListener(parameterName, this);

    for (int j = 0; j < N_EQ; j++) {
        parameterName = "RT_" + String(j);
        attenuationGainParameters[j] = parameters.getRawParameterValue(parameterName);
        parameters.addParameterListener(parameterName, this);
    }

    for (int j = 0; j < N_EQ; j++) {
        parameterName = "tonal_gain_" + String(j);
        tonalGainParameters[j] = parameters.getRawParameterValue(parameterName);
        parameters.addParameterListener(parameterName, this);
    }

    for (int i = 0; i < N_LINES; i++) {
        parameterName = "b" + juce::String(i) + "_gain";
        bParameters[i] = parameters.getRawParameterValue(parameterName);
        parameters.addParameterListener(parameterName, this);
        parameterName = "c" + juce::String(i) + "_gain";
        cParameters[i] = parameters.getRawParameterValue(parameterName);
        parameters.addParameterListener(parameterName, this);

        parameterName = "mod" + juce::String(i) + "_freq";
        modFreqParameters[i] = parameters.getRawParameterValue(parameterName);
        parameters.addParameterListener(parameterName, this);
        parameterName = "mod" + juce::String(i) + "_depth";
        modDepthParameters[i] = parameters.getRawParameterValue(parameterName);
        parameters.addParameterListener(parameterName, this);
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
    reverbProcessor->setBGainParameters(bParameters);
    reverbProcessor->setCGainParameters(cParameters);
    reverbProcessor->setAttenuationGainParameters(attenuationGainParameters);
    reverbProcessor->setTonalGainParameters(tonalGainParameters);
    reverbProcessor->setDelayLengthParameters(delayLengthMinParameter, delayLengthMaxParameter);
    reverbProcessor->setPredelayLengthParameter(predelayLengthParameter);
    reverbProcessor->setModFrequencyParameters(modFreqParameters);
    reverbProcessor->setModDepthParameters(modDepthParameters);
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

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());


    auto* leftChannelData = buffer.getWritePointer(0);
    //auto* rightChannelData = buffer.getWritePointer(1);

    for (int n = 0; n < buffer.getNumSamples(); n++) {
        float output = reverbProcessor->process(leftChannelData[n]);
        leftChannelData[n] = output;
        //rightChannelData[n] = output;
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
    if (parameterID.contains("b"))
        reverbProcessor->setBGainParameters(bParameters);
    else if (parameterID.contains("c"))
        reverbProcessor->setCGainParameters(cParameters);
    else if (parameterID.contains("RT_"))
        reverbProcessor->setAttenuationGainParameters(attenuationGainParameters);
    else if (parameterID.contains("tonal_gain_"))
        reverbProcessor->setTonalGainParameters(tonalGainParameters);
    else if (parameterID.contains("delay_length_max") || parameterID.contains("delay_length_min"))
        reverbProcessor->setDelayLengthParameters(delayLengthMinParameter, delayLengthMaxParameter);
    else if (parameterID.contains("predelay_length"))
        reverbProcessor->setPredelayLengthParameter(predelayLengthParameter);
    else if (parameterID.contains("_freq"))
        reverbProcessor->setModFrequencyParameters(modFreqParameters);
    else if (parameterID.contains("_depth"))
        reverbProcessor->setModDepthParameters(modDepthParameters);
}

void ReverberatorAudioProcessor::reset()
{
    reverbProcessor->reset();
}

juce::AudioProcessorValueTreeState::ParameterLayout ReverberatorAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout params;
    
    params.add(std::make_unique<AudioParameterFloat>("delay_length_min", "delay_length_min", 0.0f, 100.0f, 50.0f));
    params.add(std::make_unique<AudioParameterFloat>("delay_length_max", "delay_length_max", 0.0f, 100.0f, 50.0f));
    params.add(std::make_unique<AudioParameterFloat>("predelay_length", "predelay_length", 0.0f, 100.0f, 20.0f));
    juce::String name = "";
    for (int j = 0; j < N_EQ; j++) {
        name = "RT_" + String(j);
        params.add(std::make_unique<AudioParameterFloat>(name, name, 0.0f, 100.0f, 50.0f));
    }
    for (int j = 0; j < N_EQ; j++) {
        name = "tonal_gain_" + String(j);
        params.add(std::make_unique<AudioParameterFloat>(name, name, 0.0f, 100.0f, 50.0f));
    }
    for (int i = 0; i < N_LINES; i++) {
        name = "b" + String(i) + "_gain";
        params.add(std::make_unique<AudioParameterFloat>(name, name, 0.0f, 100.0f, 50.0f));
        name = "c" + String(i) + "_gain";
        params.add(std::make_unique<AudioParameterFloat>(name, name, 0.0f, 100.0f, 50.0f));
        name = "mod" + String(i) + "_freq";
        params.add(std::make_unique<AudioParameterFloat>(name, name, 0.0f, 100.0f, 0.0f));
        name = "mod" + String(i) + "_depth";
        params.add(std::make_unique<AudioParameterFloat>(name, name, 0.0f, 100.0f, 0.0f));
    }


    return params;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ReverberatorAudioProcessor();
}
