// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace AbyssTags_UI
{
	namespace WidgetStack
	{
		//最下面的能Disable最上面的
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(MainMenu); //（主菜单阶段）
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD);//（血条、技能栏、Buff）
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameMenu);//（暂停、背包）
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Modal);//（弹窗、提示）
	}
}
