/* =====================================================================
 * CircularProgressBar.h
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2025 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024-2025 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#pragma once

#include <CoreMinimal.h>
#include <Blueprint/UserWidget.h>

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
	UPROPERTY(
		BlueprintReadWrite,
		Category = "圆形进度条",
		meta = (BindWidget)
	)
	UImage* Image;

	UFUNCTION(
		BlueprintImplementableEvent,
		BlueprintCallable,
		Category = "圆形进度条",
		meta = (DisplayName = "设置百分比")
	)
	void SetPercent(float Percent);
};
