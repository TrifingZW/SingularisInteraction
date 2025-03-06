// Fill out your copyright notice in the Description page of Project Settings.

#include "HighlightComponent.h"

UHighlightComponent::UHighlightComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	HighlightCount = 0;
}

void UHighlightComponent::EnableHighlight()
{
	HighlightCount++;
	if (HighlightCount == 1) UpdateHighlight(true);
}

void UHighlightComponent::DisableHighlight()
{
	HighlightCount = FMath::Max(0, HighlightCount - 1);
	if (HighlightCount == 0) UpdateHighlight(false);
}

void UHighlightComponent::BeginPlay()
{
	Super::BeginPlay();

	for (TArray<UPrimitiveComponent*> Components = GetRelevantComponents(); UPrimitiveComponent* Comp : Components)
		OriginalStates.Add(Comp, Comp->bRenderCustomDepth);
}

void UHighlightComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHighlightComponent::UpdateHighlight(const bool bEnable)
{
	for (TArray<UPrimitiveComponent*> Components = GetRelevantComponents(); UPrimitiveComponent* Comp : Components)
	{
		if (bEnable)
		{
			Comp->SetRenderCustomDepth(true);
			Comp->SetCustomDepthStencilValue(1);
		}
		else
		{
			const bool* OriginalState = OriginalStates.Find(Comp);
			Comp->SetRenderCustomDepth(OriginalState ? *OriginalState : false);
		}
	}
}

TArray<UPrimitiveComponent*> UHighlightComponent::GetRelevantComponents() const
{
	TArray<UPrimitiveComponent*> Results;
	if (const AActor* Owner = GetOwner())
		Owner->GetComponents<UPrimitiveComponent>(Results);
	return Results;
}
