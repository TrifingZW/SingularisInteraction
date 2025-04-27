/* =====================================================================
 * SingularisInteraction.cpp
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2025 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024-2025 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#include "SingularisInteraction.h"

DEFINE_LOG_CATEGORY(LogSingularisInteraction);

void FSingularisInteractionModule::StartupModule()
{
	// 这段代码将在你的模块加载到内存后执行；具体的时间安排在每个模块的.uplugin文件中指定。
}

void FSingularisInteractionModule::ShutdownModule()
{
	// 此函数可能在关闭时被调用以清理你的模块。对于支持动态重载的模块，
	// 我们会在卸载模块之前调用此函数。
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSingularisInteractionModule, SingularisInteraction)
