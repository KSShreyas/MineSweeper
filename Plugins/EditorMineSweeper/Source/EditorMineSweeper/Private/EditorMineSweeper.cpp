#include "EditorMineSweeper.h"


DEFINE_LOG_CATEGORY( LogMain );

static const FName EditorMineSweeperTabName( "EditorMineSweeper" );

#define LOCTEXT_NAMESPACE "FEditorMineSweeperModule"


#pragma region CUSTOM_EXPERIMENTAL_MACROS
/*Reccursive Macro Calling Experiemnt*/
#define STRFY(A) #A
#define MakeGrid(N) 

#define LOOP_N_TIMES(N, CODE)	CODE
#define LOOP_1_TIMES(N, CODE)  CODE
#define LOOP_2_TIMES(N, CODE)  CODE CODE
#define LOOP_3_TIMES(N, CODE)  CODE CODE  CODE
#define LOOP_4_TIMES(N, CODE)  CODE CODE  CODE CODE

#define M_REPEAT_(N, X) M_REPEAT ## N(X) 
#define M_REPEAT(N, X) M_REPEAT_(N, X)

#define Grid(n)													\
	SNew( SVerticalBox )										\
	LOOP_N_TIMES(n,+SVerticalBox::Slot().FillHeight( 1 )[SNew( SHorizontalBox )	LOOP_N_TIMES(n,MineSweeperButton_Macro(0))])	\

//LOOP_N_TIMES(INNER_LOOP_COUNT,MineSweeperButton_Macro(0))
#pragma endregion CUSTOM_EXPERIMENTAL_MACROS


#pragma region CUSTOM_MACROS

#define MineSweeperButton_Macro(i) \
	+SHorizontalBox::Slot()	.FillWidth( 1 ) \
	[\
			SNew(SBox)\
			.HeightOverride_Lambda([this]()->float {return  128*9/GridSize;})\
			.Visibility_Lambda([this]()->EVisibility {return /*(i%9)<GridSize*/ (i%9)<GridSize && (i-((9-GridSize) * FMath::Floor(i/9) ) <GridSize*GridSize) ? EVisibility::Visible : EVisibility::Collapsed  ;})\
			[\
				SNew( SButton ).Text( LOCTEXT( "0-0", "O" ) ).HAlign( HAlign_Center ) \
				.OnClicked( FOnClicked::CreateRaw( this,&FEditorMineSweeperModule::OnMineFieldButtonClicked,i )).VAlign( VAlign_Center )	\
				.IsEnabled_Lambda([this]()->bool {return (i%9)<GridSize && (i-((9-GridSize) * FMath::Floor(i/9) ) <GridSize*GridSize);})		\
				.ButtonColorAndOpacity_Lambda([this]()-> FLinearColor {	\
					if(  Visible_Table[i-((9-GridSize) * FMath::Floor(i/9) )].Equals("-")){ return (FLinearColor(1.f, 1.f, 1.f, 1.f));}else if(  Visible_Table[i-((9-GridSize) * FMath::Floor(i/9) )].Equals(" ")){ return (FLinearColor(1.f, 1.f, 1.f, 0.5f));}else	if(  Visible_Table[i-((9-GridSize) * FMath::Floor(i/9) )].Equals("*")){ return (FLinearColor(1.f, 0.f, 0.f, 0.35f));} else { return (FLinearColor(1.f, 1.f, 1.f, 1.f));}	})		\
					[\
					SNew( SBorder ).Content()\
						[\
							SNew( STextBlock ) \
							/*.ColorAndOpacity( FLinearColor(0.0f, 1.0f, 0.0f))*/	\
							.Text_Lambda([this]()->FText { return  GetTableValueAt(i-((9-GridSize) * FMath::Floor(i/9) ))	\
							/*FText::AsNumber(i)*/ /*FText::AsNumber(i-((9-GridSize) * FMath::Floor(i/9) ))*/;})\
							.ColorAndOpacity_Lambda([this]()-> FSlateColor { if(  Visible_Table[i-((9-GridSize) * FMath::Floor(i/9) )].Equals("6")){return FSlateColor(FLinearColor(1.0f, 0.f, 0.f, 1.f));}else if(  Visible_Table[i-((9-GridSize) * FMath::Floor(i/9) )].Equals("5")){return FSlateColor(FLinearColor(0.9f, 0.f, 0.f, 1.f));}if(  Visible_Table[i-((9-GridSize) * FMath::Floor(i/9) )].Equals("4")){return FSlateColor(FLinearColor(0.8f, 0.f, 0.f, 1.f));} else if(  Visible_Table[i-((9-GridSize) * FMath::Floor(i/9) )].Equals("3")){return FSlateColor(FLinearColor(0.7f, 0.f, 0.f, 1.f));} else if(  Visible_Table[i-((9-GridSize) * FMath::Floor(i/9) )].Equals("2")){return FSlateColor(FLinearColor(0.6f, 0.f, 0.f, 1.f));} else if(  Visible_Table[i-((9-GridSize) * FMath::Floor(i/9) )].Equals("*")){return FSlateColor(FLinearColor(1.0f, 0.f, 0.f, 0.35f));}else if(  Visible_Table[i-((9-GridSize) * FMath::Floor(i/9) )].Equals(" ")){return FSlateColor(FLinearColor(1.0f,1.f, 1.f, 0.5f));} else {return FSlateColor(FLinearColor(0.f, 0.f, 0.f, 1.f));}}  )	\
							.Font( LargeLayoutFont )	\
						]\
					]\
			]\
	]\

#pragma endregion CUSTOM_MACROS


#pragma region Plugin_Startup_Stuff


void FEditorMineSweeperModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FEditorMineSweeperStyle::Initialize();
	FEditorMineSweeperStyle::ReloadTextures();

	FEditorMineSweeperCommands::Register();

	PluginCommands = MakeShareable( new FUICommandList );

	PluginCommands->MapAction(
		FEditorMineSweeperCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw( this, &FEditorMineSweeperModule::PluginButtonClicked ),
		FCanExecuteAction() );

	UToolMenus::RegisterStartupCallback( FSimpleMulticastDelegate::FDelegate::CreateRaw( this, &FEditorMineSweeperModule::RegisterMenus ) );

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner( EditorMineSweeperTabName, FOnSpawnTab::CreateRaw( this, &FEditorMineSweeperModule::OnSpawnPluginTab ) )
		.SetDisplayName( LOCTEXT( "FEditorMineSweeperTabTitle", "EditorMineSweeper" ) )
		.SetMenuType( ETabSpawnerMenuType::Hidden );
}

void FEditorMineSweeperModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback( this );

	UToolMenus::UnregisterOwner( this );

	FEditorMineSweeperStyle::Shutdown();

	FEditorMineSweeperCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner( EditorMineSweeperTabName );
}

void FEditorMineSweeperModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab( EditorMineSweeperTabName );
}

void FEditorMineSweeperModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped( this );
	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu( "LevelEditor.MainMenu.Window" );
		{
			FToolMenuSection& Section = Menu->FindOrAddSection( "WindowLayout" );
			Section.AddMenuEntryWithCommandList( FEditorMineSweeperCommands::Get().OpenPluginWindow, PluginCommands );
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu( "LevelEditor.LevelEditorToolBar" );
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection( "Settings" );
			{
				FToolMenuEntry& Entry = Section.AddEntry( FToolMenuEntry::InitToolBarButton( FEditorMineSweeperCommands::Get().OpenPluginWindow ) );
				Entry.SetCommandList( PluginCommands );
			}
		}
	}
}


#pragma endregion Plugin_Startup_Stuff


#pragma region Plugin_MineSweeper_UI

TSharedRef<SDockTab> FEditorMineSweeperModule::OnSpawnPluginTab( const FSpawnTabArgs& SpawnTabArgs )
{
	return MineSeeperUI();
}

TSharedRef<SDockTab> FEditorMineSweeperModule::MineSeeperUI()
{
	FText WidgetText = FText::Format(
		LOCTEXT( "WindowWidgetText", "Add code to {0} in {1} to override this window's contents" ),
		FText::FromString( TEXT( "FEditorMineSweeperModule::OnSpawnPluginTab" ) ),
		FText::FromString( TEXT( "EditorMineSweeper.cpp" ) )
	);


	const FVector2D HeadingShadowOffset( 2, 2 );

	TSharedRef<SDockTab> UI=SNew( SDockTab )
			[
			SNew( SScrollBox )
				+SScrollBox::Slot().Padding( 5 )
					[
						SNew( STextBlock )
						.ShadowOffset( HeadingShadowOffset )
						.Font( XLargeLayoutFont )
						.Text( LOCTEXT( "Editor-Minesweeper", "Editor Minesweeper" ) )
						.ColorAndOpacity( FLinearColor(0.5f, 0.5f, 0.5f))
						/*.ColorAndOpacity_Lambda([this]()-> FSlateColor { return FSlateColor(FLinearColor(1.f, 1.f, 1.f, 1.f));})*/		
					]
				+SScrollBox::Slot().Padding( 10, 5 )
					[
						SNew(SBox)
						.HeightOverride(96)
						[							
								SAssignNew( GenerateGridButtonPtr, SButton )
								//.Text( LOCTEXT( "Generate-New-Grid", "New Game." ) )
								.OnClicked( FOnClicked::CreateRaw( this, &FEditorMineSweeperModule::OnGenerateGridClicked ) )
								//.ButtonColorAndOpacity(this, &SPrimaryAssetIdGraphPin::OnGetWidgetBackground)
								.ButtonColorAndOpacity_Lambda([this]()-> FLinearColor { return (FLinearColor(1.f, 1.f, 1.f, 1.f)); })
								//FSlateColor(FLinearColor(1.f, 1.f, 1.f, Alpha))			
								.HAlign( HAlign_Center ) .VAlign( VAlign_Center )
								[
									SNew( STextBlock )									
									.Text( LOCTEXT( "Generate-New-Grid", "New Game." ) )
									.Font( LargeLayoutFont )
								]
						]

					]
				+SScrollBox::Slot().Padding( 5 )
					[
						SNew( STextBlock ).ShadowOffset( HeadingShadowOffset ).Font( LargeLayoutFont ).Text( LOCTEXT( "Properties", "Properties" ) )
					]

				+SScrollBox::Slot().Padding( 10, 5 )
					[
						SNew( SHorizontalBox )
						+SHorizontalBox::Slot().FillWidth( 3 )
							[
								SNew( STextBlock ).Font( SmallLayoutFont ).Text( LOCTEXT( "WidthLable", "GridSize :" ) )
							]
						+SHorizontalBox::Slot().FillWidth( 3 )
							[
								SAssignNew( WidthTextBoxPtr, SSpinBox<float> )
								.MinValue( 3.0 )
								.MaxValue( 9.0 )
								.MinSliderValue( TAttribute< TOptional<float> >( 3.0f ) )
								.MaxSliderValue( TAttribute< TOptional<float> >( 9.0f ) )
								.Delta( 1.0f )
								.Value( GridSize )
								.OnValueChanged_Lambda([this](float value)-> void { GridSize=value; return; })
								//.OnValueChanged( SReferenceViewer::CreateRaw( this, &FEditorMineSweeperModule::StoreWidthValue, ) )
								.OnBeginSliderMovement_Lambda([this]()->void {showMineField=false;NumberOfMinesTextBoxPtr->SetValue(3); return;})
							]
					]
				+SScrollBox::Slot().Padding( 10, 5 )
					[
						SNew( SHorizontalBox )
						+SHorizontalBox::Slot().FillWidth( 3 )
							[
								SNew( STextBlock ).Font( SmallLayoutFont ).Text( LOCTEXT( "NoOfMinesLable", "Number Of Mines :" ) )
							]
						+SHorizontalBox::Slot().FillWidth( 3 )
							[
								SAssignNew( NumberOfMinesTextBoxPtr, SSpinBox<float> )
								.MinValue( 0.0 )
								/*.MaxValue( 9.0 )*/
								.MinSliderValue( TAttribute< TOptional<float> >( 0.0f ) )
								/*.MaxSliderValue( TAttribute< TOptional<float> >( 9.0f ) )*/
								//.MinValue_Lambda([this]()-> float {return GridSize;})
								.MaxValue_Lambda([this]()-> float {return GridSize+5;})
								//.MinSliderValue_Lambda([this]()-> float {showMineField=false; return GridSize;})
								.MaxSliderValue_Lambda([this]()-> float { return GridSize+5;})
								.Delta( 1.0f )
								.Value( NumberOfMines )
								//.OnValueChanged_Lambda([this](float value)-> void { GridSize=value; return; })
								//.OnValueChanged( SReferenceViewer::CreateRaw( this, &FEditorMineSweeperModule::StoreWidthValue, ) )
								.OnBeginSliderMovement_Lambda([this]()->void {showMineField=false; return;})
							]
					]

				+SScrollBox::Slot().Padding( 5 )
					[
						SNew( STextBlock )
						.ShadowOffset( HeadingShadowOffset )
						.Font( LargeLayoutFont ).
						Text( LOCTEXT( "Play-Here", "Play Here : " ) )
						.Visibility_Lambda([this]()->EVisibility {return showMineField && !GameOver ? EVisibility::Visible : EVisibility::Collapsed;})
					]
				+SScrollBox::Slot().Padding( 10, 5 )
					[
						SNew(SBox)			
						.Visibility_Lambda([this]()->EVisibility {return  GameOver ? EVisibility::Visible : EVisibility::Collapsed;})
						[
							SNew( SGridPanel )
							.FillColumn( 0, 100 )
							.FillColumn( 1, 100 )
							.FillColumn( 2, 100 )
							+SGridPanel::Slot( 0, 0 )
							.Padding( 0.0f, 0.0f, 5.0f, 2.0f )
							.VAlign( VAlign_Center )
							.RowSpan( 3 )
							.ColumnSpan( 3 )
							[
								SNew( SScrollBox )
								+SScrollBox::Slot().Padding( 5 )
								[
									SNew(SBox) .HAlign( HAlign_Center ) .VAlign( VAlign_Center )
									.Visibility_Lambda([this]()->EVisibility {return GameOver ? EVisibility::Visible : EVisibility::Collapsed;})
									[
										SNew( STextBlock )
										.ShadowOffset( HeadingShadowOffset )
										.Font( XXLargeLayoutFont )
										.Text( LOCTEXT( "Game-Over", "Game Over" ) )											
									]								
								]
								/*[
									SNew(SBox) .HAlign( HAlign_Center ) .VAlign( VAlign_Center )
									.Visibility_Lambda([this]()->EVisibility {return GameOver && DidWin ? EVisibility::Visible : EVisibility::Collapsed;})
									[
										SNew( STextBlock ).ShadowOffset( HeadingShadowOffset ).Font( XXLargeLayoutFont ).Text( LOCTEXT( "You-Win", "You Win!" ) )									
									]
								]*/
								[
									SNew(SBox) .HAlign( HAlign_Center ) .VAlign( VAlign_Center )
									.Visibility_Lambda([this]()->EVisibility {return GameOver? EVisibility::Visible : EVisibility::Collapsed;})
									[
									SNew( STextBlock ).ShadowOffset( HeadingShadowOffset ).Font( XXLargeLayoutFont )
									.Text_Lambda([this]()->FText { return DidWin ?  LOCTEXT( "You-Win", "You Win!" ): LOCTEXT( "You-Lose", "You Lose!" );})
									//.Text(DidWin ? LOCTEXT( "You-Win", "You Win!" ): LOCTEXT( "You-Lose", "You Lose!" ) )
									.Visibility_Lambda([this]()->EVisibility {return GameOver  ? EVisibility::Visible : EVisibility::Collapsed;})
									]
								]
								/*[
									SNew( STextBlock ).ShadowOffset( HeadingShadowOffset ).Font( XXLargeLayoutFont ).Text( LOCTEXT( "You-Win", "You Win!" ) )
									.Visibility_Lambda([this]()->EVisibility {return GameOver && DidWin  ? EVisibility::Visible : EVisibility::Collapsed;})
								]*/
							]
						]
					]

		//TODO: MAke the grid for displiyig the buttons
		+SScrollBox::Slot().Padding( 10, 5 )
			[
				SNew(SBox)
				.Visibility_Lambda([this]()->EVisibility {return showMineField  ? EVisibility::Visible : EVisibility::Collapsed;})			
				.IsEnabled_Lambda([this]()->bool {return showMineField && !GameOver;})			
				[
				SNew( SGridPanel )
				.FillColumn( 0, 100 )
				.FillColumn( 1, 100 )
				.FillColumn( 2, 100 )
				.FillColumn( 3, 100 )
				.FillColumn( 4, 100 )
				.FillColumn( 5, 100 )
				.FillColumn( 6, 100 )
				.FillColumn( 7, 100 )
				.FillColumn( 8, 100 )
				+SGridPanel::Slot( 0, 0 )
				.Padding( 0.0f, 0.0f, 5.0f, 2.0f )
				.VAlign( VAlign_Center )
				.RowSpan( 9 )
				.ColumnSpan( 9 )
					[
					SNew( SScrollBox )
					+SScrollBox::Slot().Padding( 10, 5 )
						[						
						/*Grid(GridSize)*/
						SNew( SVerticalBox )	
						+SVerticalBox::Slot()
						.FillHeight( 1 )
							[							
								SNew( SHorizontalBox )
								MineSweeperButton_Macro(0)								
								MineSweeperButton_Macro(1)								
								MineSweeperButton_Macro(2)											
								MineSweeperButton_Macro(3)											
								MineSweeperButton_Macro(4)											
								MineSweeperButton_Macro(5)								
								MineSweeperButton_Macro(6)								
								MineSweeperButton_Macro(7)								
								MineSweeperButton_Macro(8)						
							]	
						+SVerticalBox::Slot()
						.FillHeight( 1 )
							[							
								SNew( SHorizontalBox )
								MineSweeperButton_Macro(9)								
								MineSweeperButton_Macro(10)								
								MineSweeperButton_Macro(11)											
								MineSweeperButton_Macro(12)											
								MineSweeperButton_Macro(13)											
								MineSweeperButton_Macro(14)								
								MineSweeperButton_Macro(15)								
								MineSweeperButton_Macro(16)								
								MineSweeperButton_Macro(17)								
							]			
						+SVerticalBox::Slot()
						.FillHeight( 1 )
							[							
								SNew( SHorizontalBox )
								MineSweeperButton_Macro(18)								
								MineSweeperButton_Macro(19)								
								MineSweeperButton_Macro(20)											
								MineSweeperButton_Macro(21)											
								MineSweeperButton_Macro(22)											
								MineSweeperButton_Macro(23)								
								MineSweeperButton_Macro(24)								
								MineSweeperButton_Macro(25)								
								MineSweeperButton_Macro(26)								
							]			
						+SVerticalBox::Slot()
						.FillHeight( 1 )
							[							
								SNew( SHorizontalBox )
								MineSweeperButton_Macro(27)								
								MineSweeperButton_Macro(28)								
								MineSweeperButton_Macro(29)											
								MineSweeperButton_Macro(30)											
								MineSweeperButton_Macro(31)											
								MineSweeperButton_Macro(32)								
								MineSweeperButton_Macro(33)								
								MineSweeperButton_Macro(34)								
								MineSweeperButton_Macro(35)								
							]
						+SVerticalBox::Slot()
						.FillHeight( 1 )
							[							
								SNew( SHorizontalBox )
								MineSweeperButton_Macro(36)								
								MineSweeperButton_Macro(37)								
								MineSweeperButton_Macro(38)											
								MineSweeperButton_Macro(39)											
								MineSweeperButton_Macro(40)											
								MineSweeperButton_Macro(41)								
								MineSweeperButton_Macro(42)								
								MineSweeperButton_Macro(43)								
								MineSweeperButton_Macro(44)								
							]			
						+SVerticalBox::Slot()
						.FillHeight( 1 )
							[							
								SNew( SHorizontalBox )
								MineSweeperButton_Macro(45)								
								MineSweeperButton_Macro(46)								
								MineSweeperButton_Macro(47)											
								MineSweeperButton_Macro(48)											
								MineSweeperButton_Macro(49)											
								MineSweeperButton_Macro(50)								
								MineSweeperButton_Macro(51)								
								MineSweeperButton_Macro(52)								
								MineSweeperButton_Macro(53)								
							]		
						+SVerticalBox::Slot()
						.FillHeight( 1 )
							[							
								SNew( SHorizontalBox )
								MineSweeperButton_Macro(54)								
								MineSweeperButton_Macro(55)								
								MineSweeperButton_Macro(56)											
								MineSweeperButton_Macro(57)											
								MineSweeperButton_Macro(58)											
								MineSweeperButton_Macro(59)								
								MineSweeperButton_Macro(60)								
								MineSweeperButton_Macro(61)								
								MineSweeperButton_Macro(62)								
							]			
						+SVerticalBox::Slot()
						.FillHeight( 1 )
							[							
								SNew( SHorizontalBox )
								MineSweeperButton_Macro(63)								
								MineSweeperButton_Macro(64)								
								MineSweeperButton_Macro(65)											
								MineSweeperButton_Macro(66)											
								MineSweeperButton_Macro(67)											
								MineSweeperButton_Macro(68)								
								MineSweeperButton_Macro(69)								
								MineSweeperButton_Macro(70)								
								MineSweeperButton_Macro(71)								
							]			
						+SVerticalBox::Slot()
						.FillHeight( 1 )
							[							
								SNew( SHorizontalBox )
								MineSweeperButton_Macro(72)								
								MineSweeperButton_Macro(73)								
								MineSweeperButton_Macro(74)											
								MineSweeperButton_Macro(75)											
								MineSweeperButton_Macro(76)											
								MineSweeperButton_Macro(77)								
								MineSweeperButton_Macro(78)								
								MineSweeperButton_Macro(79)								
								MineSweeperButton_Macro(80)								
							]											
						]
					]				
				]				
			]
	];
	return UI;
}

FReply FEditorMineSweeperModule::OnGenerateGridClicked()
{
	ResetValues();
	GridSize=WidthTextBoxPtr->GetValue();
	NumberOfMines=NumberOfMinesTextBoxPtr->GetValue();
	InitialzeGrid();
	//Remove this after viewing the table
	/*for( int32 i = 0; i < Actual_Table.Num(); i++ )
	{
		Visible_Table[i]=Actual_Table[i];
	}*/
	
	FString outStr = "GridSize is " +FString::SanitizeFloat( WidthTextBoxPtr->GetValue() );
	MineLog( LogMain, Warning, outStr );
	
	//FMessageDialog::Open( EAppMsgType::Ok, FText::FromString( TEXT( "SUCCESFULLY GENERATED GRID DATA" ) ) );
	//MineSeeperUI();
	showMineField=true;
	safe = GridSize*GridSize - NumberOfMines;
	GameOver=false;
	DidWin=false;
	return FReply::Handled();
}

FReply FEditorMineSweeperModule::OnMineFieldButtonClicked( int32 i )
{
	int32 index= i-((9-GridSize) * FMath::Floor(i/9) );
	FString outStr = "Clicked on " +FString::FromInt( index );
	MineLog( LogMain, Warning, outStr );
	//FMessageDialog::Open( EAppMsgType::Ok, FText::FromString(outStr) );
	if ( Visible_Table[index].Equals("-"))
	{
		curr_symbol = Actual_Table[index];
		Visible_Table[index] = curr_symbol;
		if ( curr_symbol.Equals(" "))
		{
			openSafe ( GridSize, index );
		}
		if ( !curr_symbol.Equals("*") )
		{       
			Score++;			
			FString outStr2 = "Score : " +FString::FromInt( Score ) + "Safe : " +FString::FromInt( safe );
			MineLog( LogMain, Warning, outStr2 );
			
			if ( Score == safe )
			{
				DidWin=true;
				GameOver=true;
				
			}
		}
		if ( curr_symbol.Equals("*"))
		{
			DidWin=false;
			GameOver=true;
			
			for( int32 i = 0; i < Actual_Table.Num(); i++ )
			{
				Visible_Table[i]=Actual_Table[i];
			}			
		}
	}
	
	return FReply::Handled();
}


#pragma endregion Plugin_MineSweeper_UI


#pragma region Plugin_MINESWEEPER_LOGIC

void FEditorMineSweeperModule::InitialzeGrid()
{
	Actual_Table.Init("-",GridSize*GridSize);
	Visible_Table.Init("-",GridSize*GridSize);
	placeMines();
	calculateNumbers ();
}

void FEditorMineSweeperModule::ResetValues()
{
	GameOver=false;
	DidWin=false;
	Score = 0;
}

void FEditorMineSweeperModule::openSafe (  int32 n, int32 pos )
{
	TArray<int32> neighbors;
	neighbors.Init(0,GridSize*GridSize);
	neighbors= getneighbors (  pos, n );
	for ( int32 i = 0; i < 8; i++ )
	{ 
		if ( isRealNeighbor ( i, pos, n ) )
		{
			if ( Visible_Table[neighbors[i]].Equals("-") )
			{
				Visible_Table[neighbors[i]] = Actual_Table[neighbors[i]];
				Score++;
				if ( Visible_Table[neighbors[i]].Equals(" "))
				{
					openSafe ( n, neighbors[i] );
				}
			}
		} 
	}
	return;
}

FText FEditorMineSweeperModule::GetTableValueAt(int32 i)
{
	/*int32 index= i-((9-GridSize) * FMath::Floor(i/9) );
	FString outStr = "Clicked on " +FString::FromInt( index );
	MineLog( LogMain, Warning, outStr );*/
	FText val;
	if( Visible_Table.Num()>0 )
	{
		if( Visible_Table.IsValidIndex(i) )
		{		
			if(!Visible_Table[i].IsEmpty())
			{		
				val=FText::FromString(FString(Visible_Table[i]));
				//val=FText::FromString(FString("G"));
			}
			else
			{
				val=FText::FromString(FString("--"));
			}
		}
		else
		{
			val=FText::FromString(FString("--"));
		}
	}
	else
	{
		val=FText::FromString(FString("--"));
	}	
	return val;
}

void FEditorMineSweeperModule::placeMines()
{
	int32 mine;
	int32 num_mines = 0;
	while ( num_mines < NumberOfMines )
	{
		mine =FMath::RandRange(0,(GridSize * GridSize)-1);	
		if ( !Actual_Table[mine].Equals("*") )
		{
			Actual_Table[mine] = "*";
			num_mines++;
		}
	}
	return;

}

void FEditorMineSweeperModule::calculateNumbers()
{
	TArray<int32> neighbors;
	neighbors.Init(0,8);
	//int32 neighbors[GridSize]; 
	for ( int32 i = 0; i < (GridSize * GridSize); i++ )
	{
		if ( !Actual_Table[i].Equals("*"))
		{ 
			neighbors = getneighbors( i, GridSize);
			Actual_Table[i] = countMines ( neighbors, i,  GridSize  );		
		}
	}
	return;
}

TArray<int32> FEditorMineSweeperModule:: getneighbors (  int32 pos, int32 n )
{
	TArray<int32> neighbors;
	neighbors.Init(0,8);
	neighbors[0] = ( pos - n ) - 1;
	neighbors[1] =   pos - n ;
	neighbors[2] = ( pos - n ) + 1;
	neighbors[3] =   pos       - 1;
	neighbors[4] =   pos       + 1;
	neighbors[5] = ( pos + n ) - 1;
	neighbors[6] = ( pos + n );
	neighbors[7] = ( pos + n ) + 1;
	return neighbors;
}

bool FEditorMineSweeperModule:: isRealNeighbor ( int32 index, int32 pos, int32 n )
{
	if ( pos < n )
	{
		if ( index == 0 || index == 1 || index == 2 )
		{
			return false;
		}
	}
	if ( n * ( n - 1 ) <= pos )
	{
		if ( index == 5 || index == 6 || index == 7 )
		{
			return false;
		}
	}
	if ( pos % n == 0 )
	{
		if ( index == 0 || index == 3 || index == 5 )
		{
			return false;
		}
	}
	if ( ( pos + 1 ) % n == 0 )
	{
		if ( index == 2 || index == 4 || index == 7 )
		{
			return false;
		}
	}
	return true;
}

FString FEditorMineSweeperModule::countMines (TArray<int32> neighbors, int32 pos, int32 n )
{
	int32 mines = 0;
	FString value;
	for ( int32 i = 0; i < 8; i++ )
	{ 		
		bool isRH=isRealNeighbor( i, pos, n );	
		if ( isRH )
		{			
			if ( Actual_Table[neighbors[i]].Equals("*"))
			{
				mines++;
			}
		}
	}
	if ( mines == 0 )
	{
		value = " ";
	}
	else
	{
		//value = ( char ) ( ( ( int ) '0' ) + mines );
		value = FString::FromInt(mines);
	}
	FString outStr2 = "Mine COunt is " +FString::FromInt(mines);
	MineLog( LogMain, Warning, outStr2 );
	return value;
}

#pragma endregion Plugin_MINESWEEPER_LOGIC


#pragma region Plugin_TRASH
/* Unused Stuff*/
void FEditorMineSweeperModule::StoreWidthValue( float NewValue )
{
	FString outStr = "GridSize is " +FString::SanitizeFloat( NewValue);
	MineLog( LogMain, Warning, outStr );
	GridSize = NewValue;
}

//SHorizontalBox FEditorMineSweeperModule::MineSweeperButton_Macro()
//{
//	//TSharedRef<SVerticalBox> VB=	SNew( SVerticalBox );
//	
//	return +SHorizontalBox::Slot()
//		.FillWidth( 1 )
//		[
//			SNew( SButton ).Text( LOCTEXT( "0-0", "O" ) ).HAlign( HAlign_Center )
//			.OnClicked( FOnClicked::CreateRaw( this,&FEditorMineSweeperModule::OnMineFieldButtonClicked,0 ) )
//		.VAlign( VAlign_Center )
//		[
//			SNew( SBorder ).Content()
//			[
//				SNew( STextBlock )
//				//.Text( LOCTEXT( "ROW-COLUMN","0") )
//		.Text_Lambda([this]()->FText {return GetTableValueAt(0);})
//			]
//		]
//		];
//	
//}

#pragma endregion Plugin_TRASH


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FEditorMineSweeperModule, EditorMineSweeper )