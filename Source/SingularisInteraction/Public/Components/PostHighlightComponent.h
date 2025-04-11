/* =====================================================================
 * PostHighlightComponent.h
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2025 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024-2025 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#pragma once

#include "CoreMinimal.h"
#include "Components/HighlightComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "PostHighlightComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=("引力奇点交互插件"))
class SINGULARISINTERACTION_API UPostHighlightComponent : public UHighlightComponent
{
	GENERATED_BODY()

	int32 HighlightCount;
	TMap<TWeakObjectPtr<UPrimitiveComponent>, bool> OriginalStates;

public:
	UPostHighlightComponent();

	virtual void BeginPlay() override;
	virtual void EnableHighlight_Implementation(AActor* Actor) override;
	virtual void DisableHighlight_Implementation(AActor* Actor) override;

private:
	void UpdateHighlight(bool bEnable);
	TArray<UPrimitiveComponent*> GetRelevantComponents() const;
};
