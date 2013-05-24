// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WM_TEST_TEST_SHELL_DELEGATE_H_
#define WM_TEST_TEST_SHELL_DELEGATE_H_

#include "ash/shell_delegate.h"
#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"

namespace ash {
namespace test {
class TestSessionStateDelegate;
}
}

namespace wm {
class ForeignWindowManager;

namespace test {

class TestShellDelegate : public ash::ShellDelegate {
 public:
  TestShellDelegate();
  virtual ~TestShellDelegate();

  // Overridden from ShellDelegate:
  virtual bool IsFirstRunAfterBoot() const OVERRIDE;
  virtual bool IsMultiProfilesEnabled() const OVERRIDE;
  virtual bool IsRunningInForcedAppMode() const OVERRIDE;
  virtual void PreInit() OVERRIDE;
  virtual void Shutdown() OVERRIDE;
  virtual void Exit() OVERRIDE;
  virtual void NewTab() OVERRIDE;
  virtual void NewWindow(bool incognito) OVERRIDE;
  virtual void ToggleFullscreen() OVERRIDE;
  virtual void ToggleMaximized() OVERRIDE;
  virtual void OpenFileManager(bool as_dialog) OVERRIDE;
  virtual void OpenCrosh() OVERRIDE;
  virtual void OpenMobileSetup(const std::string& service_path) OVERRIDE;
  virtual void RestoreTab() OVERRIDE;
  virtual bool RotatePaneFocus(ash::Shell::Direction direction) OVERRIDE;
  virtual void ShowKeyboardOverlay() OVERRIDE;
  virtual keyboard::KeyboardControllerProxy*
      CreateKeyboardControllerProxy() OVERRIDE;
  virtual void ShowTaskManager() OVERRIDE;
  virtual content::BrowserContext* GetCurrentBrowserContext() OVERRIDE;
  virtual void ToggleSpokenFeedback(
      ash::AccessibilityNotificationVisibility notify) OVERRIDE;
  virtual bool IsSpokenFeedbackEnabled() const OVERRIDE;
  virtual void ToggleHighContrast() OVERRIDE;
  virtual bool IsHighContrastEnabled() const OVERRIDE;
  virtual void SetMagnifierEnabled(bool enabled) OVERRIDE;
  virtual void SetMagnifierType(ash::MagnifierType type) OVERRIDE;
  virtual bool IsMagnifierEnabled() const OVERRIDE;
  virtual ash::MagnifierType GetMagnifierType() const OVERRIDE;
  virtual bool ShouldAlwaysShowAccessibilityMenu() const OVERRIDE;
  virtual void SilenceSpokenFeedback() const OVERRIDE;
  virtual app_list::AppListViewDelegate* CreateAppListViewDelegate() OVERRIDE;
  virtual ash::LauncherDelegate* CreateLauncherDelegate(
      ash::LauncherModel* model) OVERRIDE;
  virtual ash::SystemTrayDelegate* CreateSystemTrayDelegate() OVERRIDE;
  virtual ash::UserWallpaperDelegate* CreateUserWallpaperDelegate() OVERRIDE;
  virtual ash::CapsLockDelegate* CreateCapsLockDelegate() OVERRIDE;
  virtual ash::SessionStateDelegate* CreateSessionStateDelegate() OVERRIDE;
  virtual aura::client::UserActionClient* CreateUserActionClient() OVERRIDE;
  virtual void OpenFeedbackPage() OVERRIDE;
  virtual void RecordUserMetricsAction(ash::UserMetricsAction action) OVERRIDE;
  virtual void HandleMediaNextTrack() OVERRIDE;
  virtual void HandleMediaPlayPause() OVERRIDE;
  virtual void HandleMediaPrevTrack() OVERRIDE;
  virtual base::string16 GetTimeRemainingString(
      base::TimeDelta delta) OVERRIDE;
  virtual base::string16 GetTimeDurationLongString(
      base::TimeDelta delta) OVERRIDE;
  virtual void SaveScreenMagnifierScale(double scale) OVERRIDE;
  virtual double GetSavedScreenMagnifierScale() OVERRIDE;
  virtual ui::MenuModel* CreateContextMenu(aura::RootWindow* root) OVERRIDE;
  virtual ash::RootWindowHostFactory* CreateRootWindowHostFactory() OVERRIDE;
  virtual string16 GetProductName() const OVERRIDE;

 private:
  bool screen_magnifier_enabled_;
  ash::MagnifierType screen_magnifier_type_;

  scoped_ptr<content::BrowserContext> current_browser_context_;

  ash::test::TestSessionStateDelegate* test_session_state_delegate_;

  DISALLOW_COPY_AND_ASSIGN(TestShellDelegate);
};

}  // namespace test
}  // namespace wm

#endif  // WM_TEST_TEST_SHELL_DELEGATE_H_
