/* =====================================================================
 * InteractionManager.cpp
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#include "InteractionManager.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InteractableInterface.h"
#include "InteractionTarget.h"

// 设置该组件属性的默认值
UInteractionManager::UInteractionManager()
{
	// 设置该组件在游戏开始时初始化，并在每一帧进行更新。如果不需要这些功能，
	// 可以关闭它们以提高性能。
	PrimaryComponentTick.bCanEverTick = true;

	// ...
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

	BindInput();
	// ...
}


// 每一帧调用
void UInteractionManager::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	UpdateInteractionTarget();
}

void UInteractionManager::UpdateInteractionTarget()
{
	const APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC) return;

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
				NewInteractionTarget = FindBestInteractable(
					InteractionComponents,
					CameraLoc,
					CameraRot,
					HitResult.Location
				);
		}
	}

	// 统一处理交互状态变化
	if (NewInteractionTarget != CurrentInteractionTarget)
	{
		// 退出旧交互
		if (CurrentInteractionTarget.IsValid())
			CurrentInteractionTarget->Execute_OnEndHover(CurrentInteractionTarget.Get(), GetOwner());

		// 更新当前交互目标
		CurrentInteractionTarget = NewInteractionTarget;

		// 进入新交互
		if (CurrentInteractionTarget.IsValid())
			CurrentInteractionTarget->Execute_OnBeginHover(CurrentInteractionTarget.Get(), GetOwner());
	}
}

TWeakObjectPtr<UInteractionTarget> UInteractionManager::FindBestInteractable(
	const TArray<UInteractionTarget*>& Candidates,
	const FVector& CameraLocation,
	const FRotator& CameraRotation,
	const FVector& HitLocation
) const
{
	const APlayerController* PC = Cast<APlayerController>(GetOwner());
	TWeakObjectPtr<UInteractionTarget> BestCandidate = nullptr;
	float BestScore = -FLT_MAX;

	for (const auto& Candidate : Candidates)
	{
		// 综合评分系统
		float Score = 0.0f;

		// 1. 组件优先级（如果有）
		if (Candidate->UsePriority)
			Score += Candidate->InteractionPriority * 1000.0f;

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
			true))
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

void UInteractionManager::BindInput()
{
	if (!InteractiveInputMappingContext || !InteractiveInputAction)
	{
		return;
	}

	// 添加输入映射上下文
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		PlayerController->GetLocalPlayer()))
		Subsystem->AddMappingContext(InteractiveInputMappingContext, InputPriority);

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
		return;

	if (UInteractionTarget* ActiveInteractionTarget = CurrentInteractionTarget.Get();
		ActiveInteractionTarget->InteractionConfig.InteractionType == EInteractionType::Press)
		ActiveInteractionTarget->Execute_OnInteract(ActiveInteractionTarget, GetOwner(), Value);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UInteractionManager::HandleCompleted(const FInputActionValue& Value)
{
	if (!CurrentInteractionTarget.IsValid())
		return;

	if (UInteractionTarget* ActiveInteractionTarget = CurrentInteractionTarget.Get();
		ActiveInteractionTarget->InteractionConfig.InteractionType == EInteractionType::Release)
		ActiveInteractionTarget->Execute_OnInteract(ActiveInteractionTarget, GetOwner(), Value);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UInteractionManager::HandleOngoing(const FInputActionValue& Value)
{
	if (!CurrentInteractionTarget.IsValid())
		return;
	
	if (UInteractionTarget* ActiveInteractionTarget = CurrentInteractionTarget.Get();
		ActiveInteractionTarget->InteractionConfig.InteractionType == EInteractionType::Hold)
		ActiveInteractionTarget->Execute_OnInteract(ActiveInteractionTarget, GetOwner(), Value);
}
