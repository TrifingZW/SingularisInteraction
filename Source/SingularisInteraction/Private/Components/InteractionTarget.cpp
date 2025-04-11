/* =====================================================================
 * InteractionTarget.cpp
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2025 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024-2025 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#include "Components/InteractionTarget.h"

#include <GameFramework/Character.h>

#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InteractionManager.h"
#include "Components/InteractionWidgetComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/InteractionTargetWidget.h"

#define ECC_INTERACTABLE ECC_GameTraceChannel1

// 设置该组件属性的默认值
UInteractionTarget::UInteractionTarget()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	UInteractionWidgetComponent* InteractionWidgetComponent = CreateDefaultSubobject<UInteractionWidgetComponent>(TEXT("WidgetComponent"));
	InteractionWidgetComponent->SetupAttachment(this);
	InteractionWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	WidgetComponent = InteractionWidgetComponent;
}

void UInteractionTarget::OnRegister()
{
	Super::OnRegister();

	if (InteractiveRange)
	{
		InteractiveRange->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
		InteractiveRange->UpdateComponentToWorld();
		// InteractiveRange->Rename(TEXT("InteractiveRange"));
		InteractiveRange->ComponentTags.Add(FName("Interactable"));
	}

	if (PromptRange)
	{
		PromptRange->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
		PromptRange->UpdateComponentToWorld();
		// PromptRange->Rename(TEXT("PromptRange"));
	}
}


// 当游戏开始时调用
void UInteractionTarget::BeginPlay()
{
	Super::BeginPlay();

	if (HighlightComponentClass)
	{
		HighlightComponent = NewObject<UHighlightComponent>(this, HighlightComponentClass);
	}

	// 设置 WidgetComponent 的 WidgetClass
	if
	(PromptWidgetClass)
	{
		WidgetComponent->SetWidgetClass(PromptWidgetClass.Get());
	}

	// 设置 InteractiveRange
	if
	(InteractiveRange)
	{
		InteractiveRange->SetCollisionResponseToChannel(ECC_INTERACTABLE, ECR_Block);
	}

	// 设置 PromptRange
	if
	(PromptRange)
	{
		PromptRange->SetCollisionProfileName(TEXT("Trigger"));
		PromptRange->SetCollisionResponseToChannel(ECC_INTERACTABLE, ECR_Ignore);
		PromptRange->OnComponentBeginOverlap.AddDynamic(this, &UInteractionTarget::OnPromptRangeBeginOverlap);
		PromptRange->OnComponentEndOverlap.AddDynamic(this, &UInteractionTarget::OnPromptRangeEndOverlap);
	}

	if (const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		InteractionManager = PlayerController->FindComponentByClass<UInteractionManager>();
	}
}

// 每一帧调用
void UInteractionTarget::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 如果阻断则返回
	if (bBlockInteraction)
	{
		return;
	}

	// 调试绘制范围
	if (bInteractionDebugDraw)
	{
		DrawDebugRange(InteractiveRange, InteractiveRange->ShapeColor, 0.0f);
		DrawDebugRange(PromptRange, PromptRange->ShapeColor, 0.0f);
	}
}

void UInteractionTarget::OnPlayersEnterPromptArea_Implementation(
	UPrimitiveComponent* OverlappedComponent,
	APawn* Pawn,
	UPrimitiveComponent* OtherComp,
	const int32 OtherBodyIndex,
	const bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (UUserWidget* Widget = WidgetComponent->GetWidget())
	{
		Cast<UInteractionTargetWidget>(Widget)->ShowWidget();
	}
	OnPlayersEnterPromptAreaEvent.Broadcast(
		OverlappedComponent,
		Pawn,
		OtherComp,
		OtherBodyIndex,
		bFromSweep,
		SweepResult
	);
}

void UInteractionTarget::OnPlayerLeavingPromptArea_Implementation(
	UPrimitiveComponent* OverlappedComponent,
	APawn* Pawn,
	UPrimitiveComponent* OtherComp,
	const int32 OtherBodyIndex
)
{
	if (UUserWidget* Widget = WidgetComponent->GetWidget())
	{
		Cast<UInteractionTargetWidget>(Widget)->HideWidget();
	}
	OnPlayerLeavingPromptAreaEvent.Broadcast(
		OverlappedComponent,
		Pawn,
		OtherComp,
		OtherBodyIndex
	);
}

void UInteractionTarget::OnPromptRangeBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	const int32 OtherBodyIndex,
	const bool bFromSweep,
	const FHitResult& SweepResult
)
{
	// 如果阻断则返回
	if (bBlockInteraction)
	{
		return;
	}

	if (APawn* Pawn = Cast<APawn>(OtherActor))
	{
		if (const AController* Controller = Pawn->GetController(); Controller && Controller->IsLocalPlayerController())
		{
			if (Controller->FindComponentByClass(UInteractionManager::StaticClass()) != nullptr)
			{
				OnPlayersEnterPromptArea(OverlappedComponent, Pawn, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
			}
		}
	}
}

void UInteractionTarget::OnPromptRangeEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	const int32 OtherBodyIndex
)
{
	// 如果阻断则返回
	if (bBlockInteraction)
	{
		return;
	}

	if (APawn* Pawn = Cast<APawn>(OtherActor))
	{
		if (const AController* Controller = Pawn->GetController(); Controller && Controller->IsLocalPlayerController())
		{
			if (Controller->FindComponentByClass(UInteractionManager::StaticClass()) != nullptr)
			{
				OnPlayerLeavingPromptArea(OverlappedComponent, Pawn, OtherComp, OtherBodyIndex);
			}
		}
	}
}

void UInteractionTarget::DrawDebugRange(UShapeComponent* DebugShapeComponent, const FColor Color, const float Duration) const
{
	if (!DebugShapeComponent || !GetWorld())
	{
		return;
	}

	const FVector Center = DebugShapeComponent->GetComponentLocation();
	const FQuat Rotation = DebugShapeComponent->GetComponentQuat();

	// 使用 Cast<> 检查并处理不同类型的组件
	if (const UBoxComponent* BoxComponent = Cast<UBoxComponent>(DebugShapeComponent))
	{
		const FVector Extent = BoxComponent->GetScaledBoxExtent();
		// 绘制一个绿色的调试框
		DrawDebugBox(GetWorld(), Center, Extent, Rotation, Color, false, Duration);
	}
	else if (const UCapsuleComponent* CapsuleComponent = Cast<UCapsuleComponent>(DebugShapeComponent))
	{
		const float HalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
		const float Radius = CapsuleComponent->GetScaledCapsuleRadius();
		// 绘制一个绿色的调试胶囊
		DrawDebugCapsule(GetWorld(), Center, HalfHeight, Radius, Rotation, Color, false, Duration);
	}
	else if (const USphereComponent* SphereComponent = Cast<USphereComponent>(DebugShapeComponent))
	{
		const float Radius = SphereComponent->GetScaledSphereRadius();
		// 绘制一个绿色的调试球体
		DrawDebugSphere(GetWorld(), Center, Radius, 16, Color, false, Duration);
	}
	else
	{
		// 如果不是已知的形状类型
		UE_LOG(LogTemp, Warning, TEXT("Unknown shape type!"));
	}
}

void UInteractionTarget::OnBeginHover_Implementation(AActor* Interactor, const FVector_NetQuantize ImpactPoint)
{
	if (bBlockInteraction)
	{
		return;
	}

	if (bInteractionDebugOutput)
		UE_LOG(LogTemp, Warning, TEXT("开始注视 %s 的 %s 交互目标"), *GetOwner()->GetName(), *InteractionTitle.ToString());

	if (APlayerController* PlayerController = Cast<APlayerController>(Interactor))
	{
		if (ACharacter* Character = PlayerController->GetPawn<ACharacter>())
		{
			OnBeginFocusedEvent.Broadcast(PlayerController, Character, ImpactPoint);
		}
	}

	if (bInteractionHighlight)
	{
		if (HighlightComponent)
		{
			HighlightComponent->EnableHighlight(GetOwner());
		}
	}
}

void UInteractionTarget::OnEndHover_Implementation(AActor* Interactor, const FVector_NetQuantize ImpactPoint)
{
	if (bBlockInteraction)
	{
		return;
	}

	if (bInteractionDebugOutput)
		UE_LOG(LogTemp, Warning, TEXT("结束注视 %s 的 %s 交互目标"), *GetOwner()->GetName(), *InteractionTitle.ToString());

	if (APlayerController* PlayerController = Cast<APlayerController>(Interactor))
	{
		if (ACharacter* Character = PlayerController->GetPawn<ACharacter>())
		{
			OnEndFocusedEvent.Broadcast(PlayerController, Character, ImpactPoint);
		}
	}

	if (bInteractionHighlight)
	{
		if (HighlightComponent)
		{
			HighlightComponent->DisableHighlight(GetOwner());
		}
	}
}

void UInteractionTarget::OnInteract_Implementation(AActor* Interactor, const FVector_NetQuantize ImpactPoint, const FInputActionValue& Value)
{
	if (bBlockInteraction)
	{
		return;
	}

	// 处理交互逻辑
	if (bInteractionDebugOutput)
		UE_LOG(LogTemp, Warning, TEXT("与 %s 的 %s 交互目标交互"), *GetOwner()->GetName(), *InteractionTitle.ToString());

	if (APlayerController* PlayerController = Cast<APlayerController>(Interactor))
	{
		if (ACharacter* Character = PlayerController->GetPawn<ACharacter>())
		{
			OnInteractionEvent.Broadcast(PlayerController, Character, ImpactPoint, Value);
		}
	}
}

void UInteractionTarget::BlockInteraction()
{
	if (UUserWidget* Widget = WidgetComponent->GetWidget())
	{
		Cast<UInteractionTargetWidget>(Widget)->HideWidget();
		HighlightComponent->DisableHighlight(GetOwner());
	}
	bBlockInteraction = true;
}

void UInteractionTarget::UnlockInteraction()
{
	if (const UInteractionManager* IM = InteractionManager.Get())
	{
		if (const auto* PlayerController = IM->PlayerController.Get())
		{
			if (PromptRange->IsOverlappingActor(PlayerController->GetPawn()))
			{
				if (UUserWidget* Widget = WidgetComponent->GetWidget())
				{
					Cast<UInteractionTargetWidget>(Widget)->ShowWidget();
				}
			}
		}
	}

	/*if (IM->CurrentInteractionTarget == this)
	{
		if (UUserWidget* Widget = WidgetComponent->GetWidget())
		{
			Cast<UInteractionTargetWidget>(Widget)->ShowWidget();
		}
	}*/

	bBlockInteraction = false;
}
