// Copyright Sydney Fonderie. All Rights Reserved.

#include "TileActors/TileBoundActor.h"
#include "TileData/TileBound.h"
#include "Components/BoxComponent.h"
#include "Engine/CollisionProfile.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileBoundActor)

ATileBoundActor::ATileBoundActor()
{
	bIsEditorOnlyActor = true;
	SetActorHiddenInGame(true);
	SetCanBeDamaged(false);

	// Define the bounding box subobject.
	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoundingBox");
	BoxComponent->ShapeColor = FColor(0, 255, 255, 255);
	BoxComponent->InitBoxExtent(FVector(200, 200, 200));
	BoxComponent->SetLineThickness(2);

	// Do not use standard collision; the actor only exists as a visual representation.
	BoxComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	BoxComponent->Mobility = EComponentMobility::Static;
	BoxComponent->bShouldCollideWhenPlacing = true;
	BoxComponent->bHiddenInGame = false;

	RootComponent = BoxComponent;
	bCollideWhenPlacing = true;
}

FTileBound ATileBoundActor::GetTileBound() const
{
	return FTileBound(
		BoxComponent->GetComponentLocation(),
		BoxComponent->GetComponentRotation(),
		BoxComponent->GetScaledBoxExtent()
	);
}

#if WITH_EDITOR

void ATileBoundActor::EditorApplyScale(const FVector& DeltaScale, const FVector* PivotLocation, bool bAltDown, bool bShiftDown, bool bCtrlDown)
{
	const FVector ModifiedScale = DeltaScale * (AActor::bUsePercentageBasedScaling ? 5000 : 50);

	if (bCtrlDown)
	{
		// CTRL + Scale adjusts the bounding box vertically.
		FVector Extent = BoxComponent->GetUnscaledBoxExtent() + FVector(0, 0, ModifiedScale.X);
		Extent.Z = FMath::Max<FVector::FReal>(0, Extent.Z);
		BoxComponent->SetBoxExtent(Extent);
	}
	else
	{
		// Scale the bounding box extent instead of the actor while using the editor widget.
		FVector Extent = BoxComponent->GetUnscaledBoxExtent() + ModifiedScale;
		Extent.X = FMath::Max<FVector::FReal>(0, Extent.X);
		Extent.Y = FMath::Max<FVector::FReal>(0, Extent.Y);
		Extent.Z = FMath::Max<FVector::FReal>(0, Extent.Z);
		BoxComponent->SetBoxExtent(Extent);
	}
}

#endif
