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
            std::make_unique<AudioParameterFloat>("b8_gain", "b8_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("b9_gain", "b9_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("b10_gain", "b10_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("b11_gain", "b11_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("b12_gain", "b12_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("b13_gain", "b13_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("b14_gain", "b14_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("b15_gain", "b15_gain", 0.0f, 1.0f, 0.9f),

            std::make_unique<AudioParameterFloat>("c0_gain", "c0_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("c1_gain", "c1_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("c2_gain", "c2_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("c3_gain", "c3_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("c4_gain", "c4_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("c5_gain", "c5_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("c6_gain", "c6_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("c7_gain", "c7_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("c8_gain", "c8_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("c9_gain", "c9_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("c10_gain", "c10_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("c11_gain", "c11_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("c12_gain", "c12_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("c13_gain", "c13_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("c14_gain", "c14_gain", 0.0f, 1.0f, 0.9f),
            std::make_unique<AudioParameterFloat>("c15_gain", "c15_gain", 0.0f, 1.0f, 0.9f),

            //std::make_unique<AudioParameterInt>("M0_delay", "M0_delay", 500, 30000, 2927),
            //std::make_unique<AudioParameterInt>("M1_delay", "M1_delay", 500, 30000, 2593),
            //std::make_unique<AudioParameterInt>("M2_delay", "M2_delay", 500, 30000, 2273),
            //std::make_unique<AudioParameterInt>("M3_delay", "M3_delay", 500, 30000, 3697),
            //std::make_unique<AudioParameterInt>("M4_delay", "M4_delay", 500, 30000, 1877),
            //std::make_unique<AudioParameterInt>("M5_delay", "M5_delay", 500, 30000, 3877),
            //std::make_unique<AudioParameterInt>("M6_delay", "M6_delay", 500, 30000, 2477),
            //std::make_unique<AudioParameterInt>("M7_delay", "M7_delay", 500, 30000, 3461),
            //std::make_unique<AudioParameterInt>("M8_delay", "M8_delay", 500, 30000, 1609),
            //std::make_unique<AudioParameterInt>("M9_delay", "M9_delay", 500, 30000, 3779),
            //std::make_unique<AudioParameterInt>("M10_delay", "M10_delay", 500, 30000, 3541),
            //std::make_unique<AudioParameterInt>("M11_delay", "M11_delay", 500, 30000, 4259),
            //std::make_unique<AudioParameterInt>("M12_delay", "M12_delay", 500, 30000, 1669),
            //std::make_unique<AudioParameterInt>("M13_delay", "M13_delay", 500, 30000, 3539),
            //std::make_unique<AudioParameterInt>("M14_delay", "M14_delay", 500, 30000, 3637),
            //std::make_unique<AudioParameterInt>("M15_delay", "M15_delay", 500, 30000, 4013),

            std::make_unique<AudioParameterFloat>("filter0_coeff0", "filter0_coeff0", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter0_coeff1", "filter0_coeff1", -1.0f, 1.0f, 0.195f),
            std::make_unique<AudioParameterFloat>("filter0_coeff2", "filter0_coeff2", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter0_coeff3", "filter0_coeff3", -1.0f, 1.0f, -0.943f),
            std::make_unique<AudioParameterFloat>("filter0_coeff4", "filter0_coeff4", -1.0f, 1.0f, 0.333f),

            std::make_unique<AudioParameterFloat>("filter1_coeff0", "filter1_coeff0", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter1_coeff1", "filter1_coeff1", -1.0f, 1.0f, 0.195f),
            std::make_unique<AudioParameterFloat>("filter1_coeff2", "filter1_coeff2", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter1_coeff3", "filter1_coeff3", -1.0f, 1.0f, -0.943f),
            std::make_unique<AudioParameterFloat>("filter1_coeff4", "filter1_coeff4", -1.0f, 1.0f, 0.333f),

            std::make_unique<AudioParameterFloat>("filter2_coeff0", "filter2_coeff0", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter2_coeff1", "filter2_coeff1", -1.0f, 1.0f, 0.195f),
            std::make_unique<AudioParameterFloat>("filter2_coeff2", "filter2_coeff2", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter2_coeff3", "filter2_coeff3", -1.0f, 1.0f, -0.943f),
            std::make_unique<AudioParameterFloat>("filter2_coeff4", "filter2_coeff4", -1.0f, 1.0f, 0.333f),

            std::make_unique<AudioParameterFloat>("filter3_coeff0", "filter3_coeff0", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter3_coeff1", "filter3_coeff1", -1.0f, 1.0f, 0.195f),
            std::make_unique<AudioParameterFloat>("filter3_coeff2", "filter3_coeff2", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter3_coeff3", "filter3_coeff3", -1.0f, 1.0f, -0.943f),
            std::make_unique<AudioParameterFloat>("filter3_coeff4", "filter3_coeff4", -1.0f, 1.0f, 0.333f),

            std::make_unique<AudioParameterFloat>("filter4_coeff0", "filter4_coeff0", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter4_coeff1", "filter4_coeff1", -1.0f, 1.0f, 0.195f),
            std::make_unique<AudioParameterFloat>("filter4_coeff2", "filter4_coeff2", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter4_coeff3", "filter4_coeff3", -1.0f, 1.0f, -0.943f),
            std::make_unique<AudioParameterFloat>("filter4_coeff4", "filter4_coeff4", -1.0f, 1.0f, 0.333f),

            std::make_unique<AudioParameterFloat>("filter5_coeff0", "filter5_coeff0", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter5_coeff1", "filter5_coeff1", -1.0f, 1.0f, 0.195f),
            std::make_unique<AudioParameterFloat>("filter5_coeff2", "filter5_coeff2", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter5_coeff3", "filter5_coeff3", -1.0f, 1.0f, -0.943f),
            std::make_unique<AudioParameterFloat>("filter5_coeff4", "filter5_coeff4", -1.0f, 1.0f, 0.333f),

            std::make_unique<AudioParameterFloat>("filter6_coeff0", "filter6_coeff0", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter6_coeff1", "filter6_coeff1", -1.0f, 1.0f, 0.195f),
            std::make_unique<AudioParameterFloat>("filter6_coeff2", "filter6_coeff2", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter6_coeff3", "filter6_coeff3", -1.0f, 1.0f, -0.943f),
            std::make_unique<AudioParameterFloat>("filter6_coeff4", "filter6_coeff4", -1.0f, 1.0f, 0.333f),

            std::make_unique<AudioParameterFloat>("filter7_coeff0", "filter7_coeff0", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter7_coeff1", "filter7_coeff1", -1.0f, 1.0f, 0.195f),
            std::make_unique<AudioParameterFloat>("filter7_coeff2", "filter7_coeff2", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter7_coeff3", "filter7_coeff3", -1.0f, 1.0f, -0.943f),
            std::make_unique<AudioParameterFloat>("filter7_coeff4", "filter7_coeff4", -1.0f, 1.0f, 0.333f),

            std::make_unique<AudioParameterFloat>("filter8_coeff0", "filter8_coeff0", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter8_coeff1", "filter8_coeff1", -1.0f, 1.0f, 0.195f),
            std::make_unique<AudioParameterFloat>("filter8_coeff2", "filter8_coeff2", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter8_coeff3", "filter8_coeff3", -1.0f, 1.0f, -0.943f),
            std::make_unique<AudioParameterFloat>("filter8_coeff4", "filter8_coeff4", -1.0f, 1.0f, 0.333f),

            std::make_unique<AudioParameterFloat>("filter9_coeff0", "filter9_coeff0", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter9_coeff1", "filter9_coeff1", -1.0f, 1.0f, 0.195f),
            std::make_unique<AudioParameterFloat>("filter9_coeff2", "filter9_coeff2", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter9_coeff3", "filter9_coeff3", -1.0f, 1.0f, -0.943f),
            std::make_unique<AudioParameterFloat>("filter9_coeff4", "filter9_coeff4", -1.0f, 1.0f, 0.333f),

            std::make_unique<AudioParameterFloat>("filter10_coeff0", "filter10_coeff0", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter10_coeff1", "filter10_coeff1", -1.0f, 1.0f, 0.195f),
            std::make_unique<AudioParameterFloat>("filter10_coeff2", "filter10_coeff2", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter10_coeff3", "filter10_coeff3", -1.0f, 1.0f, -0.943f),
            std::make_unique<AudioParameterFloat>("filter10_coeff4", "filter10_coeff4", -1.0f, 1.0f, 0.333f),

            std::make_unique<AudioParameterFloat>("filter11_coeff0", "filter11_coeff0", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter11_coeff1", "filter11_coeff1", -1.0f, 1.0f, 0.195f),
            std::make_unique<AudioParameterFloat>("filter11_coeff2", "filter11_coeff2", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter11_coeff3", "filter11_coeff3", -1.0f, 1.0f, -0.943f),
            std::make_unique<AudioParameterFloat>("filter11_coeff4", "filter11_coeff4", -1.0f, 1.0f, 0.333f),

            std::make_unique<AudioParameterFloat>("filter12_coeff0", "filter12_coeff0", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter12_coeff1", "filter12_coeff1", -1.0f, 1.0f, 0.195f),
            std::make_unique<AudioParameterFloat>("filter12_coeff2", "filter12_coeff2", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter12_coeff3", "filter12_coeff3", -1.0f, 1.0f, -0.943f),
            std::make_unique<AudioParameterFloat>("filter12_coeff4", "filter12_coeff4", -1.0f, 1.0f, 0.333f),

            std::make_unique<AudioParameterFloat>("filter13_coeff0", "filter13_coeff0", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter13_coeff1", "filter13_coeff1", -1.0f, 1.0f, 0.195f),
            std::make_unique<AudioParameterFloat>("filter13_coeff2", "filter13_coeff2", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter13_coeff3", "filter13_coeff3", -1.0f, 1.0f, -0.943f),
            std::make_unique<AudioParameterFloat>("filter13_coeff4", "filter13_coeff4", -1.0f, 1.0f, 0.333f),

            std::make_unique<AudioParameterFloat>("filter14_coeff0", "filter14_coeff0", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter14_coeff1", "filter14_coeff1", -1.0f, 1.0f, 0.195f),
            std::make_unique<AudioParameterFloat>("filter14_coeff2", "filter14_coeff2", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter14_coeff3", "filter14_coeff3", -1.0f, 1.0f, -0.943f),
            std::make_unique<AudioParameterFloat>("filter14_coeff4", "filter14_coeff4", -1.0f, 1.0f, 0.333f),

            std::make_unique<AudioParameterFloat>("filter15_coeff0", "filter15_coeff0", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter15_coeff1", "filter15_coeff1", -1.0f, 1.0f, 0.195f),
            std::make_unique<AudioParameterFloat>("filter15_coeff2", "filter15_coeff2", -1.0f, 1.0f, 0.098f),
            std::make_unique<AudioParameterFloat>("filter15_coeff3", "filter15_coeff3", -1.0f, 1.0f, -0.943f),
            std::make_unique<AudioParameterFloat>("filter15_coeff4", "filter15_coeff4", -1.0f, 1.0f, 0.333f),

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
        //parameterName = "M" + juce::String(i) + "_delay";
        //MParameters[i] = parameters.getRawParameterValue(parameterName);
        //parameters.addParameterListener(parameterName, this);
        for (int j = 0; j < 5; j++) {
            parameterName = "filter" + juce::String(i) + "_coeff" + juce::String(j);
            filterCoeffParameters[i][j] = parameters.getRawParameterValue(parameterName);
            parameters.addParameterListener(parameterName, this);
        }
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
    //reverbProcessor->setParameters(bParameters, cParameters, MParameters, filterCoeffParameters);
    reverbProcessor->setParameters(bParameters, cParameters, filterCoeffParameters);
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
void ReverberatorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ReverberatorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void ReverberatorAudioProcessor::parameterChanged(const String& parameterID, float newValue)
{
    //reverbProcessor->setParameters(bParameters, cParameters, MParameters, filterCoeffParameters);
    reverbProcessor->setParameters(bParameters, cParameters, filterCoeffParameters);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ReverberatorAudioProcessor();
}
