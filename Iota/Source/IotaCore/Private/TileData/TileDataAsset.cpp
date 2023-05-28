// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileData/TileDataAsset.h"
#include "TileData/TilePortal.h"
#include "TileData/TileBound.h"
#include "UObject/SavePackage.h"
#include "UObject/Package.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileDataAsset)

void UTileDataAsset::SaveAsset()
{
	// Access our package so that we can save it.
	UPackage* AssetPackage = GetPackage();

	// Extract our package's file location on the local disk.
	FString FileName = FPackageName::LongPackageNameToFilename(AssetPackage->GetName(), FPackageName::GetAssetPackageExtension());

	// Assemble save package arguments.
	FSavePackageArgs SaveArguments;

	// Attempt to save the package using a standard call.
	UPackage::SavePackage(AssetPackage, this, *FileName, SaveArguments);
}
