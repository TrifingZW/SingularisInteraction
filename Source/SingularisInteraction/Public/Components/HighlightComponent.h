/* =====================================================================
 * PostHighlightComponent.h
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2025 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024-2025 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#pragma once

#include <CoreMinimal.h>
#include <Components/PrimitiveComponent.h>
#include <GameFramework/Actor.h>

#include "Components/ActorComponent.h"
#include "HighlightComponent.generated.h"

/**
 * 
 */
UCLASS(
	Blueprintable,
	ClassGroup = ("引力奇点交互插件"),
	meta = (BlueprintSpawnableComponent, DisplayName = "高亮组件")
)
class SINGULARISINTERACTION_API UHighlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:
#pragma region 属性

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "高亮组件|属性",
		meta = (DisplayName = "高亮图元")
	)
	TArray<FComponentReference> HighlightPrimitives{};

#pragma endregion

private:
#pragma region 私有数据成员

	bool bIsEnabled = false;
	TArray<TWeakObjectPtr<UPrimitiveComponent>> Primitives{};

#pragma endregion

public:
#pragma region 常规函数

	UHighlightComponent();
	virtual void BeginPlay() override;

#pragma endregion

#pragma region 公开函数

	UFUNCTION(
		BlueprintCallable,
		Category = "高亮组件|函数",
		meta = (DisplayName = "添加图元")
	)
	void AddPrimitive(UPrimitiveComponent* PrimitiveComponent);

	UFUNCTION(
		BlueprintCallable,
		Category = "高亮组件|函数",
		meta = (DisplayName = "删除图元")
	)
	void RemovePrimitive(UPrimitiveComponent* PrimitiveComponent);

	UFUNCTION(
		BlueprintCallable,
		Category = "高亮组件|函数",
		meta = (DisplayName = "清除图元")
	)
	void ClearPrimitive();

	UFUNCTION(
		BlueprintCallable,
		Category = "高亮组件|函数",
		meta = (DisplayName = "开启高亮")
	)
	void Enable();

	UFUNCTION(
		BlueprintCallable,
		Category = "高亮组件|函数",
		meta = (DisplayName = "关闭高亮")
	)
	void Disable();

#pragma endregion

#pragma region  静态公开函数

	static void SetPrimitiveHighlight(UPrimitiveComponent* PrimitiveComponent, bool bEnable);

#pragma endregion
};
