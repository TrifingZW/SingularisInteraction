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
); // 声明动态多播委托

UCLASS(ClassGroup=("引力奇点交互插件"), meta=(BlueprintSpawnableComponent))
class SINGULARISINTERACTION_API UInteractionTarget : public USceneComponent, public IInteractableInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "交互选项",
		meta = (
			DisplayName = "交互提示",
			ToolTip = "可以交互时显示的提示文字"
		))
	FText PromptText = FText::FromString("PressEToInteract");

	UPROPERTY(VisibleAnywhere, Category = "交互选项")
	UHighlightComponent* HighlightComponent = nullptr;

	UPROPERTY(EditAnywhere,
		Category = "交互选项",
		meta = (
			DisplayName = "轮廓高亮",
			ToolTip = "是否在物体出现在玩家视线范围内时触发高亮效果"
		))
	bool Highlight = true;

	/*UPROPERTY(EditAnywhere,
		Category = "交互选项",
		meta = (
			DisplayName = "轮廓高亮颜色",
			ToolTip = "物体出现在玩家视线范围内时触发高亮效果的颜色"
		))
	FColor HighlightColor = FColor::Yellow;*/

	UPROPERTY(EditAnywhere,
		Category = "交互选项",
		meta = (
			DisplayName = "调试输出",
			ToolTip = "是否输出调试信息"
		))
	bool DebugOutput = false;

	UPROPERTY(EditAnywhere,
		Category = "交互选项",
		meta = (
			DisplayName = "调试绘制",
			ToolTip = "是否绘制提示范围"
		))
	bool DebugDraw = false;

	UPROPERTY(EditAnywhere,
		Category = "交互选项",
		meta = (
			DisplayName = "调试绘制颜色",
			ToolTip = "调试绘制时绘制的颜色"
		))
	FColor DebugDrawColor = FColor::Green;

	UPROPERTY(EditAnywhere,
		Category = "交互选项",
		meta = (
			DisplayName = "提示范围",
			ToolTip = "如果玩家在此范围内会绘制交互提示UI"
		))
	UShapeComponent* PromptRange = nullptr;

	UPROPERTY(EditAnywhere,
		Category = "交互选项",
		meta = (
			DisplayName = "提示UI",
			ToolTip = "要绘制的交互提示的UI类"
		))
	TSubclassOf<UUserWidget> PromptWidgetClass = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "交互选项")
	UWidgetComponent* WidgetComponent = nullptr;

	// 暴露给蓝图的事件调度器
	UPROPERTY(
		BlueprintAssignable,
		Category = "交互事件",
		meta = (
			DisplayName = "交互时触发",
			ToolTip = "当玩家与对象发生交互行为时触发此事件"
		))
	FOnInteractionSignature OnInteraction{};

private:
	UPROPERTY(VisibleAnywhere, Category = "交互选项")
	UUserWidget* Widget = nullptr; // Widget 实例

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

	/** 当玩家执行交互时触发 */
	UFUNCTION(BlueprintNativeEvent, Category = "交互事件")
	void ExecuteInteraction();
	virtual void ExecuteInteraction_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "交互事件")
	void OnPlayersEnterPromptArea(UPrimitiveComponent* OverlappedComponent,
	                              APawn* Pawn,
	                              UPrimitiveComponent* OtherComp,
	                              int32 OtherBodyIndex,
	                              bool bFromSweep,
	                              const FHitResult& SweepResult);
	UFUNCTION(BlueprintNativeEvent, Category = "交互事件")
	void OnPlayerLeavingPromptArea(UPrimitiveComponent* OverlappedComponent,
	                               APawn* Pawn,
	                               UPrimitiveComponent* OtherComp,
	                               int32 OtherBodyIndex);

private:
	// 在外部开始重叠
	UFUNCTION()
	void OnPromptRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	                               AActor* OtherActor,
	                               UPrimitiveComponent* OtherComp,
	                               int32 OtherBodyIndex,
	                               bool bFromSweep,
	                               const FHitResult& SweepResult);
	// 在外部结束重叠
	UFUNCTION()
	void OnPromptRangeEndOverlap(UPrimitiveComponent* OverlappedComponent,
	                             AActor* OtherActor,
	                             UPrimitiveComponent* OtherComp,
	                             int32 OtherBodyIndex);

	/** 调试绘制交互范围 */
	void DrawDebugRange(UShapeComponent* DebugShapeComponent, FColor Color, float Duration) const;

	void AddWidgetToScreen();
	void RemoveWidgetFromScreen();

public:
	virtual void OnBeginHover_Implementation(AActor* Interactor) override;
	virtual void OnEndHover_Implementation(AActor* Interactor) override;
	virtual void OnInteract_Implementation(AActor* Interactor, const FInputActionValue& Value) override;
};
