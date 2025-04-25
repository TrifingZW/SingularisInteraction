/* =====================================================================
 * InteractionConfig.h
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2025 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024-2025 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#pragma once

#include <CoreMinimal.h>

#include "InteractionType.h"

#include <InteractionConfig.generated.h>

/**
 * 交互配置结构体
 */
enum class EInteractionType : uint8;

USTRUCT(BlueprintType)
struct SINGULARISINTERACTION_API FInteractionConfig
{
	GENERATED_BODY()

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "交互",
		meta = (DisplayName = "交互类型")
	)
	EInteractionType InteractionType = EInteractionType::Press;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "交互",
		meta = (DisplayName = "长按时间")
	)
	float HoldDuration = 0.5f;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "交互",
		meta = (DisplayName = "连击次数")
	)
	int32 RequiredTaps = 3;
};
