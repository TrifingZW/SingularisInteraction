/* =====================================================================
 * InteractionWidget.cpp
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#include "InteractionWidget.h"
#include "Components/Image.h"

void UInteractionWidget::ShowWidget()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UInteractionWidget::HideWidget()
{
	SetVisibility(ESlateVisibility::Hidden);
}
