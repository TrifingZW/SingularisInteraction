/* =====================================================================
 * InteractionTarget.h
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#pragma once

#include "CoreMinimal.h"
#include "InteractableInterface.h"
#include "InteractionConfig.h"

#include "Components/SceneComponent.h"
#include "Components/ShapeComponent.h"
#include "Blueprint/UserWidget.h"
#include "InteractionTarget.generated.h"

class UHighlightComponent;
class UWidgetComponent;
class UInteractionManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnInteractionSignature,
	AActor*,
	Interactor,
	const FInputActionValue&,
	Value
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(
	FOnPlayersEnterPromptAreaSignature,
	UPrimitiveComponent*,
	OverlappedComponent,
	APawn*,
	Pawn,
	UPrimitiveComponent*,
	OtherComp,
	int32,
	OtherBodyIndex,
	bool,
	bFromSweep,
	const FHitResult&,
	SweepResult
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FOnPlayerLeavingPromptAreaSignature,
	UPrimitiveComponent*,
	OverlappedComponent,
	APawn*,
	Pawn,
	UPrimitiveComponent*,
	OtherComp,
	int32,
	OtherBodyIndex
);

UCLASS(ClassGroup=("引力奇点交互插件"), meta=(BlueprintSpawnableComponent))
class SINGULARISINTERACTION_API UInteractionTarget : public USceneComponent, public IInteractableInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "交互选项", meta=(EditHide))
	UHighlightComponent* HighlightComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "交互选项", meta=(EditHide))
	UWidgetComponent* WidgetComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "交互选项", meta=(EditHide))
	UUserWidget* Widget = nullptr; // Widget 实例

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "交互选项",
		meta = (
			DisplayName = "交互标题",
			ToolTip = "该交互的标题"
		))
	FText InteractionTitle = FText::FromString(TEXT("物品拾取"));

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "交互选项",
		meta = (
			DisplayName = "交互配置",
			ToolTip = "该交互的配置"
		))
	FInteractionConfig InteractionConfig{};

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "交互选项",
		meta = (
			DisplayName = "启用优先级",
			ToolTip = "启用交互优先级"
		))
	bool UsePriority = false;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "交互选项",
		meta = (
			DisplayName = "交互优先级",
			ToolTip = "当一个物体拥有多个交互目标组件时，优先级高的组件会被优先执行交互事件"
		))
	int32 InteractionPriority = 0;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "交互选项",
		meta = (
			DisplayName = "轮廓高亮",
			ToolTip = "是否在物体出现在玩家视线上时触发高亮效果"
		))
	bool Highlight = true;

	/*UPROPERTY(EditAnywhere,
		Category = "交互选项",
		meta = (
			DisplayName = "轮廓高亮颜色",
			ToolTip = "物体出现在玩家视线上时触发高亮效果的颜色"
		))
	FColor HighlightColor = FColor::Yellow;*/

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "交互选项",
		meta = (
			DisplayName = "调试输出",
			ToolTip = "是否输出调试信息"
		))
	bool DebugOutput = false;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "交互选项",
		meta = (
			DisplayName = "调试绘制",
			ToolTip = "是否绘制提示区域"
		))
	bool DebugDraw = false;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "交互选项",
		meta = (
			DisplayName = "调试绘制颜色",
			ToolTip = "调试绘制时绘制的颜色"
		))
	FColor DebugDrawColor = FColor::Green;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "交互选项",
		meta = (
			DisplayName = "提示区域",
			ToolTip = "如果玩家在此区域内会绘制交互提示UI"
		))
	UShapeComponent* PromptRange = nullptr;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "交互选项",
		meta = (
			DisplayName = "提示UI",
			ToolTip = "要绘制的交互提示的UI类"
		))
	TSubclassOf<UUserWidget> PromptWidgetClass = nullptr;

	// 暴露给蓝图的事件调度器
	UPROPERTY(
		BlueprintAssignable,
		Category = "交互事件",
		meta = (
			DisplayName = "交互时触发",
			ToolTip = "当玩家与对象发生交互行为时触发此事件"
		))
	FOnInteractionSignature OnInteractionEvent{};

	// 暴露给蓝图的事件调度器
	UPROPERTY(
		BlueprintAssignable,
		Category = "交互事件",
		meta = (
			DisplayName = "玩家进入提示区域时触发",
			ToolTip = "当玩家进入提示区域时触发此事件，提示区域参考交互选项的PromptRange"
		))
	FOnPlayersEnterPromptAreaSignature OnPlayersEnterPromptAreaEvent{};

	// 暴露给蓝图的事件调度器
	UPROPERTY(
		BlueprintAssignable,
		Category = "交互事件",
		meta = (
			DisplayName = "玩家离开提示区域时触发",
			ToolTip = "当玩家离开提示区域时触发此事件，提示区域参考交互选项的PromptRange"
		))
	FOnPlayerLeavingPromptAreaSignature OnPlayerLeavingPromptAreaEvent{};

private:
	bool bCanInteract = false;
	TWeakObjectPtr<UInteractionManager> InteractionManager = nullptr;

public:
	// 设置该组件属性的默认值
	UInteractionTarget();

	// 当组件注册时调用
	virtual void OnRegister() override;

	bool IsInteractable() const { return bCanInteract; }

protected:
	// 当游戏开始时调用
	virtual void BeginPlay() override;

	// 每一帧调用
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintNativeEvent, Category = "交互事件")
	void OnPlayersEnterPromptArea(
		UPrimitiveComponent* OverlappedComponent,
		APawn* Pawn,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	UFUNCTION(BlueprintNativeEvent, Category = "交互事件")
	void OnPlayerLeavingPromptArea(
		UPrimitiveComponent* OverlappedComponent,
		APawn* Pawn,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

private:
	// 在外部开始重叠
	UFUNCTION()
	void OnPromptRangeBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	// 在外部结束重叠
	UFUNCTION()
	void OnPromptRangeEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	/** 调试绘制交互区域 */
	void DrawDebugRange(UShapeComponent* DebugShapeComponent, FColor Color, float Duration) const;

	void AddWidgetToScreen();
	void RemoveWidgetFromScreen();

public:
	virtual void OnBeginHover_Implementation(AActor* Interactor) override;
	virtual void OnEndHover_Implementation(AActor* Interactor) override;
	virtual void OnInteract_Implementation(AActor* Interactor, const FInputActionValue& Value) override;
};
