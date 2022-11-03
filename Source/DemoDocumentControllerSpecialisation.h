/*
  ==============================================================================

    ARADemoPluginDocumentControllerSpecialisation.h
    Created: 2 Nov 2022 10:52:41am
    Author:  Ryan Devens

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "DemoEditorRenderer.h"
#include "DemoPlaybackRenderer.h"

class DemoDocumentControllerSpecialisation  : public ARADocumentControllerSpecialisation
{
public:
	using ARADocumentControllerSpecialisation::ARADocumentControllerSpecialisation;

	PreviewState previewState;

protected:
	DemoPlaybackRenderer* doCreatePlaybackRenderer() noexcept override;

	DemoEditorRenderer* doCreateEditorRenderer() noexcept override;

	bool doRestoreObjectsFromStream (ARAInputStream& input,
									 const ARARestoreObjectsFilter* filter) noexcept override;

	bool doStoreObjectsToStream (ARAOutputStream& output, const ARAStoreObjectsFilter* filter) noexcept override;
};
