/* =====================================================================
 * InteractionWidget.h
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionWidget.generated.h"

class UCircularProgressBar;
class UImage;
class UTextBlock;

/**
 * 交互 UI 基类
 */
UCLASS(Abstract, Blueprintable)
class SINGULARISINTERACTION_API UInteractionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
#pragma region 交互控件函数

	UFUNCTION(BlueprintCallable, Category="交互控件|函数")
	void ShowWidget();

	UFUNCTION(BlueprintCallable, Category="交互控件|函数")
	void HideWidget();

#pragma endregion

#pragma region 交互控件接口

	UFUNCTION(
		BlueprintImplementableEvent,
		BlueprintCallable,
		Category="交互控件|接口",
		meta = (
			DisplayName = "显示进度条",
			ToolTip = "当玩家实现内有交互目标时会调用此函数，你需要做的是让进度条可视"
		)
	)
	void ShowProgressBar();

	UFUNCTION(
		BlueprintImplementableEvent,
		BlueprintCallable,
		Category="交互控件|接口",
		meta = (
			DisplayName = "隐藏进度条",
			ToolTip = "当玩家实现内没有交互目标时会调用此函数，你需要做的是让进度条隐藏"
		)
	)
	void HideProgressBar();

	UFUNCTION(
		BlueprintImplementableEvent,
		BlueprintCallable,
		Category="交互控件|接口",
		meta = (
			DisplayName = "设置提示文本",
			ToolTip = "当玩家的交互目标改变时间会调用此函数，并且会给一个文本，你需要做的是设置提示文本的文本"
		)
	)
	void SetHintText(const FText& NewHintText);

	UFUNCTION(
		BlueprintImplementableEvent,
		BlueprintCallable,
		Category="交互控件|接口",
		meta = (
			DisplayName = "设置长按进度",
			ToolTip = "当玩家长按时会调用此函数，并且会给一个进度值，你需要做的是设置进度条的进度"
		)
	)
	void SetHoldProgress(float Percent);

#pragma endregion
};
