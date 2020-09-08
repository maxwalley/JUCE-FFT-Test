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
class FftTestAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                     public juce::Timer
{
public:
    FftTestAudioProcessorEditor (FftTestAudioProcessor&);
    ~FftTestAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    
    void timerCallback() override;
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FftTestAudioProcessor& audioProcessor;
    
    juce::Slider freqSelSlider;
    
    juce::Label freqAmpDescriptorLabel;
    juce::Label freqAmpLabel;
    
    juce::Label loudestFreqDescriptorLabel;
    juce::Label loudestFreqLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FftTestAudioProcessorEditor)
};
