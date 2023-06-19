// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileData/TilePortalActor.h"
#include "TileData/TilePortal.h"
#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/ArrowComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Texture2D.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TilePortalActor)

ATilePortalActor::ATilePortalActor()
{
	SetHidden(true);
	SetCanBeDamaged(false);

	// Portal plane component is a box component with a collapsed extent.
	PlaneComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("PlaneExtent"));
	PlaneComponent->ShapeColor = FColor(0, 255, 255, 255);
	PlaneComponent->InitBoxExtent(FVector(0, 150, 150));
	PlaneComponent->SetLineThickness(3);

	// Do not use standard collision; the actor only exists as a visual representation.
	PlaneComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	PlaneComponent->Mobility = EComponentMobility::Static;
	PlaneComponent->bShouldCollideWhenPlacing = true;
	PlaneComponent->bHiddenInGame = false;

	// Set the plane as the root.
	RootComponent = PlaneComponent;
	bCollideWhenPlacing = true;

#if WITH_EDITORONLY_DATA

	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UTexture2D> SpriteObject;
		FName ID_TilePortal;
		FText NAME_TilePortal;

		FConstructorStatics()
			: SpriteObject(TEXT("/Engine/EditorResources/S_TriggerBox"))
			, ID_TilePortal(TEXT("TilePortal"))
			, NAME_TilePortal(NSLOCTEXT("SpriteCategory", "TilePortal", "Tile Portal"))
		{
			// Default constructor.
		}
	};

	static FConstructorStatics ConstructorStatics;

	// Define the billboard component subobject.
	SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("HandleSprite"));
	SpriteComponent->Sprite = ConstructorStatics.SpriteObject.Get();
	SpriteComponent->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	SpriteComponent->bIsScreenSizeScaled = true;

	// Set up some other properties for the sprite.
	SpriteComponent->SpriteInfo.Category = ConstructorStatics.ID_TilePortal;
	SpriteComponent->SpriteInfo.DisplayName = ConstructorStatics.NAME_TilePortal;
	SpriteComponent->bHiddenInGame = false;

	// Connect the billboard to the plane center.
	SpriteComponent->SetupAttachment(PlaneComponent);

	// Define the arrow component subobject.
	ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("DirectionVector"));
	ArrowComponent->ArrowColor = FColor(0, 255, 255, 255);
	ArrowComponent->bIsScreenSizeScaled = true;
	ArrowComponent->bTreatAsASprite = true;
	ArrowComponent->ArrowSize = 0.8f;

	// Set up some other properties for the arrow.
	ArrowComponent->SpriteInfo.Category = ConstructorStatics.ID_TilePortal;
	ArrowComponent->SpriteInfo.DisplayName = ConstructorStatics.NAME_TilePortal;
	ArrowComponent->bHiddenInGame = false;

	// Put the arrow component in position near the sprite.
	ArrowComponent->SetupAttachment(PlaneComponent);
	ArrowComponent->SetRelativeLocation(FVector(10, 0, 0));

#endif
}

FTilePortal ATilePortalActor::GetTilePortal() const
{
	// Ignore the up component of the forward.
	FVector Forward = GetActorForwardVector();
	Forward = FVector(Forward.X, Forward.Y, 0);
	Forward.Normalize();

	// Round the plane extent to the nearest meter value.
	FVector PlaneExtent = PlaneComponent->GetScaledBoxExtent();
	FIntPoint PlaneSize(
		FMath::FloorToInt32(PlaneExtent.Y / 50),
		FMath::FloorToInt32(PlaneExtent.Z / 50)
	);

	return FTilePortal(GetActorLocation(), Forward, PlaneSize);
}

#if WITH_EDITOR

void ATilePortalActor::EditorApplyRotation(const FRotator& DeltaRotation, bool bAltDown, bool bShiftDown, bool bCtrlDown)
{
	Super::EditorApplyRotation(FRotator(0, DeltaRotation.Yaw, 0), bAltDown, bShiftDown, bCtrlDown);
}

void ATilePortalActor::EditorApplyScale(const FVector& DeltaScale, const FVector* PivotLocation, bool bAltDown, bool bShiftDown, bool bCtrlDown)
{
	const FVector ModifiedScale = DeltaScale * (AActor::bUsePercentageBasedScaling ? 5000 : 50);

	if (bCtrlDown)
	{
		// CTRL + Scale adjusts the portal horizontally, since that's the common direction.
		FVector Extent = PlaneComponent->GetUnscaledBoxExtent() + FVector(0, ModifiedScale.Y, 0);
		Extent.Y = FMath::Max<FVector::FReal>(0, Extent.Y);
		PlaneComponent->SetBoxExtent(Extent);
	}
	else
	{
		// Scale plane in the two directions that a plane can normally scale.
		FVector Extent = PlaneComponent->GetUnscaledBoxExtent() + FVector(0, ModifiedScale.Y, ModifiedScale.Z);
		Extent.Y = FMath::Max<FVector::FReal>(0, Extent.Y);
		Extent.Z = FMath::Max<FVector::FReal>(0, Extent.Z);
		PlaneComponent->SetBoxExtent(Extent);
	}
}

#endif
