// Copyright Epic Games, Inc. All Rights Reserved.

#include "EditorMineSweeperCommands.h"

#define LOCTEXT_NAMESPACE "FEditorMineSweeperModule"

void FEditorMineSweeperCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "EditorMineSweeper", "Bring up EditorMineSweeper window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
