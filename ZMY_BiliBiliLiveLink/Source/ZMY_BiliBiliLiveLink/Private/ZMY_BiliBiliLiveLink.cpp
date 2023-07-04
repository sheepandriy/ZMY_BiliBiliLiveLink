// Copyright Epic Games, Inc. All Rights Reserved.

#include "ZMY_BiliBiliLiveLink.h"

#define LOCTEXT_NAMESPACE "FZMY_BiliBiliLiveLinkModule"

void FZMY_BiliBiliLiveLinkModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FZMY_BiliBiliLiveLinkModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FZMY_BiliBiliLiveLinkModule, ZMY_BiliBiliLiveLink)