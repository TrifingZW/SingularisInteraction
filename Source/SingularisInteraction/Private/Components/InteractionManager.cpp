/* =====================================================================
 * InteractionManager.cpp
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2025 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024-2025 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#include "Components/InteractionManager.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TimerManager.h"
#include "Components/InteractionTarget.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/InteractionInterface.h"
#include "Widgets/InteractionManagerWidget.h"

#define LOCTEXT_NAMESPACE "InteractionManager"
#define ECC_INTERACTABLE ECC_GameTraceChannel1

UInteractionManager::UInteractionManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UInteractionManager::OnRegister()
{
	Super::OnRegister();

	PlayerController = Cast<APlayerController>(GetOwner());
	if (PlayerController == nullptr)
	{
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

void UInteractionManager::BeginPlay()
{
	Super::BeginPlay();

	// 绑定输入
	BindInput();

	// 设置交互 UI 实例
	CreateInteractionWidget();

	// 准备就绪！
	OnInteractionManagerReadyEvent.Broadcast();
}

void UInteractionManager::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bCanInteraction)
	{
		// 更新交互目标
		UpdateInteractionTarget();

		// 更新交互 UI
		UpdateInteractionWidget();
	}
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

	const FVector TraceEnd = CameraLoc + CameraRot.Vector() * InteractionDistance;

	// 射线检测参数
	FHitResult HitResult;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(Cast<AActor>(PC->GetPawn())); // 忽略玩家自身
	TraceParams.bTraceComplex = true; // 检测复杂碰撞

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		CameraLoc,
		TraceEnd,
		ECC_INTERACTABLE,
		TraceParams
	);

	// 处理交互目标
	TWeakObjectPtr<UInteractionTarget> NewInteractionTarget = nullptr;

	// 旧版逻辑
	/*if (bHit)
		if (const AActor* HitActor = HitResult.GetActor())
		{
			// 获取所有交互组件
			TArray<UInteractionTarget*> InteractionComponents;
			HitActor->GetComponents(InteractionComponents);

			// 存在多个候选时进行智能选择
			if (InteractionComponents.Num() > 0)
				NewInteractionTarget = FindBestInteractable(
					InteractionComponents,
					CameraLoc,
					CameraRot,
					HitResult.Location
				);
		}*/

	if (bHit)
	{
		CurrentHitImpactPoint = HitResult.ImpactPoint;
		if (const UPrimitiveComponent* PrimitiveComponent = HitResult.GetComponent())
		{
			if (PrimitiveComponent->ComponentHasTag("Interaction"))
			{
				if (UInteractionTarget* InteractionTarget = Cast<UInteractionTarget>(
					PrimitiveComponent->GetOwner()->FindComponentByClass(UInteractionTarget::StaticClass())
				))
				{
					if (!InteractionTarget->IsBlockInteraction())
					{
						NewInteractionTarget = InteractionTarget;
					}
				}
			}
		}
	}

	// 统一处理交互状态变化
	if (NewInteractionTarget != CurrentInteractionTarget)
	{
		// 退出旧交互
		if (CurrentInteractionTarget.IsValid())
		{
			CurrentInteractionTarget->Execute_OnEndHover(
				CurrentInteractionTarget.Get(),
				GetOwner(),
				CurrentHitImpactPoint
			);
		}

		// 更新当前交互目标
		CurrentInteractionTarget = NewInteractionTarget;

		// 进入新交互
		if (CurrentInteractionTarget.IsValid())
		{
			if (InteractionManagerWidget)
			{
				InteractionManagerWidget->UpdateDetection(CurrentInteractionTarget.Get());
			}
			CurrentInteractionTarget->Execute_OnBeginHover(
				CurrentInteractionTarget.Get(),
				GetOwner(),
				CurrentHitImpactPoint
			);
			UpdateHintText(CurrentInteractionTarget.Get());
		}
		else
		{
			if (InteractionManagerWidget)
			{
				InteractionManagerWidget->NoDetection();
			}
		}
	}
}

void UInteractionManager::UpdateInteractionWidget()
{
	if (CurrentInteractionTarget.IsValid())
	{
		// 更新交互 UI 长按进度
		UpdateHoldProgress();
	}
	else if (IsProgress)
	{
		IsProgress = false;

		if (InteractionManagerWidget)
		{
			InteractionManagerWidget->HideProgressBar();
		}
	}
}

void UInteractionManager::UpdateHoldProgress()
{
	// 更新Hold进度
	if (GetWorld()->GetTimerManager().IsTimerActive(HoldTimerHandle))
	{
		const float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(HoldTimerHandle);
		HoldProgress = ElapsedTime / HoldTotalDuration;

		if (InteractionManagerWidget)
		{
			InteractionManagerWidget->ShowProgressBar();
			InteractionManagerWidget->SetHoldProgress(HoldProgress);
		}
	}
	else
	{
		HoldProgress = 0.0f; // 重置长按进度
		HoldTotalDuration = 0.0f; // 重置总持续时间

		if (InteractionManagerWidget)
		{
			InteractionManagerWidget->HideProgressBar();
		}
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
	if (InteractionManagerWidget)
	{
		InteractionManagerWidget->SetHintText(FormattedHint);
	}
}

/*TWeakObjectPtr<UInteractionTarget> UInteractionManager::FindBestInteractable(
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
		if (Candidate->bUseInteractionPriority)
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
}*/

void UInteractionManager::CreateInteractionWidget()
{
	if (PlayerController.IsValid() && InteractionWidgetClass)
		InteractionManagerWidget = CreateWidget<UInteractionManagerWidget>(PlayerController.Get(), InteractionWidgetClass);

	if (InteractionManagerWidget)
		InteractionManagerWidget->AddToViewport(100);
}

void UInteractionManager::BindInput()
{
	if (!PlayerController.IsValid() || !InteractiveInputMappingContext || !InteractiveInputAction) return;

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		Subsystem->AddMappingContext(InteractiveInputMappingContext, InputPriority);

	// 绑定输入动作
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
	{
		EnhancedInput->BindAction(InteractiveInputAction, ETriggerEvent::Started, this, &UInteractionManager::HandleTriggered);
		EnhancedInput->BindAction(InteractiveInputAction, ETriggerEvent::Completed, this, &UInteractionManager::HandleCompleted);
	}
}

// ReSharper disable once CppMemberFunctionMayB eConst
void UInteractionManager::HandleTriggered(const FInputActionValue& Value)
{
	if (!CurrentInteractionTarget.IsValid()) return;

	if (UInteractionTarget* ActiveInteractionTarget = CurrentInteractionTarget.Get())
	{
		const EInteractionType InteractionType = ActiveInteractionTarget->InteractionConfig.InteractionType;

		if (InteractionType == EInteractionType::Press)
			ActiveInteractionTarget->Execute_OnInteraction(ActiveInteractionTarget, GetOwner(), CurrentHitImpactPoint, Value);

		if (InteractionType == EInteractionType::Hold)
		{
			IsProgress = true;
			HoldTotalDuration = ActiveInteractionTarget->InteractionConfig.HoldDuration;
			GetWorld()->GetTimerManager().SetTimer(
				HoldTimerHandle,
				this,
				&UInteractionManager::HandleHold,
				ActiveInteractionTarget->InteractionConfig.HoldDuration,
				false
			);
		}
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UInteractionManager::HandleCompleted(const FInputActionValue& Value)
{
	if (!CurrentInteractionTarget.IsValid())
	{
		IsProgress = false;
		GetWorld()->GetTimerManager().ClearTimer(HoldTimerHandle);
		return;
	}

	if (UInteractionTarget* ActiveInteractionTarget = CurrentInteractionTarget.Get())
	{
		const EInteractionType InteractionType = ActiveInteractionTarget->InteractionConfig.InteractionType;
		if (InteractionType == EInteractionType::Release)
			ActiveInteractionTarget->Execute_OnInteraction(ActiveInteractionTarget, GetOwner(), CurrentHitImpactPoint, Value);
		IsProgress = false;
		GetWorld()->GetTimerManager().ClearTimer(HoldTimerHandle);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UInteractionManager::HandleHold()
{
	if (!CurrentInteractionTarget.IsValid()) return;

	if (UInteractionTarget* ActiveInteractionTarget = CurrentInteractionTarget.Get())
		ActiveInteractionTarget->Execute_OnInteraction(ActiveInteractionTarget, GetOwner(), CurrentHitImpactPoint, {});
}

void UInteractionManager::DisableInteraction()
{
	if (!bCanInteraction) return;

	bCanInteraction = false;
	HoldProgress = 0.0f;
	HoldTotalDuration = 0.0f;
	CurrentInteractionTarget = nullptr;
	GetWorld()->GetTimerManager().ClearTimer(HoldTimerHandle);

	if (InteractionManagerWidget)
		InteractionManagerWidget->HideProgressBar();
}

void UInteractionManager::EnableInteraction()
{
	if (bCanInteraction) return;

	bCanInteraction = true;

	if (InteractionManagerWidget)
		InteractionManagerWidget->ShowWidget();
}

void UInteractionManager::ShowInteractionManagerWidget() const
{
	if (InteractionManagerWidget)
		InteractionManagerWidget->ShowWidget();
}

void UInteractionManager::HideInteractionManagerWidget() const
{
	if (InteractionManagerWidget)
		InteractionManagerWidget->HideWidget();
}

void UInteractionManager::EnterSpecialState() const
{
	if (InteractionManagerWidget)
		InteractionManagerWidget->EnableSpecialState();
}

void UInteractionManager::ExitSpecialState() const
{
	if (InteractionManagerWidget)
		InteractionManagerWidget->DisableSpecialState();
}

/*void UInteractionManager::SetInteractionManagerWidget(UInteractionManagerWidget* NewInteractionManagerWidget)
{
	if (InteractionManagerWidget)
	{
		InteractionManagerWidget->RemoveFromParent();
		InteractionManagerWidget = nullptr;
	}

	InteractionManagerWidget = NewInteractionManagerWidget;

	if (InteractionManagerWidget)
	{
		InteractionManagerWidget->AddToViewport(100);
	}
}

void UInteractionManager::ResetInteractionManagerWidget()
{
	if (InteractionManagerWidget)
	{
		InteractionManagerWidget->RemoveFromParent();
		InteractionManagerWidget = nullptr;
	}

	CreateInteractionWidget();
}*/

FText UInteractionManager::GetInteractionTypeText(const EInteractionType Type)
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
