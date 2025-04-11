/* =====================================================================
 * InteractionTargetWidget.h
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2025 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024-2025 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionTargetWidget.generated.h"

/**
 * 交互目标 Widget
 */
UCLASS(Abstract, Blueprintable)
class SINGULARISINTERACTION_API UInteractionTargetWidget : public UUserWidget
{
	GENERATED_BODY()

public:
#pragma region 交互目标控件蓝图实现函数

	UFUNCTION(
		BlueprintImplementableEvent,
		BlueprintCallable,
		Category="交互目标控件|蓝图实现函数",
		meta = (
			DisplayName = "显示控件",
			ToolTip = "显示控件"
		)
	)
	void ShowWidget();

	UFUNCTION(
		BlueprintImplementableEvent,
		BlueprintCallable,
		Category="交互目标控件|蓝图实现函数",
		meta = (
			DisplayName = "隐藏控件",
			ToolTip = "隐藏控件"
		)
	)
	void HideWidget();

#pragma endregion
};
