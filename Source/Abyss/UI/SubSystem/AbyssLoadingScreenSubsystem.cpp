// Copyright (c) 2025 Leon Lee


#include "AbyssLoadingScreenSubsystem.h"

#include "Blueprint/UserWidget.h"


//////////////////////////////////////////////////////////////////////
// ULyraLoadingScreenSubsystem


UAbyssLoadingScreenSubsystem::UAbyssLoadingScreenSubsystem()
{
}


void UAbyssLoadingScreenSubsystem::SetLoadingScreenContentWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (LoadingScreenWidgetClass != NewWidgetClass)
	{
		LoadingScreenWidgetClass = NewWidgetClass;

		OnLoadingScreenWidgetChanged.Broadcast(LoadingScreenWidgetClass);
	}
}

TSubclassOf<UUserWidget> UAbyssLoadingScreenSubsystem::GetLoadingScreenContentWidget() const
{
	return LoadingScreenWidgetClass;
}