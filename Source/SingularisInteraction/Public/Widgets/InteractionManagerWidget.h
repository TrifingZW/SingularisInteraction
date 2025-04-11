/* =====================================================================
 * InteractionManagerWidget.h
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2025 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024-2025 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionManagerWidget.generated.h"

class UInteractionTarget;
class UCircularProgressBar;
class UImage;
class UTextBlock;

/**
 * 交互管理器 Widget
 */
UCLASS(Abstract, Blueprintable)
class SINGULARISINTERACTION_API UInteractionManagerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
#pragma region 交互管理器控件函数

	UFUNCTION(BlueprintCallable, Category="交互管理器控件|函数")
	void ShowWidget();

	UFUNCTION(BlueprintCallable, Category="交互管理器控件|函数")
	void HideWidget();

#pragma endregion

#pragma region 交互管理器控件蓝图实现函数

	UFUNCTION(
		BlueprintImplementableEvent,
		BlueprintCallable,
		Category="交互管理器控件|蓝图实现函数",
		meta = (
			DisplayName = "显示进度条",
			ToolTip = "显示进度条"
		)
	)
	void ShowProgressBar();

	UFUNCTION(
		BlueprintImplementableEvent,
		BlueprintCallable,
		Category="交互管理器控件|蓝图实现函数",
		meta = (
			DisplayName = "隐藏进度条",
			ToolTip = "隐藏进度条"
		)
	)
	void HideProgressBar();

	UFUNCTION(
		BlueprintImplementableEvent,
		BlueprintCallable,
		Category="交互管理器控件|蓝图实现函数",
		meta = (
			DisplayName = "交互目标改变时触发",
			ToolTip = "交互目标改变时触发"
		)
	)
	void UpdateDetection(UInteractionTarget* InteractionTarget);

	UFUNCTION(
		BlueprintImplementableEvent,
		BlueprintCallable,
		Category="交互管理器控件|蓝图实现函数",
		meta = (
			DisplayName = "无交互目标时触发",
			ToolTip = "交互管理器交互目标离开"
		)
	)
	void NoDetection();

	UFUNCTION(
		BlueprintImplementableEvent,
		BlueprintCallable,
		Category="交互管理器控件|蓝图实现函数",
		meta = (
			DisplayName = "启动特殊状态",
			ToolTip = "启动特殊状态"
		)
	)
	void EnableSpecialState();

	UFUNCTION(
		BlueprintImplementableEvent,
		BlueprintCallable,
		Category="交互管理器控件|蓝图实现函数",
		meta = (
			DisplayName = "关闭特殊状态",
			ToolTip = "关闭特殊状态"
		)
	)
	void DisableSpecialState();

	UFUNCTION(
		BlueprintImplementableEvent,
		BlueprintCallable,
		Category="交互管理器控件|蓝图实现函数",
		meta = (
			DisplayName = "设置提示文本",
			ToolTip = "当玩家的交互目标改变时间会调用此函数，并且会给一个文本，你需要做的是设置提示文本的文本"
		)
	)
	void SetHintText(const FText& NewHintText);

	UFUNCTION(
		BlueprintImplementableEvent,
		BlueprintCallable,
		Category="交互管理器控件|蓝图实现函数",
		meta = (
			DisplayName = "设置长按进度",
			ToolTip = "当玩家长按时会调用此函数，并且会给一个进度值，你需要做的是设置进度条的进度"
		)
	)
	void SetHoldProgress(float Percent);


#pragma endregion
};
