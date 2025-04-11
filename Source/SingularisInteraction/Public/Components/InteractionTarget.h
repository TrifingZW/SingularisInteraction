/* =====================================================================
 * InteractionTarget.h
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2025 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024-2025 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#pragma once

#include "CoreMinimal.h"
#include "HighlightComponent.h"
#include "InteractionConfig.h"
#include "Components/SceneComponent.h"
#include "Components/ShapeComponent.h"
#include "Interfaces/InteractableInterface.h"
#include "InteractionTarget.generated.h"

class IHighlightInterface;
class UInteractionTargetWidget;
class UWidgetComponent;
class UInteractionManager;

#pragma region 委托签名

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FOnInteractionSignature,
	APlayerController*,
	PlayerController,
	ACharacter*,
	Character,
	FVector_NetQuantize,
	ImpactPoint,
	const FInputActionValue&,
	Value
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnBeginFocusedSignature,
	APlayerController*,
	PlayerController,
	ACharacter*,
	Character,
	FVector_NetQuantize,
	ImpactPoint
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnEndFocusedSignature,
	APlayerController*,
	PlayerController,
	ACharacter*,
	Character,
	FVector_NetQuantize,
	ImpactPoint
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

#pragma endregion

/**
 * 交互目标
 */
UCLASS(Blueprintable, ClassGroup=("引力奇点交互插件"), meta=(BlueprintSpawnableComponent))
class SINGULARISINTERACTION_API UInteractionTarget : public USceneComponent, public IInteractableInterface
{
	GENERATED_BODY()

public:
#pragma region 交互目标持有实例

	UPROPERTY(
		BlueprintReadOnly,
		Category = "交互目标|持有实例",
		meta = (
			EditHide,
			DisplayName = "高亮组件",
			ToolTip = "交互目标持有的高亮组件"
		)
	)
	UHighlightComponent* HighlightComponent = nullptr;

	UPROPERTY(
		BlueprintReadOnly,
		Category = "交互目标|持有实例",
		meta = (
			EditHide,
			DisplayName = "控件组件",
			ToolTip = "交互目标持有的控件组件"
		)
	)
	UWidgetComponent* WidgetComponent = nullptr;

#pragma endregion

#pragma region 交互目标属性

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "交互目标|属性",
		meta = (
			DisplayName = "交互标题",
			ToolTip = "该交互的标题"
		)
	)
	FText InteractionTitle = FText::FromString(TEXT("物品拾取"));

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "交互目标|属性",
		meta = (
			DisplayName = "交互配置",
			ToolTip = "该交互的配置"
		)
	)
	FInteractionConfig InteractionConfig{};

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "交互目标|属性",
		meta = (
			DisplayName = "启用交互优先级",
			ToolTip = "启用交互优先级"
		)
	)
	bool bUseInteractionPriority = false;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "交互目标|属性",
		meta = (
			DisplayName = "交互优先级",
			ToolTip = "当一个物体拥有多个交互目标组件时，优先级高的组件会被优先执行交互事件"
		)
	)
	int32 InteractionPriority = 0;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "交互目标|属性",
		meta = (
			DisplayName = "轮廓高亮",
			ToolTip = "是否在物体出现在玩家视线上时触发高亮效果"
		)
	)
	bool bInteractionHighlight = true;

	/*UPROPERTY(EditAnywhere,
		Category = "交互选项",
		meta = (
			DisplayName = "轮廓高亮颜色",
			ToolTip = "物体出现在玩家视线上时触发高亮效果的颜色"
		))
	FColor HighlightColor = FColor::Yellow;*/

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "交互目标|属性",
		meta = (
			DisplayName = "调试输出",
			ToolTip = "是否输出调试信息"
		)
	)
	bool bInteractionDebugOutput = false;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "交互目标|属性",
		meta = (
			DisplayName = "调试绘制",
			ToolTip = "是否绘制提示区域"
		)
	)
	bool bInteractionDebugDraw = false;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "交互目标|属性",
		meta = (
			DisplayName = "交互区域",
			ToolTip = "射线检测区域"
		)
	)
	UShapeComponent* InteractiveRange = nullptr;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "交互目标|属性",
		meta = (
			DisplayName = "提示区域",
			ToolTip = "如果玩家在此区域内会绘制交互提示UI"
		)
	)
	UShapeComponent* PromptRange = nullptr;

#pragma endregion

#pragma region 交互目标子类引用

	UPROPERTY(
		EditDefaultsOnly,
		Category = "交互目标|子类引用",
		meta = (
			DisplayName = "提示Widget",
			ToolTip = "要绘制的交互提示的Widget类"
		)
	)
	TSubclassOf<UInteractionTargetWidget> PromptWidgetClass = nullptr;

	UPROPERTY(
		EditDefaultsOnly,
		Category = "交互目标|子类引用",
		meta = (
			// MustImplement="HighlightInterface",
			DisplayName = "高亮组件",
			ToolTip = "高亮组件类"
		)
	)
	TSubclassOf<UHighlightComponent> HighlightComponentClass = nullptr;

#pragma endregion

#pragma region 交互目标事件分发器

	UPROPERTY(
		BlueprintAssignable,
		Category = "交互目标|事件分发器",
		meta = (
			DisplayName = "交互时触发",
			ToolTip = "当玩家与对象发生交互行为时触发此事件"
		)
	)
	FOnInteractionSignature OnInteractionEvent{};

	UPROPERTY(
		BlueprintAssignable,
		Category = "交互目标|事件分发器",
		meta = (
			DisplayName = "被注视时触发",
			ToolTip = "当玩家注视该物体时触发此事件"
		)
	)
	FOnBeginFocusedSignature OnBeginFocusedEvent{};

	UPROPERTY(
		BlueprintAssignable,
		Category = "交互目标|事件分发器",
		meta = (
			DisplayName = "注视结束时触发",
			ToolTip = "当玩家结束注视该物体时触发此事件"
		)
	)
	FOnEndFocusedSignature OnEndFocusedEvent{};

	UPROPERTY(
		BlueprintAssignable,
		Category = "交互目标|事件分发器",
		meta = (
			DisplayName = "玩家进入提示区域时触发",
			ToolTip = "当玩家进入提示区域时触发此事件，提示区域参考交互选项的 PromptRange"
		)
	)
	FOnPlayersEnterPromptAreaSignature OnPlayersEnterPromptAreaEvent{};

	UPROPERTY(
		BlueprintAssignable,
		Category = "交互目标|事件分发器",
		meta = (
			DisplayName = "玩家离开提示区域时触发",
			ToolTip = "当玩家离开提示区域时触发此事件，提示区域参考交互选项的 PromptRange"
		)
	)
	FOnPlayerLeavingPromptAreaSignature OnPlayerLeavingPromptAreaEvent{};

#pragma endregion

#pragma region 交互目标公开变量

	bool bBlockInteraction = false;

#pragma endregion

private:
#pragma region 交互目标私有变量

	TWeakObjectPtr<UInteractionManager> InteractionManager = nullptr;

#pragma endregion

public:
#pragma region 公共常规函数

	UInteractionTarget();
	virtual void OnRegister() override;

#pragma endregion

protected:
#pragma region 保护常规函数

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma endregion

#pragma region 交互目标蓝图虚函数

	UFUNCTION(BlueprintNativeEvent, Category = "交互目标|蓝图虚函数")
	void OnPlayersEnterPromptArea(
		UPrimitiveComponent* OverlappedComponent,
		APawn* Pawn,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	UFUNCTION(BlueprintNativeEvent, Category = "交互目标|蓝图虚函数")
	void OnPlayerLeavingPromptArea(
		UPrimitiveComponent* OverlappedComponent,
		APawn* Pawn,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

#pragma endregion

private:
#pragma region 交互目标事件回调函数

	UFUNCTION(
		Category="交互目标|事件回调函数",
		meta = (
			DisplayName = "当提示范围开始重叠",
			ToolTip = "当提示范围开始重叠"
		)
	)
	void OnPromptRangeBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION(
		Category="交互目标|事件回调函数",
		meta = (
			DisplayName = "当提示范围结束重叠",
			ToolTip = "当提示范围结束重叠"
		)
	)
	void OnPromptRangeEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

#pragma endregion

#pragma region 交互目标私有函数

	void DrawDebugRange(UShapeComponent* DebugShapeComponent, FColor Color, float Duration) const;

#pragma endregion

public:
#pragma region IInteractableInterface 接口函数

	virtual void OnBeginHover_Implementation(AActor* Interactor, FVector_NetQuantize ImpactPoint) override;
	virtual void OnEndHover_Implementation(AActor* Interactor, FVector_NetQuantize ImpactPoint) override;
	virtual void OnInteract_Implementation(AActor* Interactor, FVector_NetQuantize ImpactPoint, const FInputActionValue& Value) override;

#pragma endregion

#pragma region 交互目标公开函数

	UFUNCTION(
		BlueprintCallable,
		Category="交互目标|函数",
		meta = (
			DisplayName = "阻断交互",
			ToolTip = "调用此函数时，属性 bBlockInteraction 将会设置为 true，交互管理器将无法与该组件交互，该组件的事件会被阻塞，并且隐藏提示范围，提示UI等。"
		)
	)
	void BlockInteraction();

	UFUNCTION(
		BlueprintCallable,
		Category="交互目标|函数",
		meta = (
			DisplayName = "启用交互",
			ToolTip = "调用此函数时，属性 bBlockInteraction 将会设置为 false，交互管理器将可以与该组件交互。"
		)
	)
	void UnlockInteraction();

#pragma endregion
};
