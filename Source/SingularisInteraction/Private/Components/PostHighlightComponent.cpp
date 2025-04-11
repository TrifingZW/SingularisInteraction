/* =====================================================================
 * PostHighlightComponent.cpp
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2025 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024-2025 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#include "Components/PostHighlightComponent.h"

UPostHighlightComponent::UPostHighlightComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	HighlightCount = 0;
}

void UPostHighlightComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPostHighlightComponent::EnableHighlight_Implementation(AActor* Actor)
{
	HighlightCount++;
	if (HighlightCount == 1)
	{
		UpdateHighlight(true);
	}
}

void UPostHighlightComponent::DisableHighlight_Implementation(AActor* Actor)
{
	HighlightCount = FMath::Max(0, HighlightCount - 1);
	if (HighlightCount == 0)
	{
		UpdateHighlight(false);
	}
}

void UPostHighlightComponent::UpdateHighlight(const bool bEnable)
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

TArray<UPrimitiveComponent*> UPostHighlightComponent::GetRelevantComponents() const
{
	TArray<UPrimitiveComponent*> Results;
	if (const AActor* Owner = GetOwner())
	{
		Owner->GetComponents<UPrimitiveComponent>(Results);
	}
	return Results;
}
