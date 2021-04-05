// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "PropertyHandle.h"
#include "Input/Reply.h"
#include "IPropertyTypeCustomization.h"
#include "PluginLibrary.h"
#include "Misc/Optional.h"
#include "EditorMineSweeperStyle.h"
#include "EditorMineSweeperCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "ToolMenus.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SBox.h"
#include "Misc/MessageDialog.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Input/SSpinBox.h"
#include "Math/UnrealMathUtility.h"
#include "Containers/Array.h"
#include "Widgets/SOverlay.h"

DECLARE_LOG_CATEGORY_EXTERN( LogMain, Log, All );

class FEditorMineSweeperModule : public IModuleInterface
{
public:
	int32 GridSize=3;
	
	int32 NumberOfMines=3;
	TArray<FString> real_table;
	TArray<FString> vis_table;
	
	
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;


	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();
	void InitialzeGrid();
	void ResetValues();
	void placeMines();
	void calculateNumbers();
	TArray<int32> getneighbors ( int32 pos, int32 n );
	FText GetTableValueAt(int32 i);
	bool isRealNeighbor ( int32 index, int32 pos, int32 n );
	FString countMines (TArray<int32> neighbors, int32 pos, int32 n );
	void openSafe ( int32 n, int32 pos );


	FReply OnGenerateGridClicked();
	FReply OnMineFieldButtonClicked(int32 i);
	void StoreWidthValue( float NewValue );

	TOptional<int32> GetCurrentWidth() const { return GridSize; }
private:

	void RegisterMenus();

	TSharedRef<class SDockTab> OnSpawnPluginTab( const class FSpawnTabArgs& SpawnTabArgs );
	TSharedRef<class SDockTab> MineSeeperUI();

private:
	TSharedPtr<class FUICommandList> PluginCommands;
	TSharedPtr<SSpinBox<float>> WidthTextBoxPtr;
	TSharedPtr<SSpinBox<float>> NumberOfMinesTextBoxPtr;
	TSharedPtr<SButton> GenerateGridButtonPtr;

	FSlateFontInfo XXLargeLayoutFont = FCoreStyle::GetDefaultFontStyle( "Regular", 42 );
	FSlateFontInfo XLargeLayoutFont = FCoreStyle::GetDefaultFontStyle( "Regular", 24 );
	FSlateFontInfo LargeLayoutFont = FCoreStyle::GetDefaultFontStyle( "Regular", 16 );
	FSlateFontInfo SmallLayoutFont = FCoreStyle::GetDefaultFontStyle( "Regular", 10 );

	bool showMineField=false;
	bool GameOver=false;
	bool DidWin=false;


	//Game Logic Variables
	int32 col;
	int32 Score = 0;
	FString curr_symbol;
	int32 Position;
	int32 row;	
	int32 safe;
};
