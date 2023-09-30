// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TilePlanStream.h"
#include "TileData/TilePlan.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TilePlanStream)

UTilePlanStream* UTilePlanStream::StreamInstance(UWorld* World, const FTilePlan& BasePlan, const FString& PlanName)
{
	FTransform Transform(BasePlan.Rotation, BasePlan.Location);
	FString PackageName = BasePlan.Level.GetLongPackageName();

	ULevelStreamingDynamic::FLoadLevelInstanceParams StreamParams(World, PackageName, Transform);
	StreamParams.OptionalLevelStreamingClass = StaticClass();
	StreamParams.OptionalLevelNameOverride = &PlanName;

	bool bSuccess = false;

	ULevelStreamingDynamic* NewStream = ULevelStreamingDynamic::LoadLevelInstance(StreamParams, bSuccess);
	return bSuccess ? Cast<UTilePlanStream>(NewStream) : nullptr;
}
