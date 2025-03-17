/* =====================================================================
 * CircularProgressBar.h
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CircularProgressBar.generated.h"

class UImage;
/**
 * 圆形进度条
 */
UCLASS(Abstract, Blueprintable)
class SINGULARISINTERACTION_API UCircularProgressBar : public UUserWidget
{
	GENERATED_BODY()

public:
	// 关键控件绑定（在蓝图中实际绑定）
	UPROPERTY(BlueprintReadWrite,
		Category = "UI",
		meta = (BindWidget, EditHide))
	UImage* Image;

	UFUNCTION(BlueprintImplementableEvent,
		BlueprintCallable,
		Category = "UI事件",
		meta = (
			DisplayName = "设置百分比",
			ToolTip = "当玩家长按时会调用此函数，并且会给一个进度值，你需要做的是设置进度条的进度"
		))
	void SetPercent(float Percent);
};
