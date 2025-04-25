/* =====================================================================
 * HighlightInterface.h
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2025 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024-2025 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#pragma once

#include <CoreMinimal.h>
#include <UObject/Interface.h>

#include "HighlightInterface.generated.h"

// 此类不需要修改
UINTERFACE(MinimalAPI)
class UHighlightInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 高亮接口
 */
class SINGULARISINTERACTION_API IHighlightInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(
		BlueprintNativeEvent,
		Category = "高亮接口",
		meta = (DisplayName = "开启高亮")
	)
	void EnableHighlight(AActor* Actor);

	UFUNCTION(
		BlueprintNativeEvent,
		Category = "高亮接口",
		meta = (DisplayName = "关闭高亮")
	)
	void DisableHighlight(AActor* Actor);
};
