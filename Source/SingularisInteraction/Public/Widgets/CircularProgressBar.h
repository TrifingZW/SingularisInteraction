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
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, EditHide))
	UImage* Image;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetPercent(float Percent);
};
