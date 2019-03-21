// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "FP_ERHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"

AFP_ERHUD::AFP_ERHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;
}


void AFP_ERHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition( (Center.X),
										   (Center.Y + 20.0f));

	// draw the crosshair
	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;

	UWorld* const World = GetWorld();
	if (World)
	{
		APlayerController* MyController = World->GetFirstPlayerController();
		/*MyController->bShowMouseCursor = true;
		MyController->bEnableClickEvents = true;
		MyController->bEnableMouseOverEvents = true;*/
		float LocationX;
		float LocationY;
		if (MyController != nullptr)
		{
			MyController->GetMousePosition(LocationX, LocationY);
			FVector2D MousePosition(LocationX, LocationY);
			TileItem.Position = MousePosition;
		}
	}

	Canvas->DrawItem( TileItem );
}
