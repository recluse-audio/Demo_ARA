/*
  ==============================================================================

    DemoRegionSequenceView.cpp
    Created: 1 Nov 2022 12:48:09pm
    Author:  Ryan Devens

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DemoRegionSequenceView.h"

//==============================================================================
DemoRegionSequenceView::DemoRegionSequenceView(ARARegionSequence& rs, WaveformCache& cache, double pixelPerSec)
: regionSequence (rs), waveformCache (cache), zoomLevelPixelPerSecond (pixelPerSec)
{
	regionSequence.addListener (this);

	for (auto* playbackRegion : regionSequence.getPlaybackRegions())
		createAndAddPlaybackRegionView (playbackRegion);

	updatePlaybackDuration();
}

DemoRegionSequenceView::~DemoRegionSequenceView()
{
	regionSequence.removeListener (this);

	for (const auto& it : playbackRegionViews)
		it.first->removeListener (this);
}



//==============================================================================
// ARA Document change callback overrides
void DemoRegionSequenceView::willRemovePlaybackRegionFromRegionSequence (juce::ARARegionSequence* regionSequence, juce::ARAPlaybackRegion* playbackRegion)
{
	playbackRegion->removeListener (this);
	removeChildComponent (playbackRegionViews[playbackRegion].get());
	playbackRegionViews.erase (playbackRegion);
	updatePlaybackDuration();
}

void DemoRegionSequenceView::didAddPlaybackRegionToRegionSequence (juce::ARARegionSequence* regionSequence, juce::ARAPlaybackRegion* playbackRegion)
{
	createAndAddPlaybackRegionView (playbackRegion);
	updatePlaybackDuration();
}

void DemoRegionSequenceView::willDestroyPlaybackRegion (ARAPlaybackRegion* playbackRegion)
{
	playbackRegion->removeListener (this);
	removeChildComponent (playbackRegionViews[playbackRegion].get());
	playbackRegionViews.erase (playbackRegion);
	updatePlaybackDuration();
}

void DemoRegionSequenceView::willUpdatePlaybackRegionProperties (juce::ARAPlaybackRegion* playbackRegion, juce::ARAPlaybackRegion::PropertiesPtr regionProperties)
{
}

void DemoRegionSequenceView::didUpdatePlaybackRegionProperties (juce::ARAPlaybackRegion*)
{
	updatePlaybackDuration();
}

void DemoRegionSequenceView::resized()
{
	for (auto& pbr : playbackRegionViews)
	{
		const auto playbackRegion = pbr.first;
		pbr.second->setBounds (
			getLocalBounds()
				.withTrimmedLeft (roundToInt (playbackRegion->getStartInPlaybackTime() * zoomLevelPixelPerSecond))
				.withWidth (roundToInt (playbackRegion->getDurationInPlaybackTime() * zoomLevelPixelPerSecond)));
	}
}

double DemoRegionSequenceView::getPlaybackDuration() const noexcept
{
	return playbackDuration;
}

void DemoRegionSequenceView::setZoomLevel (double pixelPerSecond)
{
	zoomLevelPixelPerSecond = pixelPerSecond;
	resized();
}




//========================
// PRIVATE FUNCTIONS

void DemoRegionSequenceView::createAndAddPlaybackRegionView (ARAPlaybackRegion* playbackRegion)
{
	playbackRegionViews[playbackRegion] = std::make_unique<DemoPlaybackRegionView> (*playbackRegion, waveformCache);
	playbackRegion->addListener (this);
	addAndMakeVisible (*playbackRegionViews[playbackRegion]);
}


void DemoRegionSequenceView::updatePlaybackDuration()
{
	const auto iter = std::max_element (
		playbackRegionViews.begin(),
		playbackRegionViews.end(),
		[] (const auto& a, const auto& b) { return a.first->getEndInPlaybackTime() < b.first->getEndInPlaybackTime(); });

	playbackDuration = iter != playbackRegionViews.end() ? iter->first->getEndInPlaybackTime() : 0.0;

	sendChangeMessage();
}



