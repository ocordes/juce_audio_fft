/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class Juce_audio_fftAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    Juce_audio_fftAudioProcessorEditor (Juce_audio_fftAudioProcessor&);
    ~Juce_audio_fftAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Juce_audio_fftAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Juce_audio_fftAudioProcessorEditor)
};
