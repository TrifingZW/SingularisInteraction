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
#include "Components/SceneComponent.h"
#include "Engine/NetSerialization.h"
#include "InteractionManager.generated.h"

enum class EInteractionType : uint8;
class UInteractionManagerWidget;
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

/**
 * 交互管理器
 */
UCLASS(Blueprintable, ClassGroup=("引力奇点交互插件"), meta=(BlueprintSpawnableComponent))
class SINGULARISINTERACTION_API UInteractionManager : public USceneComponent
{
	GENERATED_BODY()

public:
#pragma region 交互管理器持有实例

	UPROPERTY(
		BlueprintReadOnly,
		Category = "交互管理器|持有实例",
		meta = (
			EditHide,
			DisplayName = "屏幕控件",
			ToolTip = "交互管理器持有的屏幕控件"
		)
	)
	UInteractionManagerWidget* InteractionManagerWidget = nullptr;

#pragma endregion

#pragma region 交互管理器属性

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "交互管理器|属性",
		meta = (
			DisplayName = "按键提示",
			ToolTip = "当玩家视线内有交互物品时显示的按键提示"
		)
	)
	FText KeyPrompt = FText::FromString(TEXT("E"));

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "交互管理器|属性",
		meta = (
			DisplayName = "交互距离",
			ToolTip = "玩家与物品之间的最大交互距离"
		)
	)
	float InteractionDistance = 200.f;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "交互管理器|属性",
		meta = (
			DisplayName = "输入优先级",
			ToolTip = "决定输入的先后顺序"
		)
	)
	int32 InputPriority = 0;

#pragma endregion

#pragma region 交互管理器输入

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "交互管理器|输入",
		meta = (
			DisplayName = "交互IMC",
			ToolTip = "交互组件的输入映射上下文"
		)
	)
	UInputMappingContext* InteractiveInputMappingContext = nullptr;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "交互管理器|输入",
		meta = (
			DisplayName = "交互IA",
			ToolTip = "给交互事件绑定的输入动作"
		)
	)
	UInputAction* InteractiveInputAction = nullptr;

#pragma endregion

#pragma region 交互管理器子类引用

	UPROPERTY(
		EditDefaultsOnly,
		Category = "交互管理器|子类引用",
		meta = (
			DisplayName = "交互控件类",
			ToolTip = "要绘制的交互的控件类"
		)
	)
	TSubclassOf<UInteractionManagerWidget> InteractionWidgetClass;

#pragma endregion

#pragma region 交互管理器公开变量

	TWeakObjectPtr<APlayerController> PlayerController = nullptr;
	TWeakObjectPtr<UInteractionTarget> CurrentInteractionTarget;

#pragma endregion

private:
#pragma region 交互管理器私有变量

	FTimerHandle HoldTimerHandle{};
	float HoldProgress{};
	float HoldTotalDuration{};
	bool bCanInteract = true;
	bool IsProgress{};
	FVector_NetQuantize CurrentHitImpactPoint{};


#pragma endregion

public:
#pragma region 公开常规函数

	UInteractionManager();
	virtual void OnRegister() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma endregion

protected:
#pragma region 保护常规函数

	virtual void BeginPlay() override;

#pragma endregion

#pragma region 交互管理器保护函数

	void UpdateInteractionTarget();
	void UpdateInteractionWidget();
	void UpdateHoldProgress();
	void UpdateHintText(const UInteractionTarget* InteractionTarget) const;

	/*TWeakObjectPtr<UInteractionTarget> FindBestInteractable(
		const TArray<UInteractionTarget*>& Candidates,
		const FVector& CameraLocation,
		const FRotator& CameraRotation,
		const FVector& HitLocation
	) const;*/
	void CreateInteractionWidget();

#pragma endregion

#pragma region 输入绑定函数

	void BindInput();
	void HandleTriggered(const FInputActionValue& Value);
	void HandleCompleted(const FInputActionValue& Value);
	void HandleHold();

#pragma endregion

public:
#pragma region 交互管理器公有函数

	UFUNCTION(
		BlueprintCallable,
		Category="交互管理器|函数",
		meta = (
			DisplayName = "关闭交互",
			ToolTip = "调用此函数时，交换管理器将不可交互"
		)
	)
	void DisableInteraction();

	UFUNCTION(
		BlueprintCallable,
		Category="交互管理器|函数",
		meta = (
			DisplayName = "启用交互",
			ToolTip = "调用此函数时，交换管理器将可交互"
		)
	)
	void EnableInteraction();

#pragma endregion

	static FText GetInteractionTypeText(EInteractionType Type);
};

#undef LOCTEXT_NAMESPACE
