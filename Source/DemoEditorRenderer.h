/*
  ==============================================================================

    PluginARAEditorRenderer.h
    Created: 1 Nov 2022 12:46:25pm
    Author:  Ryan Devens

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "UtilObjects.h"
#include "Looper.h"
#include "OptionalRange.h"

class DemoEditorRenderer  : public ARAEditorRenderer,
						private ARARegionSequence::Listener
{
public:
	DemoEditorRenderer (ARA::PlugIn::DocumentController* documentController, const PreviewState* previewStateIn);

	~DemoEditorRenderer() override;

	void didAddPlaybackRegionToRegionSequence (ARARegionSequence*, ARAPlaybackRegion*) override;

	void didAddRegionSequence (ARA::PlugIn::RegionSequence* rs) noexcept override;

	void didAddPlaybackRegion (ARA::PlugIn::PlaybackRegion*) noexcept override;

	/*  An ARA host could be using either the `addPlaybackRegion()` or `addRegionSequence()` interface
		so we need to check the other side of both.

		The callback must have a signature of `bool (ARAPlaybackRegion*)`
	*/
	template <typename Callback>
	void forEachPlaybackRegion (Callback&& cb)
	{
		for (const auto& playbackRegion : getPlaybackRegions())
			if (! cb (playbackRegion))
				return;

		for (const auto& regionSequence : getRegionSequences())
			for (const auto& playbackRegion : regionSequence->getPlaybackRegions())
				if (! cb (playbackRegion))
					return;
	}

	void prepareToPlay (double sampleRateIn,
						int maximumExpectedSamplesPerBlock,
						int numChannels,
						AudioProcessor::ProcessingPrecision,
						AlwaysNonRealtime alwaysNonRealtime) override;

	void releaseResources() override;

	void reset() override;

	bool processBlock (AudioBuffer<float>& buffer,
					   AudioProcessor::Realtime realtime,
					   const AudioPlayHead::PositionInfo& positionInfo) noexcept override;

	using ARAEditorRenderer::processBlock;

private:
	void configure();
	AsyncConfigurationCallback asyncConfigCallback { [this] { configure(); } };

	const PreviewState* previewState = nullptr;
	double lastPreviewTime = 0.0;
	ARAPlaybackRegion* lastPlaybackRegion = nullptr;
	std::unique_ptr<AudioBuffer<float>> previewBuffer;
	Looper previewLooper;

	double sampleRate = 48000.0;
	SharedResourcePointer<SharedTimeSliceThread> timeSliceThread;
	std::map<ARA::PlugIn::AudioSource*, std::unique_ptr<BufferingAudioReader>> audioSourceReaders;

	std::set<ARARegionSequence*> regionSequences;
};
