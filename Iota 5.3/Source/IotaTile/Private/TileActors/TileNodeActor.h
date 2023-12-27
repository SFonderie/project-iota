// Copyright Sydney Fonderie. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileNodeActor.generated.h"

struct FTileNode;
class USceneComponent;
class UBillboardComponent;

/** Indicates an area graph node in the world. */
UCLASS()
class ATileNodeActor : public AActor
{
	GENERATED_BODY()

public:

	/** Gameplay actors relevant to this area graph node. */
	UPROPERTY(Category = "NodeData", EditAnywhere)
	TArray<TSoftObjectPtr<AActor>> References;

	ATileNodeActor();

	/** @return The node actor converted into an area node. */
	FTileNode GetNodeData() const;

private:

	/** Scene component used as the actor root for all builds. */
	UPROPERTY(BlueprintReadOnly, Category = "AreaNode", VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USceneComponent> SceneComponent;

#if WITH_EDITORONLY_DATA

	/** Billboard component used to access the node actor in-editor. */
	UPROPERTY()
	TObjectPtr<UBillboardComponent> SpriteComponent;

#endif
};
