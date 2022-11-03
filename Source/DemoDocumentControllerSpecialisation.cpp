/*
  ==============================================================================

	DemoDocumentController.cpp
    Created: 2 Nov 2022 10:55:15am
    Author:  Ryan Devens

  ==============================================================================
*/

#include "DemoDocumentControllerSpecialisation.h"

DemoPlaybackRenderer* DemoDocumentControllerSpecialisation::doCreatePlaybackRenderer() noexcept
{
	return new DemoPlaybackRenderer (getDocumentController());
}

DemoEditorRenderer* DemoDocumentControllerSpecialisation::doCreateEditorRenderer() noexcept
{
	return new DemoEditorRenderer (getDocumentController(), &previewState);
}

bool DemoDocumentControllerSpecialisation::doRestoreObjectsFromStream (ARAInputStream& input,
								 const ARARestoreObjectsFilter* filter) noexcept
{
	ignoreUnused (input, filter);
	return false;
}

bool DemoDocumentControllerSpecialisation::doStoreObjectsToStream (ARAOutputStream& output, const ARAStoreObjectsFilter* filter) noexcept
{
	ignoreUnused (output, filter);
	return false;
}

//==============================================================================
// This creates the static ARAFactory instances for the plugin.
const ARA::ARAFactory* JUCE_CALLTYPE createARAFactory()
{
	return juce::ARADocumentControllerSpecialisation::createARAFactory<DemoDocumentControllerSpecialisation>();
}
