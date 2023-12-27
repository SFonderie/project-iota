// Copyright Sydney Fonderie. All Rights Reserved.

#include "TileActors/TileNodeActor.h"
#include "TileData/TileNode.h"
#include "Components/SceneComponent.h"
#include "Components/BillboardComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Texture2D.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileNodeActor)

ATileNodeActor::ATileNodeActor()
{
	bIsEditorOnlyActor = true;
	SetActorHiddenInGame(true);
	SetCanBeDamaged(false);

	// Export scene component is a standard static scene component.
	SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SceneComponent->Mobility = EComponentMobility::Static;
	RootComponent = SceneComponent;

#if WITH_EDITORONLY_DATA

	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UTexture2D> SpriteObject;

		FConstructorStatics() : SpriteObject(TEXT("/Engine/EditorResources/S_TargetPoint"))
		{
			// Default constructor.
		}
	};

	static FConstructorStatics ConstructorStatics;

	SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>("HandleSprite");

	if (IsValid(SpriteComponent))
	{
		SpriteComponent->Sprite = ConstructorStatics.SpriteObject.Get();
		SpriteComponent->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
		SpriteComponent->bIsScreenSizeScaled = true;
		SpriteComponent->bHiddenInGame = false;

		// Connect the billboard to the root component.
		SpriteComponent->SetupAttachment(SceneComponent);
	}

#endif
}

FTileNode ATileNodeActor::GetNodeData() const
{
	return FTileNode(GetActorLocation(), References);
}
