/*
  ==============================================================================

    DemoPlaybackRegionView.cpp
    Created: 1 Nov 2022 12:47:47pm
    Author:  Ryan Devens

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DemoPlaybackRegionView.h"
#include "DemoDocumentControllerSpecialisation.h"


//==============================================================================
DemoPlaybackRegionView::DemoPlaybackRegionView(ARAPlaybackRegion& region, WaveformCache& cache)
: playbackRegion (region), waveformCache (cache)
{
	auto* audioSource = playbackRegion.getAudioModification()->getAudioSource();

	waveformCache.getOrCreateThumbnail (audioSource).addChangeListener (this);
}

DemoPlaybackRegionView::~DemoPlaybackRegionView()
{
	waveformCache.getOrCreateThumbnail (playbackRegion.getAudioModification()->getAudioSource())
		.removeChangeListener (this);
}

void DemoPlaybackRegionView::mouseDown (const MouseEvent& m)
{
	const auto relativeTime = (double) m.getMouseDownX() / getLocalBounds().getWidth();
	const auto previewTime = playbackRegion.getStartInPlaybackTime()
							 + relativeTime * playbackRegion.getDurationInPlaybackTime();
	auto& previewState = ARADocumentControllerSpecialisation::getSpecialisedDocumentController<DemoDocumentControllerSpecialisation> (playbackRegion.getDocumentController())->previewState;
	previewState.previewTime.store (previewTime);
	previewState.previewedRegion.store (&playbackRegion);
}

void DemoPlaybackRegionView::mouseUp (const MouseEvent&)
{
	auto& previewState = ARADocumentControllerSpecialisation::getSpecialisedDocumentController<DemoDocumentControllerSpecialisation> (playbackRegion.getDocumentController())->previewState;
	previewState.previewTime.store (0.0);
	previewState.previewedRegion.store (nullptr);
}

void DemoPlaybackRegionView::changeListenerCallback(ChangeBroadcaster* changeBroadcaster)
{
	repaint();
}

void DemoPlaybackRegionView::paint (juce::Graphics& g)
{
	g.fillAll (Colours::white.darker());
	g.setColour (Colours::darkgrey.darker());
	auto& thumbnail = waveformCache.getOrCreateThumbnail (playbackRegion.getAudioModification()->getAudioSource());
	thumbnail.drawChannels (g,
							getLocalBounds(),
							playbackRegion.getStartInAudioModificationTime(),
							playbackRegion.getEndInAudioModificationTime(),
							1.0f);
	g.setColour (Colours::black);
	g.drawRect (getLocalBounds());
}

void DemoPlaybackRegionView::resized()
{
	repaint();
}
