/* =====================================================================
 * InteractionManager.cpp
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#include "InteractionManager.h"

#include "Engine.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InteractableInterface.h"
#include "InteractionTarget.h"
#include "InteractionWidget.h"
#include "TimerManager.h"

#define LOCTEXT_NAMESPACE "InteractionManager"

// 设置该组件属性的默认值
UInteractionManager::UInteractionManager()
{
	// 设置该组件在游戏开始时初始化，并在每一帧进行更新。如果不需要这些功能，
	// 可以关闭它们以提高性能。
	PrimaryComponentTick.bCanEverTick = true; // 启用 Tick

	// 初始化 Hold 值
	HoldProgress = 0.0f;
	HoldTotalDuration = 0.0f;
}

void UInteractionManager::OnRegister()
{
	Super::OnRegister(); // 必须调用父类方法

	// 核心检查逻辑
	PlayerController = Cast<APlayerController>(GetOwner()); // 转换为成员变量赋值
	if (PlayerController == nullptr)
	{
		// 检查是否转换成功
		// 错误处理
		ensureMsgf(
			false,
			TEXT("组件[UMyComponent]必须附加到APlayerController上！当前Owner：%s"),
			*GetOwner()->GetName()
		);
		UE_LOG(
			LogTemp,
			Error,
			TEXT("组件[UMyComponent]被错误附加到[%s]上，该组件只能用于APlayerController！"),
			*GetOwner()->GetName()
		);
		SetComponentTickEnabled(false);
	}
}


// 当游戏开始时调用
void UInteractionManager::BeginPlay()
{
	Super::BeginPlay();

	// 绑定输入
	BindInput();

	// 设置交互 UI 实例
	CreateInteractionWidget();
}


// 每一帧调用
void UInteractionManager::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 更新交互目标
	UpdateInteractionTarget();

	// 更新交互 UI
	UpdateInteractionWidget();
}

void UInteractionManager::UpdateInteractionTarget()
{
	const APlayerController* PC = PlayerController.Get();
	if (!PC)
	{
		return;
	}

	// 获取摄像机参数
	FVector CameraLoc;
	FRotator CameraRot;
	PC->GetPlayerViewPoint(CameraLoc, CameraRot);

	const FVector TraceEnd = CameraLoc + (CameraRot.Vector() * InteractionDistance);

	// 射线检测参数
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(GetOwner()); // 忽略玩家自身
	TraceParams.bTraceComplex = true; // 检测复杂碰撞

	FHitResult HitResult;
	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		CameraLoc,
		TraceEnd,
		ECC_Visibility,
		// 使用Visibility通道
		TraceParams
	);

	// 处理交互目标
	TWeakObjectPtr<UInteractionTarget> NewInteractionTarget = nullptr;

	if (bHit)
	{
		if (const AActor* HitActor = HitResult.GetActor())
		{
			// 获取所有交互组件
			TArray<UInteractionTarget*> InteractionComponents;
			HitActor->GetComponents(InteractionComponents);

			// 存在多个候选时进行智能选择
			if (InteractionComponents.Num() > 0)
			{
				NewInteractionTarget = FindBestInteractable(
					InteractionComponents,
					CameraLoc,
					CameraRot,
					HitResult.Location
				);
			}
		}
	}

	// 统一处理交互状态变化
	if (NewInteractionTarget != CurrentInteractionTarget)
	{
		// 退出旧交互
		if (CurrentInteractionTarget.IsValid())
		{
			CurrentInteractionTarget->Execute_OnEndHover(CurrentInteractionTarget.Get(), GetOwner());
		}

		// 更新当前交互目标
		CurrentInteractionTarget = NewInteractionTarget;

		// 进入新交互
		if (CurrentInteractionTarget.IsValid())
		{
			CurrentInteractionTarget->Execute_OnBeginHover(CurrentInteractionTarget.Get(), GetOwner());
			UpdateHintText(CurrentInteractionTarget.Get());
		}
	}
}

void UInteractionManager::UpdateInteractionWidget()
{
	// 更新交互 UI 可见性
	if (CurrentInteractionTarget.IsValid())
	{
		InteractionWidget->ShowWidget();
		// 更新交互 UI 长按进度
		UpdateHoldProgress();
	}
	else
	{
		InteractionWidget->HideWidget();
	}
}

void UInteractionManager::UpdateHoldProgress()
{
	// 更新Hold进度
	if (GetWorld()->GetTimerManager().IsTimerActive(HoldTimerHandle))
	{
		const float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(HoldTimerHandle);
		HoldProgress = ElapsedTime / HoldTotalDuration;
		InteractionWidget->ShowProgressBar();
		InteractionWidget->SetHoldProgress(HoldProgress);
	}
	else
	{
		HoldProgress = 0.0f; // 重置长按进度
		HoldTotalDuration = 0.0f; // 重置总持续时间
		InteractionWidget->HideProgressBar();
	}
}

void UInteractionManager::UpdateHintText(const UInteractionTarget* InteractionTarget) const
{
	// 根据交互类型获取动作文本
	const FText KeyActionText = GetInteractionTypeText(InteractionTarget->InteractionConfig.InteractionType);

	// 假设 KeyPrompt 是 FText 类型（如 "E" 键的本地化名称）
	const FText KeyPromptText = KeyPrompt;

	// 组合格式化的最终提示文本
	const FText FormattedHint = FText::Format(
		LOCTEXT("InteractionHintFormat", "{0}: [{1} {2}]"),
		InteractionTarget->InteractionTitle,
		KeyActionText,
		KeyPromptText
	);

	// 设置到UI控件
	InteractionWidget->SetHintText(FormattedHint);
}

TWeakObjectPtr<UInteractionTarget> UInteractionManager::FindBestInteractable(
	const TArray<UInteractionTarget*>& Candidates,
	const FVector& CameraLocation,
	const FRotator& CameraRotation,
	const FVector& HitLocation
) const
{
	const APlayerController* PC = PlayerController.Get();
	TWeakObjectPtr<UInteractionTarget> BestCandidate = nullptr;
	float BestScore = -FLT_MAX;

	for (const auto& Candidate : Candidates)
	{
		// 如果交互目标启用了阻断交互则跳过
		if (Candidate->bBlockInteraction)
		{
			continue;
		}

		// 综合评分系统
		auto Score = 0.0f;

		// 1. 组件优先级（如果有）
		if (Candidate->bUsePriority)
		{
			Score += Candidate->InteractionPriority * 1000.0f;
		}

		// 2. 距离到射线命中点
		const float DistanceToHit = FVector::DistSquared(
			Candidate->GetComponentLocation(),
			HitLocation
		);
		Score -= DistanceToHit * 0.1f;

		// 3. 屏幕空间位置（需要ProjectWorldLocationToScreen）
		if (FVector2D ScreenPosition; PC->ProjectWorldLocationToScreen(
			Candidate->GetComponentLocation(),
			ScreenPosition,
			true
		))
		{
			FVector2D ViewportSize;
			GEngine->GameViewport->GetViewportSize(ViewportSize);
			const FVector2D ScreenCenter = ViewportSize * 0.5f;
			const float ScreenDistance = FVector2D::Distance(ScreenPosition, ScreenCenter);
			Score -= ScreenDistance * 10.0f;
		}

		// 4. 朝向检测（可选）
		const FVector ToComponent = (Candidate->GetComponentLocation() - CameraLocation).GetSafeNormal();
		const float DotProduct = FVector::DotProduct(ToComponent, CameraRotation.Vector());
		Score += DotProduct * 50.0f;

		// 更新最佳候选
		if (Score > BestScore)
		{
			BestScore = Score;
			BestCandidate = Candidate;
		}
	}

	return BestCandidate;
}

void UInteractionManager::CreateInteractionWidget()
{
	// 创建交互 UI 实例
	InteractionWidget = CreateWidget<UInteractionWidget>(PlayerController.Get(), InteractionWidgetClass);

	// 添加到视口顶层（ZOrder值越大层级越高）
	if (InteractionWidget)
	{
		InteractionWidget->AddToViewport(100); // 设置足够高的ZOrder值
		InteractionWidget->HideWidget(); // 初始隐藏
	}
}

void UInteractionManager::BindInput()
{
	if (!InteractiveInputMappingContext || !InteractiveInputAction)
	{
		return;
	}

	// 添加输入映射上下文
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		PlayerController->GetLocalPlayer()
	))
	{
		Subsystem->AddMappingContext(InteractiveInputMappingContext, InputPriority);
	}

	// 绑定输入动作
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
	{
		EnhancedInput->BindAction(InteractiveInputAction, ETriggerEvent::Triggered, this, &UInteractionManager::HandleTriggered);
		EnhancedInput->BindAction(InteractiveInputAction, ETriggerEvent::Completed, this, &UInteractionManager::HandleCompleted);
		// EnhancedInput->BindAction(InteractiveInputAction, ETriggerEvent::Ongoing, this, &UInteractionManager::HandleOngoing);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UInteractionManager::HandleTriggered(const FInputActionValue& Value)
{
	if (!CurrentInteractionTarget.IsValid())
	{
		return;
	}

	if (UInteractionTarget* ActiveInteractionTarget = CurrentInteractionTarget.Get())
	{
		const EInteractionType InteractionType = ActiveInteractionTarget->InteractionConfig.InteractionType;
		if (InteractionType == EInteractionType::Press)
		{
			ActiveInteractionTarget->Execute_OnInteract(ActiveInteractionTarget, GetOwner(), Value);
		}
		if (InteractionType == EInteractionType::Hold)
		{
			HoldTotalDuration = ActiveInteractionTarget->InteractionConfig.HoldDuration;
			GetWorld()->GetTimerManager().SetTimer(
				HoldTimerHandle,
				this,
				&UInteractionManager::HandleHold,
				ActiveInteractionTarget->InteractionConfig.HoldDuration,
				// 动态时间阈值
				false // 不循环
			);
		}
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UInteractionManager::HandleCompleted(const FInputActionValue& Value)
{
	if (!CurrentInteractionTarget.IsValid())
	{
		return;
	}

	if (UInteractionTarget* ActiveInteractionTarget = CurrentInteractionTarget.Get())
	{
		const EInteractionType InteractionType = ActiveInteractionTarget->InteractionConfig.InteractionType;
		if (InteractionType == EInteractionType::Release)
		{
			ActiveInteractionTarget->Execute_OnInteract(ActiveInteractionTarget, GetOwner(), Value);
		}

		GetWorld()->GetTimerManager().ClearTimer(HoldTimerHandle);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UInteractionManager::HandleHold()
{
	if (!CurrentInteractionTarget.IsValid())
	{
		return;
	}
	if (UInteractionTarget* ActiveInteractionTarget = CurrentInteractionTarget.Get())
	{
		ActiveInteractionTarget->Execute_OnInteract(ActiveInteractionTarget, GetOwner(), {});
	}
}

FText UInteractionManager::GetInteractionTypeText(EInteractionType Type)
{
	switch (Type)
	{
	case EInteractionType::Press: return PressActionText;
	case EInteractionType::Release: return ReleaseActionText;
	case EInteractionType::Hold: return HoldActionText;
	case EInteractionType::DoubleTap: return HoldActionText;
	case EInteractionType::MultiTap: return MultiTapActionText;
	default: return FText::GetEmpty();
	}
}

#undef LOCTEXT_NAMESPACE
