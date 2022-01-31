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
    , parameters(*this, nullptr, Identifier("Reverberator"),
        {
            std::make_unique<AudioParameterFloat>("b0_gain", "b0_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("b1_gain", "b1_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("b2_gain", "b2_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("b3_gain", "b3_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("b4_gain", "b4_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("b5_gain", "b5_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("b6_gain", "b6_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("b7_gain", "b7_gain", 0.0f, 1.0f, 0.9f),

            std::make_unique<AudioParameterFloat>("c0_gain", "c0_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("c1_gain", "c1_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("c2_gain", "c2_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("c3_gain", "c3_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("c4_gain", "c4_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("c5_gain", "c5_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("c6_gain", "c6_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("c7_gain", "c7_gain", 0.0f, 1.0f, 0.9f),

            std::make_unique<AudioParameterFloat>("filter0_coeff0", "filter0_coeff0", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter0_coeff1", "filter0_coeff1", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter0_coeff2", "filter0_coeff2", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter0_coeff3", "filter0_coeff3", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter0_coeff4", "filter0_coeff4", 0.0f, 1.0f, 0.5f),

            std::make_unique<AudioParameterFloat>("filter1_coeff0", "filter1_coeff0", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter1_coeff1", "filter1_coeff1", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter1_coeff2", "filter1_coeff2", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter1_coeff3", "filter1_coeff3", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter1_coeff4", "filter1_coeff4", 0.0f, 1.0f, 0.5f),

            std::make_unique<AudioParameterFloat>("filter2_coeff0", "filter2_coeff0", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter2_coeff1", "filter2_coeff1", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter2_coeff2", "filter2_coeff2", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter2_coeff3", "filter2_coeff3", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter2_coeff4", "filter2_coeff4", 0.0f, 1.0f, 0.5f),

            std::make_unique<AudioParameterFloat>("filter3_coeff0", "filter3_coeff0", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter3_coeff1", "filter3_coeff1", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter3_coeff2", "filter3_coeff2", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter3_coeff3", "filter3_coeff3", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter3_coeff4", "filter3_coeff4", 0.0f, 1.0f, 0.5f),

            std::make_unique<AudioParameterFloat>("filter4_coeff0", "filter4_coeff0", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter4_coeff1", "filter4_coeff1", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter4_coeff2", "filter4_coeff2", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter4_coeff3", "filter4_coeff3", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter4_coeff4", "filter4_coeff4", 0.0f, 1.0f, 0.5f),

            std::make_unique<AudioParameterFloat>("filter5_coeff0", "filter5_coeff0", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter5_coeff1", "filter5_coeff1", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter5_coeff2", "filter5_coeff2", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter5_coeff3", "filter5_coeff3", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter5_coeff4", "filter5_coeff4", 0.0f, 1.0f, 0.5f),

            std::make_unique<AudioParameterFloat>("filter6_coeff0", "filter6_coeff0", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter6_coeff1", "filter6_coeff1", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter6_coeff2", "filter6_coeff2", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter6_coeff3", "filter6_coeff3", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter6_coeff4", "filter6_coeff4", 0.0f, 1.0f, 0.5f),

            std::make_unique<AudioParameterFloat>("filter7_coeff0", "filter7_coeff0", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter7_coeff1", "filter7_coeff1", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter7_coeff2", "filter7_coeff2", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter7_coeff3", "filter7_coeff3", 0.0f, 1.0f, 0.5f),
            std::make_unique<AudioParameterFloat>("filter7_coeff4", "filter7_coeff4", 0.0f, 1.0f, 0.5f),

            std::make_unique<AudioParameterFloat>("mod0_freq", "mod0_freq", 0.0f, 2.0f, 0.0f),
            std::make_unique<AudioParameterFloat>("mod1_freq", "mod1_freq", 0.0f, 2.0f, 0.0f),
            std::make_unique<AudioParameterFloat>("mod2_freq", "mod2_freq", 0.0f, 2.0f, 0.0f),
            std::make_unique<AudioParameterFloat>("mod3_freq", "mod3_freq", 0.0f, 2.0f, 0.0f),
            std::make_unique<AudioParameterFloat>("mod4_freq", "mod4_freq", 0.0f, 2.0f, 0.0f),
            std::make_unique<AudioParameterFloat>("mod5_freq", "mod5_freq", 0.0f, 2.0f, 0.0f),
            std::make_unique<AudioParameterFloat>("mod6_freq", "mod6_freq", 0.0f, 2.0f, 0.0f),
            std::make_unique<AudioParameterFloat>("mod7_freq", "mod7_freq", 0.0f, 2.0f, 0.0f),
        })
{
    juce::String parameterName = "";
    for (int i = 0; i < N_LINES; i++) {
        parameterName = "b" + juce::String(i) + "_gain";
        cParameters[i] = parameters.getRawParameterValue(parameterName);
        parameters.addParameterListener(parameterName, this);
        parameterName = "c" + juce::String(i) + "_gain";
        bParameters[i] = parameters.getRawParameterValue(parameterName);
        parameters.addParameterListener(parameterName, this);
        for (int j = 0; j < 5; j++) {
            parameterName = "filter" + juce::String(i) + "_coeff" + juce::String(j);
            filterCoeffParameters[i][j] = parameters.getRawParameterValue(parameterName);
            parameters.addParameterListener(parameterName, this);
        }
        parameterName = "mod" + juce::String(i) + "_freq";
        modFreqParameters[i] = parameters.getRawParameterValue(parameterName);
        parameters.addParameterListener(parameterName, this);
    }
    reverbProcessor = std::make_unique<ReverbProcessor>(M);
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
    reverbProcessor->setParameters(bParameters, cParameters, filterCoeffParameters, modFreqParameters);
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

    stereo = (totalNumInputChannels > 1) ? true : false;

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    DBG("input channels: " + String(totalNumInputChannels) + ", output channels: " + String(totalNumOutputChannels));

    auto* leftChannelData = buffer.getWritePointer(0);
    auto* rightChannelData = buffer.getWritePointer(1);

    for (int n = 0; n < buffer.getNumSamples(); n++) {
        if (stereo) {
            std::vector<float> output = reverbProcessor->processStereo({ leftChannelData[n], rightChannelData[n] });
            leftChannelData[n] = output[0];
            rightChannelData[n] = output[1];
        }
        else {
            float output = reverbProcessor->process(leftChannelData[n]);
            leftChannelData[n] = output;
        }
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
    reverbProcessor->setParameters(bParameters, cParameters, filterCoeffParameters, modFreqParameters);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ReverberatorAudioProcessor();
}
