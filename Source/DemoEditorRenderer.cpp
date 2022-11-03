/*
  ==============================================================================

    PluginARAEditorRenderer.cpp
    Created: 1 Nov 2022 12:46:25pm
    Author:  Ryan Devens

  ==============================================================================
*/

#include "DemoEditorRenderer.h"

DemoEditorRenderer::DemoEditorRenderer (ARA::PlugIn::DocumentController* documentController, const PreviewState* previewStateIn)
	: juce::ARAEditorRenderer (documentController), previewState (previewStateIn), previewBuffer()
{
	jassert (previewState != nullptr);
}

DemoEditorRenderer::~DemoEditorRenderer()
{
	for (const auto& rs : regionSequences)
		rs->removeListener (this);
}

void DemoEditorRenderer::didAddPlaybackRegionToRegionSequence (juce::ARARegionSequence*, juce::ARAPlaybackRegion*)
{
	asyncConfigCallback.startConfigure();
}

void DemoEditorRenderer::didAddRegionSequence (ARA::PlugIn::RegionSequence* rs) noexcept
{
	auto* sequence = dynamic_cast<ARARegionSequence*> (rs);
	sequence->addListener (this);
	regionSequences.insert (sequence);
	asyncConfigCallback.startConfigure();
}

void DemoEditorRenderer::didAddPlaybackRegion (ARA::PlugIn::PlaybackRegion*) noexcept
{
	asyncConfigCallback.startConfigure();
}

void DemoEditorRenderer::prepareToPlay (double sampleRateIn,
					int maximumExpectedSamplesPerBlock,
					int numChannels,
					AudioProcessor::ProcessingPrecision,
					AlwaysNonRealtime alwaysNonRealtime)
{
	sampleRate = sampleRateIn;
	previewBuffer = std::make_unique<AudioBuffer<float>> (numChannels, (int) (2 * sampleRateIn));

	ignoreUnused (maximumExpectedSamplesPerBlock, alwaysNonRealtime);
}


void DemoEditorRenderer::releaseResources()
{
	audioSourceReaders.clear();
}

void DemoEditorRenderer::reset()
{
	previewBuffer->clear();
}

bool DemoEditorRenderer::processBlock (AudioBuffer<float>& buffer,
				   AudioProcessor::Realtime realtime,
				   const AudioPlayHead::PositionInfo& positionInfo) noexcept
{
	ignoreUnused (realtime);

	return asyncConfigCallback.withLock ([&] (bool locked)
	{
		if (! locked)
			return true;

		if (positionInfo.getIsPlaying())
			return true;

		if (const auto previewedRegion = previewState->previewedRegion.load())
		{
			const auto regionIsAssignedToEditor = [&]()
			{
				bool regionIsAssigned = false;

				forEachPlaybackRegion([&previewedRegion, &regionIsAssigned] (const auto& region)
				{
					if (region == previewedRegion)
					{
						regionIsAssigned = true;
						return false;
					}

					return true;
				});

				return regionIsAssigned;
			}();

			if (regionIsAssignedToEditor)
			{
				const auto previewTime = previewState->previewTime.load();

				if (lastPreviewTime != previewTime || lastPlaybackRegion != previewedRegion)
				{
					Range<double> previewRangeInPlaybackTime { previewTime - 1.25, previewTime + 1.25 };
					
					previewBuffer->clear();
					
					auto sourceReaderFunction = [this] (auto* source) -> auto*
					{
					   const auto iter = audioSourceReaders.find (source);
					   return iter != audioSourceReaders.end() ? iter->second.get() : nullptr;
					};
					
					const auto rangeInOutput = readPlaybackRangeIntoBuffer (previewRangeInPlaybackTime, previewedRegion, *previewBuffer, sourceReaderFunction);

					if (rangeInOutput)
					{
						lastPreviewTime = previewTime;
						lastPlaybackRegion = previewedRegion;
						previewLooper = Looper (previewBuffer.get(), *rangeInOutput);
					}
				}
				else
				{
					previewLooper.writeInto (buffer);
				}
			}
		}

		return true;
	});
}


//=========================
// PRIVATE FUNCTIONS

void DemoEditorRenderer::configure()
{
	forEachPlaybackRegion ([this, maximumExpectedSamplesPerBlock = 1000] (const auto& playbackRegion)
	{
		const auto audioSource = playbackRegion->getAudioModification()->getAudioSource();

		if (audioSourceReaders.find (audioSource) == audioSourceReaders.end())
		{
			auto sourceReader = new ARAAudioSourceReader (playbackRegion->getAudioModification()->getAudioSource());
			
			int samplesToBuffer = (int) (sampleRate * 4.0); //std::max (4 * maximumExpectedSamplesPerBlock, (int) sampleRate);
			
			audioSourceReaders[audioSource] = std::make_unique<BufferingAudioReader> ( sourceReader, *timeSliceThread, samplesToBuffer);
		}

		return true;
	});
}
