/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FftTestAudioProcessorEditor::FftTestAudioProcessorEditor (FftTestAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    freqSelSlider.setSliderStyle(juce::Slider::LinearVertical);
    freqSelSlider.setRange(0.0, 20000.0, 1);
    freqSelSlider.setValue(1000.0);
    freqSelSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100.0, 20.0);
    freqSelSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
    addAndMakeVisible(freqSelSlider);
    
    freqAmpLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    freqAmpLabel.setColour(juce::Label::outlineColourId, juce::Colours::black);
    addAndMakeVisible(freqAmpLabel);
    freqAmpDescriptorLabel.setText("Amplitude of Chosen Frequency", juce::dontSendNotification);
    freqAmpDescriptorLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    addAndMakeVisible(freqAmpDescriptorLabel);
    
    loudestFreqLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    loudestFreqLabel.setColour(juce::Label::outlineColourId, juce::Colours::black);
    addAndMakeVisible(loudestFreqLabel);
    loudestFreqDescriptorLabel.setText("Frequency with the Highest Amplitude", juce::dontSendNotification);
    loudestFreqDescriptorLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    addAndMakeVisible(loudestFreqDescriptorLabel);
    
    juce::Timer::startTimerHz(5); 
}

FftTestAudioProcessorEditor::~FftTestAudioProcessorEditor()
{
}

//==============================================================================
void FftTestAudioProcessorEditor::paint (juce::Graphics& g)
{
    
}

void FftTestAudioProcessorEditor::resized()
{
    freqSelSlider.setBounds(10, getHeight() / 6, 100, getHeight() / 3 * 2);
    
    freqAmpLabel.setBounds(130.0, getHeight() / 12.0 * 9.25, 70.0, 20.0);
    freqAmpDescriptorLabel.setBounds(130, getHeight() / 6 * 3.75, 100, 40);
    
    loudestFreqLabel.setBounds(250.0, getHeight() / 12.0 * 9.25, 70.0, 20.0);
    loudestFreqDescriptorLabel.setBounds(250, getHeight() / 6 * 3.75, 100, 40);
}

void FftTestAudioProcessorEditor::timerCallback()
{
    freqAmpLabel.setText(juce::String(audioProcessor.getAmplitudeAtFreq(freqSelSlider.getValue(), 0)), juce::dontSendNotification);
    
    loudestFreqLabel.setText(juce::String(audioProcessor.getFreqWithHighestAmpl(0)), juce::dontSendNotification);
}
