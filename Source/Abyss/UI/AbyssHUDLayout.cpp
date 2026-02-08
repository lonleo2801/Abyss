// Copyright (c) 2025 Leon Lee

#include "AbyssHUDLayout.h"
#if WITH_EDITOR
#include "CommonUIVisibilitySubsystem.h"
#endif // WITH_EDITOR

#include "AbilitySystem/Attributes/AbyssAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CommonTextBlock.h"
#include "Actors/Weapons/AbyssWeaponBase.h"
#include "CommonUIExtensions.h"
#include "CommonUISettings.h"
#include "Components/Gameplay/AbyssInventoryComponent.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "GameFramework/InputDeviceSubsystem.h"
#include "GameFramework/InputSettings.h"
#include "GameplayTags/AbyssTags.h"
#include "GameplayTags/AbyssUITags.h"
#include "ICommonUIModule.h"
#include "Input/CommonUIInputTypes.h"

// UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_LAYER_MENU, "UI.Layer.Menu");
// UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_ACTION_ESCAPE, "UI.Action.Escape");
// UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Platform_Trait_Input_PrimarlyController,
// "Platform.Trait.Input.PrimarlyController");

UAbyssHUDLayout::UAbyssHUDLayout(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer), SpawnedControllerDisconnectScreen(nullptr) {
  // By default, only primarily controller platforms require a disconnect
  // screen. 默认情况下，只有主要的控制器平台才需要设置断开连接界面。
  // PlatformRequiresControllerDisconnectScreen.AddTag(TAG_Platform_Trait_Input_PrimarlyController);
}

void UAbyssHUDLayout::NativeOnInitialized() {
  Super::NativeOnInitialized();

  // 绑定退出按键
  RegisterUIActionBinding(FBindUIActionArgs(
      BackAction, false,
      FSimpleDelegate::CreateUObject(this, &ThisClass::HandleEscapeAction)));

  // If we can display a controller disconnect screen, then listen for the
  // controller state change delegates
  // 如果我们能够显示控制器断开连接的界面，那么就监听控制器状态变化的委托方法
  if (ShouldPlatformDisplayControllerDisconnectScreen()) {
    // Bind to when input device connections change
    // 监听输入设备连接状态的变化情况
    IPlatformInputDeviceMapper &DeviceMapper =
        IPlatformInputDeviceMapper::Get();
    DeviceMapper.GetOnInputDeviceConnectionChange().AddUObject(
        this, &ThisClass::HandleInputDeviceConnectionChanged);
    DeviceMapper.GetOnInputDevicePairingChange().AddUObject(
        this, &ThisClass::HandleInputDevicePairingChanged);
  }

  // --- Bind to Character & Inventory ---
  if (AAbyssPawn *Char = GetAbyssPawn()) {
    // 1. Health
    if (UAbilitySystemComponent *ASC = Char->GetAbilitySystemComponent()) {
      // Bind to Attribute Changes
      ASC->GetGameplayAttributeValueChangeDelegate(
             UAbyssAttributeSet::GetHealthAttribute())
          .AddUObject(this, &ThisClass::OnHealthChanged);
      ASC->GetGameplayAttributeValueChangeDelegate(
             UAbyssAttributeSet::GetMaxHealthAttribute())
          .AddUObject(this, &ThisClass::OnMaxHealthChanged);
    }

    // 2. Inventory (Weapons & Ammo)
    if (UAbyssInventoryComponent *Inventory = GetInventoryComponent()) {
      Inventory->OnInventoryUpdated.AddDynamic(this,
                                               &ThisClass::OnInventoryUpdated);
      Inventory->OnAmmoChanged.AddDynamic(
          this, &ThisClass::UpdateAmmo);
    }
  }

  // Initial Update
  UpdateHealth();
  UpdateWeaponInventory();
  UpdateAmmo();
}

void UAbyssHUDLayout::NativeDestruct() {
  Super::NativeDestruct();

  // Remove bindings to input device connection changing
  // 移除与输入设备连接变化相关的绑定操作
  IPlatformInputDeviceMapper &DeviceMapper = IPlatformInputDeviceMapper::Get();
  DeviceMapper.GetOnInputDeviceConnectionChange().RemoveAll(this);
  DeviceMapper.GetOnInputDevicePairingChange().RemoveAll(this);

  if (RequestProcessControllerStateHandle.IsValid()) {
    FTSTicker::GetCoreTicker().RemoveTicker(
        RequestProcessControllerStateHandle);
    RequestProcessControllerStateHandle.Reset();
  }
}

void UAbyssHUDLayout::HandleEscapeAction() {
  if (ensure(!EscapeMenuClass.IsNull())) {
    UCommonUIExtensions::PushStreamedContentToLayer_ForPlayer(
        GetOwningLocalPlayer(), AbyssTags_UI::Layer::GameMenu, EscapeMenuClass);
  }
}

void UAbyssHUDLayout::HandleInputDeviceConnectionChanged(
    EInputDeviceConnectionState NewConnectionState,
    FPlatformUserId PlatformUserId, FInputDeviceId InputDeviceId) {
  const FPlatformUserId OwningLocalPlayerId =
      GetOwningLocalPlayer()->GetPlatformUserId();

  ensure(OwningLocalPlayerId.IsValid());

  // This device connection change happened to a different player, ignore it for
  // us. 这次设备连接变化发生在另一个玩家身上，我们暂且忽略这一情况。
  if (PlatformUserId != OwningLocalPlayerId) {
    // 如果需要看到这个界面的话 需要把这个return注释掉. 因为当控制器失去连接时
    // PlatformUserId为-1
    return;
  }

  NotifyControllerStateChangeForDisconnectScreen();
}

void UAbyssHUDLayout::HandleInputDevicePairingChanged(
    FInputDeviceId InputDeviceId, FPlatformUserId NewUserPlatformId,
    FPlatformUserId OldUserPlatformId) {
  const FPlatformUserId OwningLocalPlayerId =
      GetOwningLocalPlayer()->GetPlatformUserId();

  ensure(OwningLocalPlayerId.IsValid());

  // If this pairing change was related to our local player, notify of a change.
  // 如果此次角色更换与我们本地玩家有关，则通知这一变化。
  if (NewUserPlatformId == OwningLocalPlayerId ||
      OldUserPlatformId == OwningLocalPlayerId) {
    NotifyControllerStateChangeForDisconnectScreen();
  }
}

void UAbyssHUDLayout::NotifyControllerStateChangeForDisconnectScreen() {
  // We should only ever get here if we have bound to the controller state
  // change delegates 只有当我们已绑定到控制器的状态变化委托时，才会到达此处。
  ensure(ShouldPlatformDisplayControllerDisconnectScreen());

  // If we haven't already, queue the processing of device state for next tick.
  // 如果尚未进行操作，则将设备状态的处理任务排入下一次循环的处理队列中。
  if (!RequestProcessControllerStateHandle.IsValid()) {
    RequestProcessControllerStateHandle = FTSTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateWeakLambda(this, [this](float DeltaTime) {
          RequestProcessControllerStateHandle.Reset();
          ProcessControllerDevicesHavingChangedForDisconnectScreen();
          return false;
        }));
  }
}

void UAbyssHUDLayout::
    ProcessControllerDevicesHavingChangedForDisconnectScreen() {
  // We should only ever get here if we have bound to the controller state
  // change delegates 只有当我们已绑定到控制器的状态变化委托时，才会到达此处。
  ensure(ShouldPlatformDisplayControllerDisconnectScreen());

  const FPlatformUserId OwningLocalPlayerId =
      GetOwningLocalPlayer()->GetPlatformUserId();

  ensure(OwningLocalPlayerId.IsValid());

  // Get all input devices mapped to our player
  // 获取所有与我们的玩家关联的输入设备
  const IPlatformInputDeviceMapper &DeviceMapper =
      IPlatformInputDeviceMapper::Get();
  TArray<FInputDeviceId> MappedInputDevices;
  const int32 NumDevicesMappedToUser = DeviceMapper.GetAllInputDevicesForUser(
      OwningLocalPlayerId, OUT MappedInputDevices);

  // Check if there are any other connected GAMEPAD devices mapped to this
  // platform user.
  // 检查是否还有其他与该平台用户关联的遊戲控制器設備被映射到此平台上。
  bool bHasConnectedController = false;

  for (const FInputDeviceId MappedDevice : MappedInputDevices) {
    if (DeviceMapper.GetInputDeviceConnectionState(MappedDevice) ==
        EInputDeviceConnectionState::Connected) {
      const FHardwareDeviceIdentifier HardwareInfo =
          UInputDeviceSubsystem::Get()->GetInputDeviceHardwareIdentifier(
              MappedDevice);
      if (HardwareInfo.PrimaryDeviceType ==
          EHardwareDevicePrimaryType::Gamepad) {
        bHasConnectedController = true;
      }
    }
  }

  // If there are no gamepad input devices mapped to this user, then we want to
  // pop the toast saying to re-connect them
  // 如果没有将游戏手柄输入设备与该用户关联起来，那么我们就需要弹出提示信息，告知用户需要重新连接这些设备。
  if (!bHasConnectedController) {
    DisplayControllerDisconnectedMenu();
  }
  // Otherwise we can hide the screen if it is currently being shown
  // 另外，如果屏幕当前处于显示状态，我们可以将其隐藏起来
  else if (SpawnedControllerDisconnectScreen) {
    HideControllerDisconnectedMenu();
  }
}

bool UAbyssHUDLayout::ShouldPlatformDisplayControllerDisconnectScreen() const {
  // We only want this menu on primarily controller platforms
  // 我们只希望此菜单出现在主要的控制器平台上。
  bool bHasAllRequiredTags =
      ICommonUIModule::GetSettings().GetPlatformTraits().HasAll(
          PlatformRequiresControllerDisconnectScreen);

  // Check the tags that we may be emulating in the editor too
  // 检查我们在编辑器中可能要模拟的标签
#if WITH_EDITOR
  const FGameplayTagContainer &PlatformEmulationTags =
      UCommonUIVisibilitySubsystem::Get(GetOwningLocalPlayer())
          ->GetVisibilityTags();
  bHasAllRequiredTags |=
      PlatformEmulationTags.HasAll(PlatformRequiresControllerDisconnectScreen);
#endif // WITH_EDITOR

  return bHasAllRequiredTags;
}

void UAbyssHUDLayout::HideControllerDisconnectedMenu_Implementation() {
  /*UE_LOG(LogLyra, Log, TEXT("[%hs] Display controller disconnected menu!"),
  __func__);

  if (ControllerDisconnectedScreen)
  {
          // Push the "controller disconnected" widget to the menu layer
          // 将“控制器断开连接”小部件推至菜单层
          SpawnedControllerDisconnectScreen =
  UCommonUIExtensions::PushContentToLayer_ForPlayer(GetOwningLocalPlayer(),
  TAG_UI_LAYER_MENU, ControllerDisconnectedScreen);
  }*/
}

void UAbyssHUDLayout::
    DisplayControllerDisconnectedMenu_Implementation() { /*
                                                                UE_LOG(LogLyra,
                                                            Log, TEXT("[%hs]
                                                            Hide controller
                                                            disconnected
                                                            menu!"), __func__);

                                                                UCommonUIExtensions::PopContentFromLayer(SpawnedControllerDisconnectScreen);
                                                                SpawnedControllerDisconnectScreen
                                                            = nullptr;*/
}

// ============================================================================
// Logic Implementation
// ============================================================================

AAbyssPawn *UAbyssHUDLayout::GetAbyssPawn() const {
  return Cast<AAbyssPawn>(GetOwningPlayerPawn());
}

UAbyssInventoryComponent *UAbyssHUDLayout::GetInventoryComponent() const {
  if (AAbyssPawn *Char = GetAbyssPawn()) {
    return Char->GetInventoryComponent();
  }
  return nullptr;
}

void UAbyssHUDLayout::UpdateHealth() {
  if (!HealthBar)
    return;

  if (AAbyssPawn *Char = GetAbyssPawn()) {
    if (UAbilitySystemComponent *ASC = Char->GetAbilitySystemComponent()) {
      float Current = 0.f;
      float Max = 1.f;

      if (ASC->HasAttributeSetForAttribute(
              UAbyssAttributeSet::GetHealthAttribute())) {
        Current =
            ASC->GetNumericAttribute(UAbyssAttributeSet::GetHealthAttribute());
      }
      if (ASC->HasAttributeSetForAttribute(
              UAbyssAttributeSet::GetMaxHealthAttribute())) {
        Max = ASC->GetNumericAttribute(
            UAbyssAttributeSet::GetMaxHealthAttribute());
      }

      if (Max > 0.f) {
        HealthBar->SetPercent(Current / Max);
      }
    }
  }
}

void UAbyssHUDLayout::UpdateWeaponInventory() {
  UAbyssInventoryComponent *Inventory = GetInventoryComponent();
  if (!Inventory)
    return;

  // Helper to set icon
  auto SetIcon = [](UImage *Image, AAbyssWeaponBase *Weapon) {
    if (Image) {
      if (Weapon && Weapon->WeaponDef && Weapon->WeaponDef->Icon) {
        Image->SetBrushFromTexture(Weapon->WeaponDef->Icon);
        Image->SetVisibility(ESlateVisibility::Visible);
      } else {
        Image->SetVisibility(ESlateVisibility::Hidden); // Or SetBrush to empty
      }
    }
  };

  // 1. Primary
  AAbyssWeaponBase *Primary =
      Inventory->GetLastUsedWeaponInSlot(EWeaponSlot::Primary);
  SetIcon(PrimaryWeaponIcon, Primary);

  // 2. Secondary
  AAbyssWeaponBase *Secondary =
      Inventory->GetLastUsedWeaponInSlot(EWeaponSlot::Secondary);
  SetIcon(SecondaryWeaponIcon, Secondary);

  // 3. Melee
  AAbyssWeaponBase *Melee =
      Inventory->GetLastUsedWeaponInSlot(EWeaponSlot::Melee);
  SetIcon(MeleeWeaponIcon, Melee);
}

void UAbyssHUDLayout::UpdateAmmo() {
  if (!CurrentClipAmmo || !ReserveAmmo)
    return;

  UAbyssInventoryComponent *Inventory = GetInventoryComponent();
  if (!Inventory) {
    CurrentClipAmmo->SetText(FText::GetEmpty());
    ReserveAmmo->SetText(FText::GetEmpty());
    return;
  }

  AAbyssWeaponBase *CurrentWeapon = Inventory->GetCurrentWeapon();
  if (CurrentWeapon) {
    int32 Clip = CurrentWeapon->GetCurrentAmmo();
    EAmmoType AmmoType = CurrentWeapon->GetAmmoType();
    int32 Reserve = Inventory->GetReserveAmmo(AmmoType);

    CurrentClipAmmo->SetText(FText::AsNumber(Clip));
    ReserveAmmo->SetText(FText::AsNumber(Reserve));
  } else {
    CurrentClipAmmo->SetText(FText::GetEmpty());
    ReserveAmmo->SetText(FText::GetEmpty());
  }
}

void UAbyssHUDLayout::OnHealthChanged(const FOnAttributeChangeData &Data) {
  UpdateHealth();
}

void UAbyssHUDLayout::OnMaxHealthChanged(const FOnAttributeChangeData &Data) {
  UpdateHealth();
}

void UAbyssHUDLayout::OnInventoryUpdated() { UpdateWeaponInventory(); }

