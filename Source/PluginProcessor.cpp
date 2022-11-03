/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DemoARAAudioProcessor::DemoARAAudioProcessor()
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

DemoARAAudioProcessor::~DemoARAAudioProcessor()
{
}

//==============================================================================
const juce::String DemoARAAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DemoARAAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DemoARAAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DemoARAAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DemoARAAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DemoARAAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DemoARAAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DemoARAAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DemoARAAudioProcessor::getProgramName (int index)
{
    return {};
}

void DemoARAAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DemoARAAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	playHeadState.isPlaying.store (false);
	prepareToPlayForARA (sampleRate, samplesPerBlock, getMainBusNumOutputChannels(), getProcessingPrecision());
}

void DemoARAAudioProcessor::releaseResources()
{
	playHeadState.isPlaying.store (false);
	releaseResourcesForARA();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DemoARAAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
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

void DemoARAAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	ignoreUnused (midiMessages);

	ScopedNoDenormals noDenormals;

	auto* audioPlayHead = getPlayHead();
	playHeadState.update (audioPlayHead);

	if (! processBlockForARA (buffer, isRealtime(), audioPlayHead))
		processBlockBypassed (buffer, midiMessages);
}

//==============================================================================
bool DemoARAAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DemoARAAudioProcessor::createEditor()
{
    return new DemoARAAudioProcessorEditor (*this);
}

//==============================================================================
void DemoARAAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DemoARAAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DemoARAAudioProcessor();
}
