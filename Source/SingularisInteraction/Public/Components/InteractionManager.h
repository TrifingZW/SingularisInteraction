/* =====================================================================
 * InteractionManager.h
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2025 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024-2025 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#pragma once

#include <CoreMinimal.h>
#include <Engine/NetSerialization.h>

#include "Components/ActorComponent.h"
#include "InteractionManager.generated.h"

enum class EInteractionType : uint8;
struct FInputActionValue;
class UInteractionManager;
class UInteractionManagerWidget;
class UInteractionTarget;
class UInputMappingContext;
class UInputAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionManagerReadySignature);

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
UCLASS(
	Blueprintable,
	ClassGroup = ("引力奇点交互插件"),
	meta = (BlueprintSpawnableComponent, DisplayName = "交互管理器")
)
class SINGULARISINTERACTION_API UInteractionManager : public UActorComponent
{
	GENERATED_BODY()

public:
#pragma region 持有实例

	UPROPERTY(
		BlueprintReadOnly,
		Category = "交互管理器|持有实例",
		meta = (DisplayName = "屏幕控件")
	)
	UInteractionManagerWidget* InteractionManagerWidget = nullptr;

#pragma endregion

#pragma region 属性

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "交互管理器|属性",
		meta = (DisplayName = "按键提示")
	)
	FText KeyPrompt = FText::FromString(TEXT("E"));

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "交互管理器|属性",
		meta = (DisplayName = "交互距离")
	)
	float InteractionDistance = 200.f;

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "交互管理器|属性",
		meta = (DisplayName = "输入优先级")
	)
	int32 InputPriority = 0;

#pragma endregion

#pragma region 输入

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "交互管理器|输入",
		meta = (DisplayName = "交互 InputMappingContext")
	)
	UInputMappingContext* InteractiveInputMappingContext = nullptr;

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "交互管理器|输入",
		meta = (DisplayName = "交互 InputAction")
	)
	UInputAction* InteractiveInputAction = nullptr;

#pragma endregion

#pragma region 子类引用

	UPROPERTY(
		EditDefaultsOnly,
		Category = "交互管理器|子类引用",
		meta = (DisplayName = "交互控件类")
	)
	TSubclassOf<UInteractionManagerWidget> InteractionWidgetClass = nullptr;

#pragma endregion

#pragma region 事件分发器

	UPROPERTY(
		BlueprintAssignable,
		Category = "交互管理器|事件分发器",
		meta = (DisplayName = "交互管理器准备就绪时触发")
	)
	FOnInteractionManagerReadySignature OnInteractionManagerReadyEvent{};

#pragma endregion

#pragma region 公开数据成员

	TWeakObjectPtr<APlayerController> PlayerController = nullptr;
	TWeakObjectPtr<UInteractionTarget> CurrentInteractionTarget = nullptr;

#pragma endregion

private:
#pragma region 私有数据成员

	FTimerHandle HoldTimerHandle{};
	float HoldProgress = 0.0f;
	float HoldTotalDuration = 0.0f;
	bool bCanInteraction = false;
	bool IsProgress{};
	FVector_NetQuantize CurrentHitImpactPoint{};

#pragma endregion

public:
#pragma region 常规函数

	UInteractionManager();
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma endregion

#pragma region 公开函数

	UFUNCTION(
		BlueprintCallable,
		Category = "交互管理器|函数",
		meta = (DisplayName = "关闭交互")
	)
	void DisableInteraction();

	UFUNCTION(
		BlueprintCallable,
		Category = "交互管理器|函数",
		meta = (DisplayName = "启用交互")
	)
	void EnableInteraction();

	UFUNCTION(
		BlueprintCallable,
		Category = "交互管理器|函数",
		meta = (DisplayName = "显示交互管理器控件")
	)
	void ShowInteractionManagerWidget() const;

	UFUNCTION(
		BlueprintCallable,
		Category = "交互管理器|函数",
		meta = (DisplayName = "隐藏交互管理器控件")
	)
	void HideInteractionManagerWidget() const;

	UFUNCTION(
		BlueprintCallable,
		Category = "交互管理器|函数",
		meta = (DisplayName = "进入特殊状态")
	)
	void EnterSpecialState() const;

	UFUNCTION(
		BlueprintCallable,
		Category = "交互管理器|函数",
		meta = (DisplayName = "退出特殊状态")
	)
	void ExitSpecialState() const;

	/*UFUNCTION(
		BlueprintCallable,
		Category = "交互管理器|函数",
		meta = (DisplayName = "设置交互管理器控件")
	)
	void SetInteractionManagerWidget(UInteractionManagerWidget* NewInteractionManagerWidget);

	UFUNCTION(
		BlueprintCallable,
		Category = "交互管理器|函数",
		meta = (DisplayName = "重置交互管理器控件")
	)
	void ResetInteractionManagerWidget();*/

#pragma endregion

private:
#pragma region 输入绑定函数

	void BindInput();
	void HandleTriggered(const FInputActionValue& Value);
	void HandleCompleted(const FInputActionValue& Value);
	void HandleHold();

#pragma endregion

#pragma region 私有函数

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

#pragma region 私有静态函数

	static FText GetInteractionTypeText(EInteractionType Type);

#pragma endregion
};

#undef LOCTEXT_NAMESPACE
