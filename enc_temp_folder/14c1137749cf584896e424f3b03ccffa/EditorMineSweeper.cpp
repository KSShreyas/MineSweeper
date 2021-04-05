// Copyright Epic Games, Inc. All Rights Reserved.

#include "EditorMineSweeper.h"


DEFINE_LOG_CATEGORY( LogMain );

static const FName EditorMineSweeperTabName( "EditorMineSweeper" );

#define LOCTEXT_NAMESPACE "FEditorMineSweeperModule"


#pragma region CUSTOM_EXPERIMENTAL_MACROS

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
	LOOP_N_TIMES(n,+SVerticalBox::Slot().FillHeight( 1 )[SNew( SHorizontalBox )	LOOP_N_TIMES(n,RowButton(0))])	\

//LOOP_N_TIMES(INNER_LOOP_COUNT,RowButton(0))
#pragma endregion CUSTOM_EXPERIMENTAL_MACROS


#pragma region CUSTOM_MACROS

#define RowButton(i) \
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
					if(  vis_table[i-((9-GridSize) * FMath::Floor(i/9) )].Equals("-")){ return (FLinearColor(1.f, 1.f, 1.f, 1.f));}else if(  vis_table[i-((9-GridSize) * FMath::Floor(i/9) )].Equals(" ")){ return (FLinearColor(1.f, 1.f, 1.f, 0.5f));}else	if(  vis_table[i-((9-GridSize) * FMath::Floor(i/9) )].Equals("*")){ return (FLinearColor(1.f, 0.f, 0.f, 0.35f));} else { return (FLinearColor(1.f, 1.f, 1.f, 1.f));}	})		\
					[\
					SNew( SBorder ).Content()\
						[\
							SNew( STextBlock ) \
							/*.ColorAndOpacity( FLinearColor(0.0f, 1.0f, 0.0f))*/	\
							.Text_Lambda([this]()->FText { return  GetTableValueAt(i-((9-GridSize) * FMath::Floor(i/9) ))	\
							/*FText::AsNumber(i)*/ /*FText::AsNumber(i-((9-GridSize) * FMath::Floor(i/9) ))*/;})\
							.ColorAndOpacity_Lambda([this]()-> FSlateColor { if(  vis_table[i-((9-GridSize) * FMath::Floor(i/9) )].Equals("6")){return FSlateColor(FLinearColor(1.0f, 0.f, 0.f, 1.f));}else if(  vis_table[i-((9-GridSize) * FMath::Floor(i/9) )].Equals("5")){return FSlateColor(FLinearColor(0.9f, 0.f, 0.f, 1.f));}if(  vis_table[i-((9-GridSize) * FMath::Floor(i/9) )].Equals("4")){return FSlateColor(FLinearColor(0.8f, 0.f, 0.f, 1.f));} else if(  vis_table[i-((9-GridSize) * FMath::Floor(i/9) )].Equals("3")){return FSlateColor(FLinearColor(0.7f, 0.f, 0.f, 1.f));} else if(  vis_table[i-((9-GridSize) * FMath::Floor(i/9) )].Equals("2")){return FSlateColor(FLinearColor(0.6f, 0.f, 0.f, 1.f));} else {return FSlateColor(FLinearColor(0.f, 0.f, 0.f, 1.f));}}  )	\
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


#pragma region Plugin_MineSweeper_Logic

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
						.Text( LOCTEXT( "Editor-Minecraft", "Editor Minecraft" ) )
						.ColorAndOpacity( FLinearColor(0.5f, 0.5f, 0.5f))
						/*.ColorAndOpacity_Lambda([this]()-> FSlateColor { return FSlateColor(FLinearColor(1.f, 1.f, 1.f, 1.f));})*/		
					]
				+SScrollBox::Slot().Padding( 10, 5 )
					[
						SNew( SHorizontalBox )
						+SHorizontalBox::Slot()
					.AutoWidth()
						[
							SAssignNew( GenerateGridButtonPtr, SButton )
							.Text( LOCTEXT( "Generate-New-Grid", "Generate New Grid." ) )
							.OnClicked( FOnClicked::CreateRaw( this, &FEditorMineSweeperModule::OnGenerateGridClicked ) )
							//.ButtonColorAndOpacity(this, &SPrimaryAssetIdGraphPin::OnGetWidgetBackground)
							.ButtonColorAndOpacity_Lambda([this]()-> FLinearColor { return (FLinearColor(1.f, 1.f, 1.f, 1.f)); })
					//FSlateColor(FLinearColor(1.f, 1.f, 1.f, Alpha))
						]

					]
				+SScrollBox::Slot().Padding( 5 )
					[
						SNew( STextBlock ).ShadowOffset( HeadingShadowOffset ).Font( LargeLayoutFont ).Text( LOCTEXT( "Grid-Size", "Grid Size" ) )
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
								SNew( STextBlock ).Font( SmallLayoutFont ).Text( LOCTEXT( "NoOfMinesLable", "Number Of NumberOfMines :" ) )
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
								[
									SNew(SBox) .HAlign( HAlign_Center ) .VAlign( VAlign_Center )
									.Visibility_Lambda([this]()->EVisibility {return GameOver && DidWin ? EVisibility::Visible : EVisibility::Collapsed;})
									[
										SNew( STextBlock ).ShadowOffset( HeadingShadowOffset ).Font( XXLargeLayoutFont ).Text( LOCTEXT( "You-Win", "You Win!" ) )									
									]
								]
								[
									SNew( STextBlock ).ShadowOffset( HeadingShadowOffset ).Font( XXLargeLayoutFont ).Text( LOCTEXT( "You-Lose", "You Lose!" ) )
									.Visibility_Lambda([this]()->EVisibility {return GameOver && !DidWin  ? EVisibility::Visible : EVisibility::Collapsed;})
								]
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
								RowButton(0)								
								RowButton(1)								
								RowButton(2)											
								RowButton(3)											
								RowButton(4)											
								RowButton(5)								
								RowButton(6)								
								RowButton(7)								
								RowButton(8)						
							]	
						+SVerticalBox::Slot()
						.FillHeight( 1 )
							[							
								SNew( SHorizontalBox )
								RowButton(9)								
								RowButton(10)								
								RowButton(11)											
								RowButton(12)											
								RowButton(13)											
								RowButton(14)								
								RowButton(15)								
								RowButton(16)								
								RowButton(17)								
							]			
						+SVerticalBox::Slot()
						.FillHeight( 1 )
							[							
								SNew( SHorizontalBox )
								RowButton(18)								
								RowButton(19)								
								RowButton(20)											
								RowButton(21)											
								RowButton(22)											
								RowButton(23)								
								RowButton(24)								
								RowButton(25)								
								RowButton(26)								
							]			
						+SVerticalBox::Slot()
						.FillHeight( 1 )
							[							
								SNew( SHorizontalBox )
								RowButton(27)								
								RowButton(28)								
								RowButton(29)											
								RowButton(30)											
								RowButton(31)											
								RowButton(32)								
								RowButton(33)								
								RowButton(34)								
								RowButton(35)								
							]
						+SVerticalBox::Slot()
						.FillHeight( 1 )
							[							
								SNew( SHorizontalBox )
								RowButton(36)								
								RowButton(37)								
								RowButton(38)											
								RowButton(39)											
								RowButton(40)											
								RowButton(41)								
								RowButton(42)								
								RowButton(43)								
								RowButton(44)								
							]			
						+SVerticalBox::Slot()
						.FillHeight( 1 )
							[							
								SNew( SHorizontalBox )
								RowButton(45)								
								RowButton(46)								
								RowButton(47)											
								RowButton(48)											
								RowButton(49)											
								RowButton(50)								
								RowButton(51)								
								RowButton(52)								
								RowButton(53)								
							]		
						+SVerticalBox::Slot()
						.FillHeight( 1 )
							[							
								SNew( SHorizontalBox )
								RowButton(54)								
								RowButton(55)								
								RowButton(56)											
								RowButton(57)											
								RowButton(58)											
								RowButton(59)								
								RowButton(60)								
								RowButton(61)								
								RowButton(62)								
							]			
						+SVerticalBox::Slot()
						.FillHeight( 1 )
							[							
								SNew( SHorizontalBox )
								RowButton(63)								
								RowButton(64)								
								RowButton(65)											
								RowButton(66)											
								RowButton(67)											
								RowButton(68)								
								RowButton(69)								
								RowButton(70)								
								RowButton(71)								
							]			
						+SVerticalBox::Slot()
						.FillHeight( 1 )
							[							
								SNew( SHorizontalBox )
								RowButton(72)								
								RowButton(73)								
								RowButton(74)											
								RowButton(75)											
								RowButton(76)											
								RowButton(77)								
								RowButton(78)								
								RowButton(79)								
								RowButton(80)								
							]											
						]
					]				
				]				
			]
	];
	return UI;
}
void FEditorMineSweeperModule::ResetValues()
{
	GameOver=false;
	DidWin=false;
	Score = 0;
}
FReply FEditorMineSweeperModule::OnGenerateGridClicked()
{
	ResetValues();
	GridSize=WidthTextBoxPtr->GetValue();
	NumberOfMines=NumberOfMinesTextBoxPtr->GetValue();
	InitialzeGrid();
	//Remove this after viewing the table
	/*for( int32 i = 0; i < real_table.Num(); i++ )
	{
		vis_table[i]=real_table[i];
	}*/
	
	FString outStr = "GridSize is " +FString::SanitizeFloat( WidthTextBoxPtr->GetValue() );
	MineLog( LogMain, Warning, outStr );
	
	//FMessageDialog::Open( EAppMsgType::Ok, FText::FromString( TEXT( "SUCCESFULLY GENERATED GRID DATA" ) ) );
	//MineSeeperUI();
	showMineField=true;
	safe = GridSize*GridSize - NumberOfMines-1;
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
	if ( vis_table[index].Equals("-"))
	{
		curr_symbol = real_table[index];
		vis_table[index] = curr_symbol;
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
				GameOver=true;
				DidWin=true;
			}
		}
		if ( curr_symbol.Equals("*"))
		{
			GameOver=true;
			DidWin=false;
			for( int32 i = 0; i < real_table.Num(); i++ )
			{
				vis_table[i]=real_table[i];
			}			
		}
	}
	
	return FReply::Handled();
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
			if ( vis_table[neighbors[i]].Equals("-") )
			{
				vis_table[neighbors[i]] = real_table[neighbors[i]];
				Score++;
				if ( vis_table[neighbors[i]].Equals(" "))
				{
					openSafe ( n, neighbors[i] );
				}
			}
		} 
	}
	return;
}



#pragma region Plugin_Grid_Initilisation



void FEditorMineSweeperModule::InitialzeGrid()
{
	real_table.Init("-",GridSize*GridSize);
	vis_table.Init("-",GridSize*GridSize);
	placeMines();
	calculateNumbers ();
}
FText FEditorMineSweeperModule::GetTableValueAt(int32 i)
{
	/*int32 index= i-((9-GridSize) * FMath::Floor(i/9) );
	FString outStr = "Clicked on " +FString::FromInt( index );
	MineLog( LogMain, Warning, outStr );*/
	FText val;
	if( vis_table.Num()>0 )
	{
		if( vis_table.IsValidIndex(i) )
		{		
			if(!vis_table[i].IsEmpty())
			{		
				val=FText::FromString(FString(vis_table[i]));
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
		if ( !real_table[mine].Equals("*") )
		{
			real_table[mine] = "*";
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
		if ( !real_table[i].Equals("*"))
		{ 
			neighbors = getneighbors( i, GridSize);
			real_table[i] = countMines ( neighbors, i,  GridSize  );		
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
			if ( real_table[neighbors[i]].Equals("*"))
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

#pragma endregion Plugin_Grid_Initilisation

void FEditorMineSweeperModule::StoreWidthValue( float NewValue )
{
	FString outStr = "GridSize is " +FString::SanitizeFloat( NewValue);
	MineLog( LogMain, Warning, outStr );
	GridSize = NewValue;
}

//SHorizontalBox FEditorMineSweeperModule::RowButton()
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


#pragma endregion Plugin_MineSweeper_Logic
#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FEditorMineSweeperModule, EditorMineSweeper )