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

class UInputMappingContext;
class UInputAction;
class IInteractableInterface;
struct FInputActionValue;

UCLASS(ClassGroup=("引力奇点交互插件"), meta=(BlueprintSpawnableComponent, AllowedClasses="/Script/Engine.PlayerController"))
class SINGULARISINTERACTION_API UInteractionManager : public USceneComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,
		Category = "交互选项",
		meta = (
			DisplayName = "交互距离",
			ToolTip = "玩家与物品之间的最大交互距离"
		))
	float InteractionDistance = 200.f;

	/** 交互 InputMappingContext */
	UPROPERTY(EditAnywhere,
		BlueprintReadOnly,
		Category = Input,
		meta = (
			DisplayName = "交互IMC",
			ToolTip = "交互组件的输入映射上下文"
		))
	UInputMappingContext* InteractiveInputMappingContext = nullptr;

	/** 交互 InputAction */
	UPROPERTY(EditAnywhere,
		BlueprintReadOnly,
		Category = Input,
		meta = (
			DisplayName = "交互IA",
			ToolTip = "给交互事件绑定的输入动作"
		))
	UInputAction* InteractiveInputAction = nullptr;

	UPROPERTY(EditAnywhere,
		Category = "Input",
		meta = (
			DisplayName = "输入优先级",
			ToolTip = "决定输入的先后顺序"
		))
	int32 InputPriority = 0;

private:
	TWeakObjectPtr<APlayerController> PlayerController = nullptr;

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

private:
	UPROPERTY()
	TScriptInterface<IInteractableInterface> CurrentInteractable;

	void UpdateInteractionTarget();
	void BindInput();
	void HandleInput(const FInputActionValue& Value);
};
