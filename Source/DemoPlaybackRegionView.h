/*
  ==============================================================================

    DemoPlaybackRegionView.h
    Created: 1 Nov 2022 12:47:47pm
    Author:  Ryan Devens

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "WaveformCache.h"

//==============================================================================
/*
*/
class DemoPlaybackRegionView : public juce::Component,
public juce::ChangeListener
{
public:
	DemoPlaybackRegionView (juce::ARAPlaybackRegion& region, WaveformCache& cache);

	~DemoPlaybackRegionView() override;

	void mouseDown (const juce::MouseEvent& m) override;

	void mouseUp (const juce::MouseEvent&) override;

	void changeListenerCallback (juce::ChangeBroadcaster*) override;

	void paint (juce::Graphics& g) override;

	void resized() override;

private:
    juce::ARAPlaybackRegion& playbackRegion;
    WaveformCache& waveformCache;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DemoPlaybackRegionView);
};
