// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "IotaCore/AssetActor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AssetActor)

FPrimaryAssetId AAssetActor::GetPrimaryAssetId() const
{
	// Filter out instances by only considering the Class Default Object.
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		// Each Asset ID should correspond to a Blueprint asset in the editor, so we need to
		// exclude any object with a native class.
		if (GetClass()->HasAnyClassFlags(CLASS_Native | CLASS_Intrinsic))
		{
			return FPrimaryAssetId();
		}

		// Begin traversing up the class hierarchy.
		UClass* SuperClass = GetClass()->GetSuperClass();
		UClass* ParentType = nullptr;

		// Immediate children of AAssetActor should only ever be used as parent types.
		// If this is one such class, return an invalid Asset ID.
		if (SuperClass == AAssetActor::StaticClass())
		{
			return FPrimaryAssetId();
		}

		while (SuperClass)
		{
			// If the next class up the hierarchy is AAssetActor, then we have reached the root
			// of the class hierarchy and must use the current super class as the parent type.
			if (SuperClass->GetSuperClass() == AAssetActor::StaticClass())
			{
				ParentType = SuperClass;
				break;
			}

			// If there are still classes to traverse, try to find the first native parent class
			// so that it can be used as the ideal parent type.
			else if (SuperClass->HasAnyClassFlags(CLASS_Native | CLASS_Intrinsic))
			{
				ParentType = SuperClass;
				break;
			}

			// Otherwise, continue up the hierarchy.
			else
			{
				SuperClass = SuperClass->GetSuperClass();
			}
		}

		// If the search was able to find a viable parent type, use it to create an Asset ID from
		// the names of the parent type and base type.
		if (ParentType)
		{
			// If the parent class is a native class, we can use its name directly. If the parent
			// class is a Blueprint class, we will need to use the short package name instead.
			FName ParentNameBP = FPackageName::GetShortFName(ParentType->GetPackage()->GetFName());
			FName ParentNameNC = ParentType->GetFName();

			// Select a name based on the class flags.
			FName ParentName = ParentType->HasAnyClassFlags(CLASS_Native | CLASS_Intrinsic) ? ParentNameNC : ParentNameBP;

			// We have already ensured that the asset is a Blueprint, so we can skip the native
			// check and extract the short package name directly.
			FName AssetName = FPackageName::GetShortFName(GetPackage()->GetFName());

			// Return the now-valid Asset ID.
			return FPrimaryAssetId(ParentName, AssetName);
		}
	}

	return FPrimaryAssetId();
}