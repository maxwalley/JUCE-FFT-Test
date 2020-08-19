/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
struct ChannelFreqBuffers
{
public:
    std::array<std::complex<float>, 1024> fftData;
    std::array<std::complex<float>, 1024> delayedBuffer;
    
    int fftInputArrayIndex;
};

/**
*/
class FftTestAudioProcessor  : public juce::AudioProcessor,
                               public juce::Timer
{
public:
    //==============================================================================
    FftTestAudioProcessor();
    ~FftTestAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
   
    void numChannelsChanged() override;
    
    void timerCallback() override;
    
    const int fftOrder = 10;
    const int fftSize = std::pow(2, fftOrder);
    
    juce::dsp::FFT forwardTransform;
    
    std::vector<ChannelFreqBuffers> freqBuffers;
    
    std::array<std::complex<float>, 1024> fftData;
    std::array<std::complex<float>, 1024> delayedBuffer;
    
    int fftInputArrayIndex;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FftTestAudioProcessor)
};
