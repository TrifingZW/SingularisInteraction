// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

// 此类不需要修改
UINTERFACE(MinimalAPI)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 交互接口
 */
class SINGULARISINTERACTION_API IInteractableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void OnBeginHover(AActor* Interactor);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void OnEndHover(AActor* Interactor);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void OnInteract(AActor* Interactor);
};
