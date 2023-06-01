// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileData/TileBoundActor.h"
#include "Components/BoxComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileBoundActor)

ATileBoundActor::ATileBoundActor()
{
	SetHidden(true);
	SetCanBeDamaged(false);

	// Define the bounding box subobject.
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoundingBox"));
	BoxComponent->ShapeColor = FColor(0, 255, 255, 255);
	BoxComponent->InitBoxExtent(FVector(200, 200, 200));
	BoxComponent->SetLineThickness(1);

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
	const float Factor = AActor::bUsePercentageBasedScaling ? 1000 : 10;

	// Scale the bounding box extent instead of the actor while using the editor widget.
	FVector Extent = BoxComponent->GetUnscaledBoxExtent() + DeltaScale * Factor;
	Extent.X = FMath::Max<FVector::FReal>(0, Extent.X);
	Extent.Y = FMath::Max<FVector::FReal>(0, Extent.Y);
	Extent.Z = FMath::Max<FVector::FReal>(0, Extent.Z);
	BoxComponent->SetBoxExtent(Extent);
}

#endif
