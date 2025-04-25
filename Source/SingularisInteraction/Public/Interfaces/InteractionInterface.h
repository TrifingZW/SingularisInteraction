/* =====================================================================
 * InteractableInterface.h
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2025 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024-2025 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#pragma once

#include <CoreMinimal.h>
#include <Engine/NetSerialization.h>
#include <UObject/Interface.h>

#include "InteractionInterface.generated.h"

struct FInputActionValue;

// 此类不需要修改
UINTERFACE(MinimalAPI)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 交互接口
 */
class SINGULARISINTERACTION_API IInteractionInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "交互接口")
	void OnBeginHover(AActor* Interactor, FVector_NetQuantize ImpactPoint);

	UFUNCTION(BlueprintNativeEvent, Category = "交互接口")
	void OnEndHover(AActor* Interactor, FVector_NetQuantize ImpactPoint);

	UFUNCTION(BlueprintNativeEvent, Category = "交互接口")
	void OnInteract(AActor* Interactor, FVector_NetQuantize ImpactPoint, const FInputActionValue& Value);
};
