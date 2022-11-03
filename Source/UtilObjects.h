/*
  ==============================================================================

    UtilObjects.h
    Created: 1 Nov 2022 12:43:59pm
    Author:  Ryan Devens

  ==============================================================================
*/

#pragma once

//==============================================================================
struct PreviewState
{
    std::atomic<double> previewTime { 0.0 };
    std::atomic<juce::ARAPlaybackRegion*> previewedRegion { nullptr };
};

class SharedTimeSliceThread  : public juce::TimeSliceThread
{
public:
    SharedTimeSliceThread()
        : TimeSliceThread (String (JucePlugin_Name) + " ARA Sample Reading Thread")
    {
        startThread (7);  // Above default priority so playback is fluent, but below realtime
    }
};

class AsyncConfigurationCallback  : private juce::AsyncUpdater
{
public:
    explicit AsyncConfigurationCallback (std::function<void()> callbackIn)
        : callback (std::move (callbackIn)) {}

    ~AsyncConfigurationCallback() override { cancelPendingUpdate(); }

    template <typename RequiresLock>
    auto withLock (RequiresLock&& fn)
    {
        const juce::SpinLock::ScopedTryLockType scope (processingFlag);
        return fn (scope.isLocked());
    }

    void startConfigure() { triggerAsyncUpdate(); }

private:
    void handleAsyncUpdate() override
    {
        const juce::SpinLock::ScopedLockType scope (processingFlag);
        callback();
    }

    std::function<void()> callback;
	juce::SpinLock processingFlag;
};

struct PlayHeadState
{
    void update (juce::AudioPlayHead* aph)
    {
        const auto info = aph->getPosition();

        if (info.hasValue() && info->getIsPlaying())
        {
            isPlaying.store (true);
            timeInSeconds.store (info->getTimeInSeconds().orFallback (0));
        }
        else
        {
            isPlaying.store (false);
        }
    }

    std::atomic<bool>   isPlaying     { false };
    std::atomic<double> timeInSeconds { 0.0 };
};
