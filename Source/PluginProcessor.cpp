/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Juce_audio_fftAudioProcessor::Juce_audio_fftAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

Juce_audio_fftAudioProcessor::~Juce_audio_fftAudioProcessor()
{
}

//==============================================================================
const juce::String Juce_audio_fftAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Juce_audio_fftAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Juce_audio_fftAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Juce_audio_fftAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Juce_audio_fftAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Juce_audio_fftAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Juce_audio_fftAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Juce_audio_fftAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Juce_audio_fftAudioProcessor::getProgramName (int index)
{
    return {};
}

void Juce_audio_fftAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

juce::AudioProcessorParameter* Juce_audio_fftAudioProcessor::getBypassParameter() const
{
    return apvts.getParameter("Bypass");
}

//==============================================================================
void Juce_audio_fftAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    setLatencySamples(fft[0].getLatencyInSamples());

    fft[0].reset();
    fft[1].reset();
}

void Juce_audio_fftAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}


bool Juce_audio_fftAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    {
        return false;
    }
    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    {
        return false;
    }
   #endif

    return true;
  #endif
}


void Juce_audio_fftAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto numInputChannels = getTotalNumInputChannels();
    auto numOutputChannels = getTotalNumOutputChannels();
    auto numSamples = buffer.getNumSamples();

    
    for (auto i = numInputChannels; i < numOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    
    bool bypassed = apvts.getRawParameterValue("Bypass")->load();
    
    float* channelL = buffer.getWritePointer(0);
    float* channelR = buffer.getWritePointer(1);

    // Processing on a sample-by-sample basis:
    for (int sample = 0; sample < numSamples; ++sample) {
        float sampleL = channelL[sample];
        float sampleR = channelR[sample];

        sampleL = fft[0].processSample(sampleL, bypassed);
        sampleR = fft[1].processSample(sampleR, bypassed);
        sampleL *= 0.1f;
        sampleR *= 0.1f;

        channelL[sample] = sampleL;
        channelR[sample] = sampleR;
    }
    
}

//==============================================================================
bool Juce_audio_fftAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Juce_audio_fftAudioProcessor::createEditor()
{
    return new Juce_audio_fftAudioProcessorEditor (*this);
}

//==============================================================================
void Juce_audio_fftAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    copyXmlToBinary(*apvts.copyState().createXml(), destData);
}

void Juce_audio_fftAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
        if (xml.get() != nullptr && xml->hasTagName(apvts.state.getType())) {
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
        }
}

juce::AudioProcessorValueTreeState::ParameterLayout Juce_audio_fftAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("Bypass", 1),
        "Bypass",
        false));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Juce_audio_fftAudioProcessor();
}
