/* =====================================================================
 * InteractionWidget.cpp
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2025 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024-2025 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#include "Widgets/InteractionManagerWidget.h"

#include "Components/Image.h"

void UInteractionManagerWidget::ShowWidget()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UInteractionManagerWidget::HideWidget()
{
	SetVisibility(ESlateVisibility::Hidden);
}
