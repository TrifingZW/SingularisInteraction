// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractionTarget.h"

#include "HighlightComponent.h"
#include "InteractionManager.h"
#include "InteractionWidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

// 设置该组件属性的默认值
UInteractionTarget::UInteractionTarget()
{
	/*PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;*/

	UInteractionWidgetComponent* InteractionWidgetComponent = CreateDefaultSubobject<UInteractionWidgetComponent>(TEXT("WidgetComponent"));
	InteractionWidgetComponent->SetupAttachment(this);
	InteractionWidgetComponent->SetVisibility(false);
	InteractionWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	WidgetComponent = InteractionWidgetComponent;

	HighlightComponent = CreateDefaultSubobject<UHighlightComponent>(TEXT("HighlightComponent"));
}

void UInteractionTarget::OnRegister()
{
	Super::OnRegister();

	if (PromptRange)
	{
		PromptRange->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
		PromptRange->UpdateComponentToWorld();
	}
}


// 当游戏开始时调用v
void UInteractionTarget::BeginPlay()
{
	Super::BeginPlay();

	// 设置 WidgetComponent 的 WidgetClass
	if (PromptWidgetClass)
		WidgetComponent->SetWidgetClass(PromptWidgetClass);

	// 设置 PromptRange
	if (PromptRange)
	{
		PromptRange->SetCollisionProfileName(TEXT("Trigger"));
		PromptRange->OnComponentBeginOverlap.AddDynamic(this, &UInteractionTarget::OnPromptRangeBeginOverlap);
		PromptRange->OnComponentEndOverlap.AddDynamic(this, &UInteractionTarget::OnPromptRangeEndOverlap);
	}

	// 获取 InteractionManager
	const APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (const AController* Controller = PlayerPawn->GetController(); !PlayerPawn || !Controller)
		InteractionManager = Controller->FindComponentByClass<UInteractionManager>();

	// 调试绘制范围
	if (DebugDraw)
		DrawDebugRange(PromptRange);
}

// 每一帧调用
void UInteractionTarget::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInteractionTarget::OnPlayersEnterPromptArea_Implementation(UPrimitiveComponent* OverlappedComponent,
                                                                 APawn* Pawn,
                                                                 UPrimitiveComponent* OtherComp,
                                                                 int32 OtherBodyIndex,
                                                                 bool bFromSweep,
                                                                 const FHitResult& SweepResult)
{
	WidgetComponent->SetVisibility(true);
}

void UInteractionTarget::OnPlayerLeavingPromptArea_Implementation(UPrimitiveComponent* OverlappedComponent,
                                                                  APawn* Pawn,
                                                                  UPrimitiveComponent* OtherComp,
                                                                  int32 OtherBodyIndex)
{
	WidgetComponent->SetVisibility(false);
}

void UInteractionTarget::OnPromptRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                                   AActor* OtherActor,
                                                   UPrimitiveComponent* OtherComp,
                                                   int32 OtherBodyIndex,
                                                   bool bFromSweep,
                                                   const FHitResult& SweepResult)
{
	if (APawn* Pawn = Cast<APawn>(OtherActor))
		if (const AController* Controller = Pawn->GetController(); Controller && Controller->IsLocalPlayerController())
			if (Controller->FindComponentByClass(UInteractionManager::StaticClass()) != nullptr)
				OnPlayersEnterPromptArea(OverlappedComponent, Pawn, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void UInteractionTarget::OnPromptRangeEndOverlap(UPrimitiveComponent* OverlappedComponent,
                                                 AActor* OtherActor,
                                                 UPrimitiveComponent* OtherComp,
                                                 int32 OtherBodyIndex)
{
	if (APawn* Pawn = Cast<APawn>(OtherActor))
		if (const AController* Controller = Pawn->GetController(); Controller && Controller->IsLocalPlayerController())
			if (Controller->FindComponentByClass(UInteractionManager::StaticClass()) != nullptr)
				OnPlayerLeavingPromptArea(OverlappedComponent, Pawn, OtherComp, OtherBodyIndex);
}

void UInteractionTarget::DrawDebugRange(UShapeComponent* DebugShapeComponent) const
{
	if (!DebugShapeComponent || !GetWorld()) return;

	const FVector Center = DebugShapeComponent->GetComponentLocation();
	const FQuat Rotation = DebugShapeComponent->GetComponentQuat();

	// 使用 Cast<> 检查并处理不同类型的组件
	if (const UBoxComponent* BoxComponent = Cast<UBoxComponent>(DebugShapeComponent))
	{
		const FVector Extent = BoxComponent->GetScaledBoxExtent();
		// 绘制一个绿色的调试框
		DrawDebugBox(GetWorld(), Center, Extent, Rotation, DebugDrawColor, true, -1.0f);
	}
	else if (const UCapsuleComponent* CapsuleComponent = Cast<UCapsuleComponent>(DebugShapeComponent))
	{
		const float HalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
		const float Radius = CapsuleComponent->GetScaledCapsuleRadius();
		// 绘制一个绿色的调试胶囊
		DrawDebugCapsule(GetWorld(), Center, HalfHeight, Radius, Rotation, DebugDrawColor, true, -1.0f);
	}
	else if (const USphereComponent* SphereComponent = Cast<USphereComponent>(DebugShapeComponent))
	{
		const float Radius = SphereComponent->GetScaledSphereRadius();
		// 绘制一个绿色的调试球体
		DrawDebugSphere(GetWorld(), Center, Radius, 12, DebugDrawColor, true, -1.0f);
	}
	else
	{
		// 如果不是已知的形状类型
		UE_LOG(LogTemp, Warning, TEXT("Unknown shape type!"));
	}
}

void UInteractionTarget::AddWidgetToScreen()
{
	if (PromptWidgetClass)
	{
		// 创建 Widget 实例
		Widget = CreateWidget<UUserWidget>(GetWorld(), PromptWidgetClass);

		// 添加到视口
		if (Widget)
			Widget->AddToViewport();
	}
}

void UInteractionTarget::RemoveWidgetFromScreen()
{
	if (Widget)
	{
		Widget->RemoveFromParent();
		Widget = nullptr;
	}
}

void UInteractionTarget::ExecuteInteraction_Implementation()
{
}

void UInteractionTarget::OnBeginHover_Implementation(AActor* Interactor)
{
	// 显示高亮/UI提示
	UE_LOG(LogTemp, Warning, TEXT("开始注视 %s"), *GetName());
	if (Highlight)
		HighlightComponent->EnableHighlight();
}

void UInteractionTarget::OnEndHover_Implementation(AActor* Interactor)
{
	// 关闭高亮/提示
	UE_LOG(LogTemp, Warning, TEXT("结束注视 %s"), *GetName());
	if (Highlight)
		HighlightComponent->DisableHighlight();
}

void UInteractionTarget::OnInteract_Implementation(AActor* Interactor)
{
	// 处理交互逻辑
	UE_LOG(LogTemp, Warning, TEXT("与 %s 交互"), *GetName());
}
