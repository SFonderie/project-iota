// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IotaCore/AssetActor.h"
#include "GameplayTagContainer.h"
#include "Engine/AssetManager.h"

/**
 * Collects and stores AAssetActor subtypes in a map keyed to a custom value type. Subtypes must
 * be loaded to be collected; use the Asset Manager to handle this.
 *
 * @param KeyType Value type to pass to the internal multi-map for use as keys.
 * @param ActorType Actor type for which to collect subtypes. Must derive from AAssetActor.
 */
template <typename KeyType, class ActorType>
class TActorTable
{

public:

	TActorTable()
	{
		// Default constructor.
	}

	/**
	 * Creates a new actor table and fills it with all AAssetActor subtypes that derive from the
	 * template actor type. Each qualifying subtype will then pass through the key predicate method
	 * to extract a key value.
	 *
	 * @param Predicate Predicate function with which to extract key values. Each predicate should
	 * accept a template actor type pointer and return a template key type value.
	 */
	template <typename KeyPredicate>
	TActorTable(KeyPredicate Predicate)
	{
		Collect(Predicate);
	}

	/**
	 * Creates a new actor table and fills it with all AAssetActor subtypes that derive from the
	 * template actor type. Each qualifying subtype will then pass through the key predicate method
	 * to extract a key value.
	 *
	 * @param Category Category tag for which to load subtypes. Only subtypes explicitly marked
	 * with this tag will be loaded. If no category is needed, use Collect instead.
	 * @param Predicate Predicate function with which to extract key values. Each predicate should
	 * accept a template actor type pointer and return a template key type value.
	 */
	template <typename KeyPredicate>
	TActorTable(const FGameplayTag& Category, KeyPredicate Predicate)
	{
		CollectWithCategory(Category, Predicate);
	}

	virtual ~TActorTable()
	{
		// Default destructor.
	}

	/**
	 * Loads all AAssetActor subtypes that derive from the template actor type. Each qualifying
	 * subtype will then pass through the key predicate method to extract a key value.
	 *
	 * @param Predicate Predicate function with which to extract key values. Each predicate should
	 * accept a template actor type pointer and return a template key type value.
	 */
	template <typename KeyPredicate>
	void Collect(KeyPredicate Predicate)
	{
		UAssetManager& AssetManager = UAssetManager::Get();

		// Extract all primary asset data values for the template actor type.
		TArray<FAssetData> AssetDataList;
		AssetManager.GetPrimaryAssetDataList(ActorType::StaticClass()->GetFName(), AssetDataList);

		for (const FAssetData& AssetData : AssetDataList)
		{
			// Extract the asset class as an AAssetActor to ensure it actually derives.
			TSubclassOf<AAssetActor> AssetClass = AssetManager.GetPrimaryAssetObjectClass<AAssetActor>(AssetData.GetPrimaryAssetId());

			if (AAssetActor* AssetObject = AssetClass.GetDefaultObject())
			{
				// Invoke the predicate on the class default object to get a key value.
				KeyType NewKey = Invoke(Predicate, Cast<ActorType>(AssetObject));

				// Cast the asset class to the template actor type to get a table value.
				TSubclassOf<ActorType> NewValue = AssetClass;

				// Add the new entry to the table.
				Table.AddUnique(NewKey, NewValue);
			}
		}
	}

	/**
	 * Loads all AAssetActor subtypes that derive from the template actor type and are marked with
	 * the provided category tag. Each qualifying subtype will then pass through the key predicate
	 * method to extract a key value.
	 *
	 * @param Category Category tag for which to load subtypes. Only subtypes explicitly marked
	 * with this tag will be loaded. If no category is needed, use Collect instead.
	 * @param Predicate Predicate function with which to extract key values. Each predicate should
	 * accept a template actor type pointer and return a template key type value.
	 */
	template <typename KeyPredicate>
	void CollectWithCategory(const FGameplayTag& Category, KeyPredicate Predicate)
	{
		UAssetManager& AssetManager = UAssetManager::Get();

		// Extract all primary asset data values for the template actor type.
		TArray<FAssetData> AssetDataList;
		AssetManager.GetPrimaryAssetDataList(ActorType::StaticClass()->GetFName(), AssetDataList);

		for (const FAssetData& AssetData : AssetDataList)
		{
			// Extract the asset class as an AAssetActor to ensure it actually derives.
			TSubclassOf<AAssetActor> AssetClass = AssetManager.GetPrimaryAssetObjectClass<AAssetActor>(AssetData.GetPrimaryAssetId());

			if (AAssetActor* AssetObject = AssetClass.GetDefaultObject())
			{
				if (AssetObject->Categories.HasTagExact(Category))
				{
					// Invoke the predicate on the class default object to get a key value.
					KeyType NewKey = Invoke(Predicate, Cast<ActorType>(AssetObject));

					// Cast the asset class to the template actor type to get a table value.
					TSubclassOf<ActorType> NewValue(AssetClass);

					// Add the new entry to the table.
					Table.AddUnique(NewKey, NewValue);
				}
			}
		}
	}

	/**
	 * Returns the first actor subtype associated with the provided key. If no subtypes are found,
	 * this method will return null.
	 *
	 * @param InKey Key for which to return a value.
	 * @return First actor subtype tied to the given key.
	 */
	TSubclassOf<ActorType> GetFirstSubtype(const KeyType& InKey) const
	{
		TArray<TSubclassOf<ActorType>> ValueList;
		Table.MultiFind(InKey, ValueList);

		if (!ValueList.IsEmpty())
		{
			return *ValueList.GetData();
		}

		return TSubclassOf<ActorType>();
	}

	/**
	 * Returns a random actor subtype associated with the provided key. If no subtypes are found,
	 * this method will return null.
	 *
	 * @param InKey Key for which to return a value.
	 * @return Random actor subtype tied to the given key.
	 */
	TSubclassOf<ActorType> GetRandomSubtype(const KeyType& InKey) const
	{
		TArray<TSubclassOf<ActorType>> ValueList;
		Table.MultiFind(InKey, ValueList);

		if (!ValueList.IsEmpty())
		{
			return ValueList[FMath::RandRange(0, ValueList.Num() - 1)];
		}

		return TSubclassOf<ActorType>();
	}

	/**
	 * Returns the number of entries currently collected in the actor table.
	 *
	 * @return Number of actor types stored in the table.
	 */
	int32 GetSize() const
	{
		return Table.Num();
	}

	/**
	 * Returns the list of actor subtypes associated with the provided key and appends them to the
	 * provided array. If no subtypes were found, the provided list will return as-is.
	 *
	 * @param InKey Key for which to return values.
	 * @param OutValues Array in which to place values.
	 */
	void GetSubtypes(const KeyType& InKey, TArray<TSubclassOf<ActorType>>& Values) const
	{
		Table.MultiFind(InKey, Values);
	}

	/** Empties the actor table. */
	virtual void Empty()
	{
		Table.Empty();
	}

private:

	/** Internal actor subtype table. */
	TMultiMap<KeyType, TSubclassOf<ActorType>> Table;
};
