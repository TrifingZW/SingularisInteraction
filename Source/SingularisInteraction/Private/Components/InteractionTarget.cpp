/* =====================================================================
 * InteractionTarget.cpp
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2025 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024-2025 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#include "Components/InteractionTarget.h"

#include <DrawDebugHelpers.h>
#include <Components/BoxComponent.h>
#include <Components/CapsuleComponent.h>
#include <Components/InteractionManager.h>
#include <Components/SphereComponent.h>
#include <Components/WidgetComponent.h>
#include <GameFramework/Character.h>
#include <GameFramework/Pawn.h>

#include "Components/HighlightComponent.h"
#include "Widgets/InteractionTargetWidget.h"

#define ECC_INTERACTABLE ECC_GameTraceChannel1

UInteractionTarget::UInteractionTarget()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UInteractionTarget::BeginPlay()
{
	Super::BeginPlay();

	for (auto InteractionPrimitive : InteractionPrimitives)
		if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(InteractionPrimitive.GetComponent(GetOwner())))
		{
			PrimitiveComponent->SetCollisionResponseToChannel(ECC_INTERACTABLE, ECR_Block);
			PrimitiveComponent->ComponentTags.Add(FName("Interaction"));
		}

	if (bIsPrompt)
	{
		PromptArea = Cast<UShapeComponent>(PromptAreaReference.GetComponent(GetOwner()));
		if (PromptArea.IsValid())
		{
			PromptArea->SetCollisionProfileName(TEXT("Trigger"));
			PromptArea->SetCollisionResponseToChannel(ECC_INTERACTABLE, ECR_Ignore);
			PromptArea->OnComponentBeginOverlap.AddDynamic(this, &UInteractionTarget::OnPromptAreaBeginOverlap);
			PromptArea->OnComponentEndOverlap.AddDynamic(this, &UInteractionTarget::OnPromptRangeAreaOverlap);

			if (InteractionTargetWidgetClass)
			{
				WidgetComponent = Cast<UWidgetComponent>(
					GetOwner()->AddComponentByClass(
						UWidgetComponent::StaticClass(),
						true,
						FTransform::Identity,
						true
					)
				);

				if (WidgetComponent)
				{
					WidgetComponent->SetupAttachment(PromptArea.Get());
					WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
					WidgetComponent->SetWidgetClass(InteractionTargetWidgetClass.Get());
					WidgetComponent->RegisterComponent();
				}
			}
		}
	}

	if (bIsHighlight)
	{
		HighlightComponent = Cast<UHighlightComponent>(
			GetOwner()->AddComponentByClass(
				UHighlightComponent::StaticClass(),
				false,
				FTransform::Identity,
				false
			)
		);

		for (auto HighlightPrimitive : HighlightPrimitives)
			if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(HighlightPrimitive.GetComponent(GetOwner())))
				HighlightComponent->AddPrimitive(PrimitiveComponent);
	}

	// 准备就绪！
	OnInteractionTargetReadyEvent.Broadcast();
}

void UInteractionTarget::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bBlockInteraction) return;

	if (bInteractionDebugDraw)
		if (PromptArea.IsValid())
			DrawDebugRange(PromptArea.Get(), PromptArea->ShapeColor, 0.0f);
}

void UInteractionTarget::OnBeginHover_Implementation(AActor* Interactor, const FVector_NetQuantize ImpactPoint)
{
	if (bBlockInteraction) return;

	if (bInteractionDebugOutput)
		UE_LOG(LogTemp, Warning, TEXT("开始注视 %s 的 %s 交互目标"), *GetOwner()->GetName(), *InteractionTitle.ToString());

	if (APlayerController* PlayerController = Cast<APlayerController>(Interactor))
		if (ACharacter* Character = PlayerController->GetPawn<ACharacter>())
			OnBeginFocusedEvent.Broadcast(PlayerController, Character, ImpactPoint);

	if (bIsHighlight)
		if (HighlightComponent)
			HighlightComponent->Enable();
}

void UInteractionTarget::OnEndHover_Implementation(AActor* Interactor, const FVector_NetQuantize ImpactPoint)
{
	if (bBlockInteraction) return;

	if (bInteractionDebugOutput)
		UE_LOG(LogTemp, Warning, TEXT("结束注视 %s 的 %s 交互目标"), *GetOwner()->GetName(), *InteractionTitle.ToString());

	if (APlayerController* PlayerController = Cast<APlayerController>(Interactor))
		if (ACharacter* Character = PlayerController->GetPawn<ACharacter>())
			OnEndFocusedEvent.Broadcast(PlayerController, Character, ImpactPoint);

	if (bIsHighlight)
		if (HighlightComponent)
			HighlightComponent->Disable();
}

void UInteractionTarget::OnInteraction_Implementation(AActor* Interactor, const FVector_NetQuantize ImpactPoint, const FInputActionValue& Value)
{
	if (bBlockInteraction) return;

	if (bInteractionDebugOutput)
		UE_LOG(LogTemp, Warning, TEXT("与 %s 的 %s 交互目标交互"), *GetOwner()->GetName(), *InteractionTitle.ToString());

	if (APlayerController* PlayerController = Cast<APlayerController>(Interactor))
		if (ACharacter* Character = PlayerController->GetPawn<ACharacter>())
			OnInteractionEvent.Broadcast(PlayerController, Character, ImpactPoint, Value);
}

bool UInteractionTarget::IsBlockInteraction() const
{
	return bBlockInteraction;
}

void UInteractionTarget::BlockInteraction()
{
	if (bBlockInteraction) return;
	bBlockInteraction = true;

	if (UUserWidget* Widget = WidgetComponent->GetWidget())
	{
		Cast<UInteractionTargetWidget>(Widget)->HideWidget();
		HighlightComponent->Disable();
	}
}

void UInteractionTarget::UnlockInteraction()
{
	if (!bBlockInteraction) return;
	bBlockInteraction = false;

	if (PromptArea.IsValid())
	{
		TArray<AActor*> OverlappingActors;
		PromptArea->GetOverlappingActors(OverlappingActors, APawn::StaticClass());
		if (OverlappingActors.Num() > 0)
			if (UUserWidget* Widget = WidgetComponent->GetWidget())
				Cast<UInteractionTargetWidget>(Widget)->ShowWidget();
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UInteractionTarget::OnPromptAreaBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	const int32 OtherBodyIndex,
	const bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (bBlockInteraction) return;

	if (APawn* Pawn = Cast<APawn>(OtherActor))
	{
		if (const AController* Controller = Pawn->GetController();
			Controller && Controller->IsLocalPlayerController() && Controller->FindComponentByClass(UInteractionManager::StaticClass()))
		{
			if (UUserWidget* Widget = WidgetComponent->GetWidget())
				Cast<UInteractionTargetWidget>(Widget)->ShowWidget();

			OnPlayersEnterPromptAreaEvent.Broadcast(
				OverlappedComponent,
				Pawn,
				OtherComp,
				OtherBodyIndex,
				bFromSweep,
				SweepResult
			);
		}
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UInteractionTarget::OnPromptRangeAreaOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	const int32 OtherBodyIndex
)
{
	if (bBlockInteraction) return;

	if (APawn* Pawn = Cast<APawn>(OtherActor))
	{
		if (const AController* Controller = Pawn->GetController();
			Controller && Controller->IsLocalPlayerController() && Controller->FindComponentByClass(UInteractionManager::StaticClass()))
		{
			if (UUserWidget* Widget = WidgetComponent->GetWidget())
				Cast<UInteractionTargetWidget>(Widget)->HideWidget();

			OnPlayerLeavingPromptAreaEvent.Broadcast(
				OverlappedComponent,
				Pawn,
				OtherComp,
				OtherBodyIndex
			);
		}
	}
}

void UInteractionTarget::DrawDebugRange(UShapeComponent* DebugShapeComponent, const FColor Color, const float Duration) const
{
	if (!DebugShapeComponent || !GetWorld()) return;

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
