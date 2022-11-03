/*
  ==============================================================================

    Looper.cpp
    Created: 1 Nov 2022 12:44:11pm
    Author:  Ryan Devens

  ==============================================================================
*/

#include "Looper.h"

Looper::Looper()
	: inputBuffer (nullptr), pos (loopRange.getStart())
{
}

Looper::Looper (const juce::AudioBuffer<float>* buffer, juce::Range<juce::int64> range)
	: inputBuffer (buffer), loopRange (range), pos (range.getStart())
{
}

void Looper::writeInto (juce::AudioBuffer<float>& buffer)
{
	if (loopRange.getLength() == 0)
		buffer.clear();

	const auto numChannelsToCopy = std::min (inputBuffer->getNumChannels(), buffer.getNumChannels());

	for (auto samplesCopied = 0; samplesCopied < buffer.getNumSamples();)
	{
		const auto numSamplesToCopy =
			std::min (buffer.getNumSamples() - samplesCopied, (int) (loopRange.getEnd() - pos));

		for (int i = 0; i < numChannelsToCopy; ++i)
		{
			buffer.copyFrom (i, samplesCopied, *inputBuffer, i, (int) pos, numSamplesToCopy);
		}

		samplesCopied += numSamplesToCopy;
		pos += numSamplesToCopy;

		jassert (pos <= loopRange.getEnd());

		if (pos == loopRange.getEnd())
			pos = loopRange.getStart();
	}
}
