// Fill out your copyright notice in the Description page of Project Settings.

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
	TScriptInterface<IInteractableInterface> NewInteractable = nullptr;

	if (bHit)
	{
		if (const AActor* HitActor = HitResult.GetActor())
		{
			// 同时检查组件存在性和接口有效性
			if (UInteractionTarget* InteractionComponent = HitActor->FindComponentByClass<UInteractionTarget>())
			{
				// 自动验证接口有效性（TScriptInterface会处理）
				NewInteractable = TScriptInterface<IInteractableInterface>(InteractionComponent);

				// 显式验证接口有效性（如果引擎验证不够严格）
				if (!NewInteractable.GetInterface())
				{
					NewInteractable = nullptr;
				}
			}
		}
	}

	// 统一处理交互状态变化
	if (NewInteractable != CurrentInteractable)
	{
		// 退出旧交互
		if (CurrentInteractable)
		{
			CurrentInteractable->Execute_OnEndHover(CurrentInteractable.GetObject(), GetOwner());
		}

		// 更新当前交互目标
		CurrentInteractable = NewInteractable;

		// 进入新交互
		if (CurrentInteractable)
		{
			CurrentInteractable->Execute_OnBeginHover(CurrentInteractable.GetObject(), GetOwner());
		}
	}
}

void UInteractionManager::BindInput()
{
	if (!InteractiveInputMappingContext || !InteractiveInputAction) return;

	// 添加输入映射上下文
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		PlayerController->GetLocalPlayer()))
		Subsystem->AddMappingContext(InteractiveInputMappingContext, InputPriority);

	// 绑定输入动作
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		EnhancedInput->BindAction(InteractiveInputAction, ETriggerEvent::Triggered, this, &UInteractionManager::HandleInput);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UInteractionManager::HandleInput(const FInputActionValue& Value)
{
	if (CurrentInteractable.GetInterface())
		CurrentInteractable->Execute_OnInteract(CurrentInteractable.GetObject(), GetOwner());
}
