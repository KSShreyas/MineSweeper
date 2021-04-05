// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "EditorMineSweeperStyle.h"

class FEditorMineSweeperCommands : public TCommands<FEditorMineSweeperCommands>
{
public:

	FEditorMineSweeperCommands()
		: TCommands<FEditorMineSweeperCommands>(TEXT("EditorMineSweeper"), NSLOCTEXT("Contexts", "EditorMineSweeper", "EditorMineSweeper Plugin"), NAME_None, FEditorMineSweeperStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};