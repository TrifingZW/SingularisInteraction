/* =====================================================================
 * InteractionType.h
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2025 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024-2025 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#pragma once

#include "CoreMinimal.h"
#include "InteractionType.generated.h"

/**
 * 交互类型
 */
UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	// 基础类型
	Press UMETA(DisplayName = "按下触发"), // 按下瞬间触发
	Release UMETA(DisplayName = "释放触发"), // 松开按键时触发

	// 时间敏感类型
	Hold UMETA(DisplayName = "长按触发"), // 按住超过阈值后触发
	DoubleTap UMETA(DisplayName = "双击触发"), // 快速连续两次点击
	MultiTap UMETA(DisplayName = "连击触发"), // 指定时间内多次点击
};
