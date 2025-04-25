/* =====================================================================
 * RingSight.h
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2025 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024-2025 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#pragma once

#include <CoreMinimal.h>
#include <Blueprint/UserWidget.h>

#include "RingSight.generated.h"

class UImage;

/**
 * 圆环准星
 */
UCLASS(Abstract, Blueprintable)
class SINGULARISINTERACTION_API URingSight : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(
		BlueprintReadWrite,
		Category = "圆环准星",
		meta = (BindWidget)
	)
	UImage* Image;

	UFUNCTION(
		BlueprintImplementableEvent,
		BlueprintCallable,
		Category = "圆环准星",
		meta = (DisplayName = "填充圆环")
	)
	void FillRing();

	UFUNCTION(
		BlueprintImplementableEvent,
		BlueprintCallable,
		Category = "圆环准星",
		meta = (DisplayName = "清空圆环")
	)
	void EmptyRing();
};
