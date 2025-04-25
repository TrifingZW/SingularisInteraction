/* =====================================================================
 * PostHighlightComponent.cpp
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2025 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024-2025 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#include "Components/HighlightComponent.h"

UHighlightComponent::UHighlightComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UHighlightComponent::BeginPlay()
{
	Super::BeginPlay();

	for (auto HighlightPrimitive : HighlightPrimitives)
		if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(HighlightPrimitive.GetComponent(GetOwner())))
			Primitives.Add(PrimitiveComponent);
}

void UHighlightComponent::AddPrimitive(UPrimitiveComponent* PrimitiveComponent)
{
	if (!PrimitiveComponent) return;

	Primitives.Add(PrimitiveComponent);

	if (bIsEnabled)
		SetPrimitiveHighlight(PrimitiveComponent, true);
}

void UHighlightComponent::RemovePrimitive(UPrimitiveComponent* PrimitiveComponent)
{
	if (!PrimitiveComponent) return;

	Primitives.Remove(PrimitiveComponent);

	if (bIsEnabled)
		SetPrimitiveHighlight(PrimitiveComponent, false);
}

void UHighlightComponent::ClearPrimitive()
{
	Disable();
	Primitives.Empty();
}

void UHighlightComponent::Enable()
{
	for (const auto PrimitiveComponent : Primitives)
		if (PrimitiveComponent.IsValid())
			SetPrimitiveHighlight(PrimitiveComponent.Get(), true);
}

void UHighlightComponent::Disable()
{
	for (const auto PrimitiveComponent : Primitives)
		if (PrimitiveComponent.IsValid())
			SetPrimitiveHighlight(PrimitiveComponent.Get(), false);
}

void UHighlightComponent::SetPrimitiveHighlight(UPrimitiveComponent* PrimitiveComponent, const bool bEnable)
{
	if (!PrimitiveComponent) return;

	PrimitiveComponent->SetRenderCustomDepth(bEnable);
	PrimitiveComponent->SetCustomDepthStencilValue(bEnable ? 1 : 0);
}
