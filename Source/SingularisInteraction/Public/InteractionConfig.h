/* =====================================================================
 * InteractionConfig.h
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#pragma once

#include "CoreMinimal.h"
#include "InteractionType.h"
#include "InteractionConfig.generated.h"

/**
 * 交互配置结构体
 */

enum class EInteractionType : uint8;

USTRUCT(BlueprintType)
struct SINGULARISINTERACTION_API FInteractionConfig
{
	GENERATED_BODY()

#pragma region 交互配置属性

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "属性",
		meta = (
			DisplayName = "交互类型",
			ToolTip = "该交互目标的交互类型，列如：按下，松开，长按等。"
		)
	)
	EInteractionType InteractionType = EInteractionType::Press;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "属性",
		meta = (
			DisplayName = "长按时间",
			ToolTip = "如果交互类型是长按触发，则该值决定了长按的时间阈值。"
		)
	)
	float HoldDuration = 0.5f;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "属性",
		meta = (
			DisplayName = "连击次数",
			ToolTip = "如果交互类型是连击触发，则该值决定了连接的次数。"
		)
	)
	int32 RequiredTaps = 3;

#pragma endregion
};
