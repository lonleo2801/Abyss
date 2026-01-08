// Copyright (c) 2025 Leon Lee

#pragma once

#include "CoreMinimal.h"
#include "AbyssActivatableWidget.h"
#include "GameplayTagContainer.h"
#include "AbyssHUDLayout.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class ABYSS_API UAbyssHUDLayout : public UAbyssActivatableWidget
{
	GENERATED_BODY()

	public:
	
	// 限定断开链接对应的主要控制器Tag
	UAbyssHUDLayout(const FObjectInitializer& ObjectInitializer);

	
	// 绑定退出及断连的回调
	virtual void NativeOnInitialized() override;
	
	virtual void NativeDestruct() override;

	//要绑定的退出键
	UPROPERTY(EditDefaultsOnly, Category = "UI Options Screen", meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle BackAction;

protected:
	// 将退出菜单推送到主界面
	void HandleEscapeAction();
	
	/** 
	* Callback for when controllers are disconnected. This will check if the player now has 
	* no mapped input devices to them, which would mean that they can't play the game.
	* 
	* If this is the case, then call DisplayControllerDisconnectedMenu.
	*/
	/**
	 * 当控制器断开连接时的回调函数。此函数会检查玩家当前是否不再拥有与之对应的映射输入设备，如果不再有此类设备，则意味着玩家无法继续玩游戏。
	  * 如果情况确实如此，那么就调用 DisplayControllerDisconnectedMenu 这个函数。
	 * 
	 */
	void HandleInputDeviceConnectionChanged(EInputDeviceConnectionState NewConnectionState, FPlatformUserId PlatformUserId, FInputDeviceId InputDeviceId);

	/**
	* Callback for when controllers change their owning platform user. We will use this to check
	* if we no longer need to display the "Controller Disconnected" menu
	*/
	/**
	 * 当控制器更换其所属平台用户时的回调函数。我们将利用此功能来检查是否不再需要显示“控制器断开连接”菜单。
	 * 
	 */
	void HandleInputDevicePairingChanged(FInputDeviceId InputDeviceId, FPlatformUserId NewUserPlatformId, FPlatformUserId OldUserPlatformId);
	

	/**
	* Notify this widget that the state of controllers for the player have changed. Queue a timer for next tick to 
	* process them and see if we need to show/hide the "controller disconnected" widget.
	*/
	/**
	 * 	通知此控件，玩家控制器的状态已发生变化。为下一次循环设置一个计时器，以便处理这些状态变化，并检查是否需要显示/隐藏“控制器断开连接”控件。
	 */
	void NotifyControllerStateChangeForDisconnectScreen();

	/**
	 * This will check the state of the connected controllers to the player. If they do not have
	 * any controllers connected to them, then we should display the Disconnect menu. If they do have
	 * controllers connected to them, then we can hide the disconnect menu if its showing.
	 */
	/**
	 * 	这将检查与玩家相连的控制器的状态。如果他们没有连接任何控制器，那么我们就应该显示“断开连接”菜单。如果他们确实连接了控制器，那么如果“断开连接”菜单正在显示，我们就应该将其隐藏起来。
	 * 	
	 */
	virtual void ProcessControllerDevicesHavingChangedForDisconnectScreen();
	


	
	/**
	 * Returns true if this platform supports a "controller disconnected" screen. 
	 */
	/**
	 * 如果此平台支持“控制器断开连接”的界面，则返回真值。
	*/
	virtual bool ShouldPlatformDisplayControllerDisconnectScreen() const;
	
	/**
	* Pushes the ControllerDisconnectedMenuClass to the Menu layer (UI.Layer.Menu)
	*/
	/**
	 *  将“ControllerDisconnectedMenuClass”对象推送到“菜单”层（即“UI.Layer.Menu”）上。
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Controller Disconnect Menu")
	void DisplayControllerDisconnectedMenu();

	/**
	* Hides the controller disconnected menu if it is active.
	*/
	/**
	 * 如果控制器已处于断开连接状态，则隐藏相应的“控制器断开连接”菜单。
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Controller Disconnect Menu")
	void HideControllerDisconnectedMenu();
protected:

	/**
	 * The menu to be displayed when the user presses the "Pause" or "Escape" button
	 * 当用户按下“暂停”或“退出”按钮时将显示的菜单内容
	 */
	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UCommonActivatableWidget> EscapeMenuClass;

	/** 
	* The widget which should be presented to the user if all of their controllers are disconnected.
	* 当用户的所有控制器均处于断开状态时，应向用户展示的这个小部件。
	*/
	//UPROPERTY(EditDefaultsOnly, Category="Controller Disconnect Menu")
	//TSubclassOf<ULyraControllerDisconnectedScreen> ControllerDisconnectedScreen;

	/**
	 * The platform tags that are required in order to show the "Controller Disconnected" screen.
	 *
	 * If these tags are not set in the INI file for this platform, then the controller disconnect screen
	 * will not ever be displayed. 
	 */
	/**
	 * 显示“控制器断开连接”界面所需的平台标签。
	 * 如果在该平台的 INI 文件中未设置这些标签，那么控制台断开连接的屏幕将永远不会显示出来。
	 * 
	 */
	UPROPERTY(EditDefaultsOnly, Category="Controller Disconnect Menu")
	FGameplayTagContainer PlatformRequiresControllerDisconnectScreen;


	/** Pointer to the active "Controller Disconnected" menu if there is one. */
	/** 指向当前“控制器断开连接”菜单的指针（如果有该菜单的话）。*/
	UPROPERTY(Transient)
	TObjectPtr<UCommonActivatableWidget> SpawnedControllerDisconnectScreen;

	
	/** Handle from the FSTicker for when we want to process the controller state of our player */
	/** 处理来自 FSTicker 的内容，用于在我们需要处理玩家控制器状态时进行操作 */
	FTSTicker::FDelegateHandle RequestProcessControllerStateHandle;
	
	

};
