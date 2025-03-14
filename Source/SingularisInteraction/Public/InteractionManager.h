/* =====================================================================
 * InteractionManager.h
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "InteractionManager.generated.h"

enum class EInteractionType : uint8;
class UInteractionWidget;
class UInteractionTarget;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

// 在代码文件顶部定义命名空间（避免污染全局）
#define LOCTEXT_NAMESPACE "InteractionManager"

// 定义本地化文本键（方便后续维护和多语言支持）
const FText PressActionText = LOCTEXT("PressAction", "按下");
const FText ReleaseActionText = LOCTEXT("ReleaseAction", "松开");
const FText HoldActionText = LOCTEXT("HoldAction", "长按");
const FText DoubleTapActionText = LOCTEXT("DoubleTapAction", "双击");
const FText MultiTapActionText = LOCTEXT("MultiTapAction", "狂按");

UCLASS(ClassGroup=("引力奇点交互插件"), meta=(BlueprintSpawnableComponent, AllowedClasses="/Script/Engine.PlayerController"))
class SINGULARISINTERACTION_API UInteractionManager : public USceneComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "交互实例", meta=(EditHide))
	UInteractionWidget* InteractionWidget; // 交互 UI 实例

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "交互选项",
		meta = (
			DisplayName = "按键提示",
			ToolTip = "当玩家视线内有交互物品时显示的按键提示"
		))
	FText KeyPrompt = FText::FromString(TEXT("E"));

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "交互选项",
		meta = (
			DisplayName = "交互距离",
			ToolTip = "玩家与物品之间的最大交互距离"
		))
	float InteractionDistance = 200.f;

	/** 交互 InputMappingContext */
	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "交互选项",
		meta = (
			DisplayName = "交互IMC",
			ToolTip = "交互组件的输入映射上下文"
		))
	UInputMappingContext* InteractiveInputMappingContext = nullptr;

	/** 交互 InputAction */
	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "交互选项",
		meta = (
			DisplayName = "交互IA",
			ToolTip = "给交互事件绑定的输入动作"
		))
	UInputAction* InteractiveInputAction = nullptr;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "交互选项",
		meta = (
			DisplayName = "输入优先级",
			ToolTip = "决定输入的先后顺序"
		))
	int32 InputPriority = 0;

	UPROPERTY(EditDefaultsOnly,
		Category = "交互选项",
		meta = (
			DisplayName = "交互UI类",
			ToolTip = "要绘制的交互的UI类"
		))
	TSubclassOf<UInteractionWidget> InteractionWidgetClass;

	TWeakObjectPtr<UInteractionTarget> CurrentInteractionTarget;

private:
	TWeakObjectPtr<APlayerController> PlayerController = nullptr;
	FTimerHandle HoldTimerHandle;
	float HoldProgress;
	float HoldTotalDuration;

public:
	// 设置该组件属性的默认值
	UInteractionManager();
	//游戏开始时调用
	virtual void OnRegister() override;
	// 每一帧调用
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// 当游戏开始时调用
	virtual void BeginPlay() override;

	void UpdateInteractionTarget();
	void UpdateInteractionWidget();
	void UpdateHoldProgress();
	void UpdateHintText(const UInteractionTarget* InteractionTarget) const;

	TWeakObjectPtr<UInteractionTarget> FindBestInteractable(
		const TArray<UInteractionTarget*>& Candidates,
		const FVector& CameraLocation,
		const FRotator& CameraRotation,
		const FVector& HitLocation
	) const;
	void CreateInteractionWidget();

	void BindInput();
	void HandleTriggered(const FInputActionValue& Value);
	void HandleCompleted(const FInputActionValue& Value);
	void HandleHold();

	static FText GetInteractionTypeText(EInteractionType Type);
};

#undef LOCTEXT_NAMESPACE
