/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class DemoARAAudioProcessorEditor  : public juce::AudioProcessorEditor,
									  public AudioProcessorEditorARAExtension
{
public:
    DemoARAAudioProcessorEditor (DemoARAAudioProcessor&);
    ~DemoARAAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
	std::unique_ptr<Component> documentView;

    DemoARAAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DemoARAAudioProcessorEditor)
};
