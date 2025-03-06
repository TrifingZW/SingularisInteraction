/* =====================================================================
 * HighlightComponent.h
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HighlightComponent.generated.h"

UCLASS(ClassGroup=("引力奇点交互插件"), meta=(BlueprintSpawnableComponent))
class SINGULARISINTERACTION_API UHighlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHighlightComponent();

	UFUNCTION(BlueprintCallable, Category = "Highlight")
	void EnableHighlight();

	UFUNCTION(BlueprintCallable, Category = "Highlight")
	void DisableHighlight();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	int32 HighlightCount;
	TMap<TWeakObjectPtr<UPrimitiveComponent>, bool> OriginalStates;

	void UpdateHighlight(bool bEnable);
	TArray<UPrimitiveComponent*> GetRelevantComponents() const;
};
