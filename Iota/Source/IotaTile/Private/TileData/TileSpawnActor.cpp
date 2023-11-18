// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileData/TileSpawnActor.h"
#include "Components/BoxComponent.h"
#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileSpawnActor)

ATileSpawnActor::ATileSpawnActor()
{
	SetHidden(true);
	SetCanBeDamaged(false);

	// Define the bounding box subobject.
	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoundingBox");
	BoxComponent->ShapeColor = FColor(255, 0, 0, 255);
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

bool ATileSpawnActor::GenerateSpawnLocation(ANavigationData* NavData, FVector& OutLocation) const
{
	OutLocation = FVector::ZeroVector;

	if (UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
	{
		NavData = NavData ? NavData : NavSystem->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);

		if (NavData)
		{
			FVector RandomPoint;
			FNavLocation ProjectionResult;

			FVector ProjectionExtent = NavData->GetConfig().DefaultQueryExtent;
			ProjectionExtent.Z = BIG_NUMBER;

			FSharedConstNavQueryFilter Filter = UNavigationQueryFilter::GetQueryFilter(*NavData, this, FilterClass);

			do // Keep projecting random points on the mesh until we get a valid result.
			{
				RandomPoint = FMath::RandPointInBox(FBox::BuildAABB(FVector::ZeroVector, BoxComponent->GetUnscaledBoxExtent()));
				RandomPoint = BoxComponent->GetComponentToWorld().TransformPosition(RandomPoint);
			}
			while (!NavData->ProjectPoint(RandomPoint, ProjectionResult, ProjectionExtent, Filter));

			OutLocation = ProjectionResult.Location;
			return true;
		}
	}

	return false;
}

#if WITH_EDITOR

void ATileSpawnActor::EditorApplyScale(const FVector& DeltaScale, const FVector* PivotLocation, bool bAltDown, bool bShiftDown, bool bCtrlDown)
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
