/*
  ==============================================================================

    Looper.h
    Created: 1 Nov 2022 12:44:11pm
    Author:  Ryan Devens

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Looper
{
public:
	Looper();
	Looper (const juce::AudioBuffer<float>* buffer, juce::Range<juce::int64> range);
	
	void writeInto (juce::AudioBuffer<float>& buffer);

private:
    const juce::AudioBuffer<float>* inputBuffer;
	juce::Range<juce::int64> loopRange;
	juce::int64 pos;
};
