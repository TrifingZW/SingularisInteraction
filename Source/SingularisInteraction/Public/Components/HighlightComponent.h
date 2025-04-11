/* =====================================================================
 * InteractionManager.h
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2025 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024-2025 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HighlightComponent.generated.h"

/**
 * 描边高亮抽象组件
 */
UCLASS(Abstract, Blueprintable, ClassGroup=("引力奇点交互插件"), meta=(BlueprintSpawnableComponent))
class SINGULARISINTERACTION_API UHighlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(
		BlueprintNativeEvent,
		BlueprintCallable,
		Category="高亮组件|蓝图虚函数",
		meta = (
			DisplayName = "开启高亮",
			ToolTip = "开启高亮"
		)
	)
	void EnableHighlight(AActor* Actor);

	UFUNCTION(
		BlueprintNativeEvent,
		BlueprintCallable,
		Category="高亮组件|蓝图虚函数",
		meta = (
			DisplayName = "关闭高亮",
			ToolTip = "关闭高亮"
		)
	)
	void DisableHighlight(AActor* Actor);
};
