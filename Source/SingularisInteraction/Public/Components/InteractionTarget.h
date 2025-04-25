/* =====================================================================
 * InteractionTarget.h
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2025 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024-2025 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#pragma once

#include <CoreMinimal.h>
#include <Components/ActorComponent.h>
#include <Components/ShapeComponent.h>

#include "Interfaces/InteractionInterface.h"
#include "Systems/InteractionConfig.h"
#include "InteractionTarget.generated.h"

class UHighlightComponent;
class UInteractionTarget;
class UInteractionTargetWidget;
class UInteractionManager;
class UWidgetComponent;

#pragma region 委托签名

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionTargetReadySignature);

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
UCLASS(
	Blueprintable,
	ClassGroup = ("引力奇点交互插件"),
	meta = (BlueprintSpawnableComponent, DisplayName = "交互目标")
)
class SINGULARISINTERACTION_API UInteractionTarget : public UActorComponent, public IInteractionInterface
{
	GENERATED_BODY()

public:
#pragma region 持有实例

	UPROPERTY(
		BlueprintReadOnly,
		Category = "交互目标|持有实例",
		meta = (DisplayName = "高亮组件")
	)
	UHighlightComponent* HighlightComponent = nullptr;

	UPROPERTY(
		BlueprintReadOnly,
		Category = "交互目标|持有实例",
		meta = (DisplayName = "控件组件")
	)
	UWidgetComponent* WidgetComponent = nullptr;

#pragma endregion

#pragma region 属性

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "交互目标|属性",
		meta = (DisplayName = "交互标题")
	)
	FText InteractionTitle = FText::FromString(TEXT("物品交互"));

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "交互目标|属性",
		meta = (DisplayName = "交互图元")
	)
	TArray<FComponentReference> InteractionPrimitives{};

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "交互目标|属性",
		meta = (DisplayName = "交互配置")
	)
	FInteractionConfig InteractionConfig{};

	/*UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "交互目标|属性|交互优先级",
		meta = (DisplayName = "启用交互优先级")
	)
	bool bUseInteractionPriority = false;

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "交互目标|属性|交互优先级",
		meta = (DisplayName = "交互优先级", EditCondition = "bUseInteractionPriority")
	)
	int32 InteractionPriority = 0;*/

	/*UPROPERTY(EditAnywhere,
		Category = "交互选项",
		meta = (
			DisplayName = "轮廓高亮颜色",
			ToolTip = "物体出现在玩家视线上时触发高亮效果的颜色"
		))
	FColor HighlightColor = FColor::Yellow;*/

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "交互目标|属性|轮廓高亮",
		meta = (DisplayName = "启用轮廓高亮")
	)
	bool bIsHighlight = false;

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "交互目标|属性|轮廓高亮",
		meta = (DisplayName = "高亮图元", EditCondition = "bIsHighlight")
	)
	TArray<FComponentReference> HighlightPrimitives{};

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "交互目标|属性|区域提示",
		meta = (DisplayName = "启用区域提示")
	)
	bool bIsPrompt = false;

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "交互目标|属性|区域提示",
		meta = (DisplayName = "提示区域", EditCondition = "bIsPrompt")
	)
	FComponentReference PromptAreaReference{};

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "交互目标|属性|调试设置",
		meta = (DisplayName = "调试输出")
	)
	bool bInteractionDebugOutput = false;

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "交互目标|属性|调试设置",
		meta = (DisplayName = "调试绘制")
	)
	bool bInteractionDebugDraw = false;

#pragma endregion

#pragma region 子类引用

	UPROPERTY(
		EditDefaultsOnly,
		Category = "交互目标|子类引用",
		meta = (DisplayName = "交互目标控件类")
	)
	TSubclassOf<UInteractionTargetWidget> InteractionTargetWidgetClass = nullptr;

#pragma endregion

#pragma region 事件分发器

	UPROPERTY(
		BlueprintAssignable,
		Category = "交互目标|事件分发器",
		meta = (DisplayName = "交互目标准备就绪时触发")
	)
	FOnInteractionTargetReadySignature OnInteractionTargetReadyEvent{};

	UPROPERTY(
		BlueprintAssignable,
		Category = "交互目标|事件分发器",
		meta = (DisplayName = "交互时触发")
	)
	FOnInteractionSignature OnInteractionEvent{};

	UPROPERTY(
		BlueprintAssignable,
		Category = "交互目标|事件分发器",
		meta = (DisplayName = "被注视时触发")
	)
	FOnBeginFocusedSignature OnBeginFocusedEvent{};

	UPROPERTY(
		BlueprintAssignable,
		Category = "交互目标|事件分发器",
		meta = (DisplayName = "注视结束时触发")
	)
	FOnEndFocusedSignature OnEndFocusedEvent{};

	UPROPERTY(
		BlueprintAssignable,
		Category = "交互目标|事件分发器",
		meta = (DisplayName = "玩家进入提示区域时触发")
	)
	FOnPlayersEnterPromptAreaSignature OnPlayersEnterPromptAreaEvent{};

	UPROPERTY(
		BlueprintAssignable,
		Category = "交互目标|事件分发器",
		meta = (DisplayName = "玩家离开提示区域时触发")
	)
	FOnPlayerLeavingPromptAreaSignature OnPlayerLeavingPromptAreaEvent{};

#pragma endregion

private:
#pragma region 私有数据成员

	TWeakObjectPtr<UShapeComponent> PromptArea = nullptr;
	bool bBlockInteraction = true;

#pragma endregion

public:
#pragma region 常规函数

	UInteractionTarget();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma endregion

#pragma region IInteractableInterface 接口函数

	virtual void OnBeginHover_Implementation(AActor* Interactor, FVector_NetQuantize ImpactPoint) override;
	virtual void OnEndHover_Implementation(AActor* Interactor, FVector_NetQuantize ImpactPoint) override;
	virtual void OnInteract_Implementation(AActor* Interactor, FVector_NetQuantize ImpactPoint, const FInputActionValue& Value) override;

#pragma endregion

#pragma region 公开函数

	UFUNCTION(
		BlueprintCallable,
		Category = "交互目标|函数",
		meta = (DisplayName = "交互状态")
	)
	bool IsBlockInteraction() const;

	UFUNCTION(
		BlueprintCallable,
		Category = "交互目标|函数",
		meta = (DisplayName = "阻断交互")
	)
	void BlockInteraction();

	UFUNCTION(
		BlueprintCallable,
		Category = "交互目标|函数",
		meta = (DisplayName = "启用交互")
	)
	void UnlockInteraction();

#pragma endregion

private:
#pragma region 事件回调函数

	UFUNCTION()
	void OnPromptAreaBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnPromptRangeAreaOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

#pragma endregion

#pragma region 私有函数

	void DrawDebugRange(UShapeComponent* DebugShapeComponent, FColor Color, float Duration) const;

#pragma endregion
};
