/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DemoDocumentView.h"

//==============================================================================
DemoARAAudioProcessorEditor::DemoARAAudioProcessorEditor (DemoARAAudioProcessor& p)
    : AudioProcessorEditor (&p), AudioProcessorEditorARAExtension(&p), audioProcessor (p)
{
	if (auto* editorView = getARAEditorView())
	{
		auto* document = ARADocumentControllerSpecialisation::getSpecialisedDocumentController(editorView->getDocumentController())->getDocument();
		documentView = std::make_unique<DemoDocumentView> (*document, p.playHeadState );
	}

	addAndMakeVisible (documentView.get());

	// ARA requires that plugin editors are resizable to support tight integration
	// into the host UI
	setResizable (true, false);
	setSize (400, 300);
}

DemoARAAudioProcessorEditor::~DemoARAAudioProcessorEditor()
{
}

//==============================================================================
void DemoARAAudioProcessorEditor::paint (juce::Graphics& g)
{
	g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

	if (! isARAEditorView())
	{
		g.setColour (Colours::white);
		g.setFont (15.0f);
		g.drawFittedText ("ARA host isn't detected. This plugin only supports ARA mode",
						  getLocalBounds(),
						  Justification::centred,
						  1);
	}
}

void DemoARAAudioProcessorEditor::resized()
{
	if (documentView != nullptr)
		documentView->setBounds (getLocalBounds());
}
