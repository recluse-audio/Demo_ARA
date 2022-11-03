/*
  ==============================================================================

    DemoRegionSequenceView.h
    Created: 1 Nov 2022 12:48:09pm
    Author:  Ryan Devens

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "WaveformCache.h"
#include "DemoPlaybackRegionView.h"

//==============================================================================
/*
*/
class DemoRegionSequenceView : public Component,
                           public ARARegionSequence::Listener,
                           public ChangeBroadcaster,
                           private ARAPlaybackRegion::Listener
{
public:
	DemoRegionSequenceView (ARARegionSequence& rs, WaveformCache& cache, double pixelPerSec);

	~DemoRegionSequenceView() override;

    //==============================================================================
    // ARA Document change callback overrides
    void willRemovePlaybackRegionFromRegionSequence (ARARegionSequence*,
													 ARAPlaybackRegion* playbackRegion) override;

	void didAddPlaybackRegionToRegionSequence (ARARegionSequence*, ARAPlaybackRegion* playbackRegion) override;

	void willDestroyPlaybackRegion (ARAPlaybackRegion* playbackRegion) override;

	void willUpdatePlaybackRegionProperties (ARAPlaybackRegion*, ARAPlaybackRegion::PropertiesPtr) override;

	void didUpdatePlaybackRegionProperties (ARAPlaybackRegion*) override;

	void resized() override;

	double getPlaybackDuration() const noexcept;
	void setZoomLevel (double pixelPerSecond);

private:
	void createAndAddPlaybackRegionView (ARAPlaybackRegion* playbackRegion);

	void updatePlaybackDuration();

    juce::ARARegionSequence& regionSequence;
    WaveformCache& waveformCache;
    std::unordered_map<juce::ARAPlaybackRegion*, std::unique_ptr<DemoPlaybackRegionView>> playbackRegionViews;
    double playbackDuration = 0.0;
    double zoomLevelPixelPerSecond;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DemoRegionSequenceView)
};
