/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FftTestAudioProcessor::FftTestAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), forwardTransform {fftOrder}
#endif
{
    numChannelsChanged();
}

FftTestAudioProcessor::~FftTestAudioProcessor()
{
}

//==============================================================================
const juce::String FftTestAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FftTestAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FftTestAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FftTestAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FftTestAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FftTestAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FftTestAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FftTestAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String FftTestAudioProcessor::getProgramName (int index)
{
    return {};
}

void FftTestAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FftTestAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void FftTestAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FftTestAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void FftTestAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        
        for(int sample = 0; sample < buffer.getNumSamples(); sample++)
        {
            //A whole new buffer has been written to and is ready for processing
            if(freqBuffers[channel].fftInputArrayIndex == fftSize)
            {
                
                //Time domain to freq domain
                forwardTransform.perform(freqBuffers[channel].fftInputData.data(), freqBuffers[channel].fftOutputData.data(), false);
                
                //Freq processing here
                
                /*
                 This is needed if we wanted to change the frequency data and change the signal back for playing
                 
                //Inverse to get back to time domain
                forwardTransform.perform(freqBuffers[channel].fftOutputData.data(), freqBuffers[channel].fftInputData.data(), true);
                
                //copy the buffer into the delayed buffer to be played
                std::copy(freqBuffers[channel].fftInputData.begin(), freqBuffers[channel].fftInputData.end(), freqBuffers[channel].delayedBuffer
                          .data());
                */
                freqBuffers[channel].fftInputArrayIndex = 0;
                
            }
            //Write to the fft buffer
            //Sets the real part of the complex number to the time domain data
            freqBuffers[channel].fftInputData[freqBuffers[channel].fftInputArrayIndex].real(channelData[sample]);
            
            //Read from the delayed buffer - We don't need this since we are not changing the data based on its frequency domain
            
            //channelData[sample] = freqBuffers[channel].delayedBuffer[freqBuffers[channel].fftInputArrayIndex].real();
            
            freqBuffers[channel].fftInputArrayIndex++;
        }
    }
}

//==============================================================================
bool FftTestAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FftTestAudioProcessor::createEditor()
{
    return new FftTestAudioProcessorEditor (*this);
}

//==============================================================================
void FftTestAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FftTestAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

float FftTestAudioProcessor::getAmplitudeAtFreq (int frequency, int channel) const
{
    return std::abs(freqBuffers[channel].fftOutputData[std::floor(float(frequency) / (getSampleRate() / float(fftSize)))]);
}

int FftTestAudioProcessor::getFreqWithHighestAmpl (int channel) const
{
    //Finds the element with the highest freq
    auto maxBin = std::max_element(freqBuffers[channel].fftOutputData.begin(), freqBuffers[channel].fftOutputData.end(), [](const std::complex<float>& first, const std::complex<float>& second)
    {
        return abs(first) < abs(second);
    });
    
    float binNum = (freqBuffers[channel].fftOutputData.begin() - maxBin) * -1;
    
    float centFreq = (binNum * getSampleRate()) / float(fftSize);
    
    return floor(centFreq);
}

void FftTestAudioProcessor::numChannelsChanged()
{
    freqBuffers.resize(getTotalNumOutputChannels());
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FftTestAudioProcessor();
}
