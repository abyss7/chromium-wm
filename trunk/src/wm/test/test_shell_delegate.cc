// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/test/test_shell_delegate.h"

#include "ash/caps_lock_delegate_stub.h"
#include "ash/test/test_launcher_delegate.h"
#include "ash/test/test_session_state_delegate.h"
#include "content/public/test/test_browser_context.h"
#include "wm/foreign_window_manager.h"

namespace wm {
namespace test {

TestShellDelegate::TestShellDelegate()
    : screen_magnifier_enabled_(false),
      screen_magnifier_type_(ash::kDefaultMagnifierType),
      test_session_state_delegate_(NULL) {
}

TestShellDelegate::~TestShellDelegate() {
}

bool TestShellDelegate::IsFirstRunAfterBoot() const {
  return false;
}

bool TestShellDelegate::IsMultiProfilesEnabled() const {
  return false;
}

bool TestShellDelegate::IsRunningInForcedAppMode() const {
  return false;
}

void TestShellDelegate::PreInit() {
}

void TestShellDelegate::Shutdown() {
}

void TestShellDelegate::Exit() {
}

void TestShellDelegate::NewTab() {
}

void TestShellDelegate::NewWindow(bool incognito) {
}

void TestShellDelegate::ToggleMaximized() {
}

void TestShellDelegate::ToggleFullscreen() {
}

void TestShellDelegate::OpenFileManager(bool as_dialog) {
}

void TestShellDelegate::OpenCrosh() {
}

void TestShellDelegate::OpenMobileSetup(const std::string& service_path) {
}

void TestShellDelegate::RestoreTab() {
}

bool TestShellDelegate::RotatePaneFocus(ash::Shell::Direction direction) {
  return true;
}

void TestShellDelegate::ShowKeyboardOverlay() {
}

keyboard::KeyboardControllerProxy*
    TestShellDelegate::CreateKeyboardControllerProxy() {
  return NULL;
}

void TestShellDelegate::ShowTaskManager() {
}

content::BrowserContext* TestShellDelegate::GetCurrentBrowserContext() {
  current_browser_context_.reset(new content::TestBrowserContext());
  return current_browser_context_.get();
}

void TestShellDelegate::ToggleSpokenFeedback(
    ash::AccessibilityNotificationVisibility notify) {
}

bool TestShellDelegate::IsSpokenFeedbackEnabled() const {
  return false;
}

void TestShellDelegate::ToggleHighContrast() {
}

bool TestShellDelegate::IsHighContrastEnabled() const {
  return false;
}

void TestShellDelegate::SetMagnifierEnabled(bool enabled) {
  screen_magnifier_enabled_ = enabled;
}

void TestShellDelegate::SetMagnifierType(ash::MagnifierType type) {
  screen_magnifier_type_ = type;
}

bool TestShellDelegate::IsMagnifierEnabled() const {
  return screen_magnifier_enabled_;
}

ash::MagnifierType TestShellDelegate::GetMagnifierType() const {
  return screen_magnifier_type_;
}

bool TestShellDelegate::ShouldAlwaysShowAccessibilityMenu() const {
  return false;
}

void TestShellDelegate::SilenceSpokenFeedback() const {
}

app_list::AppListViewDelegate* TestShellDelegate::CreateAppListViewDelegate() {
  return NULL;
}

ash::LauncherDelegate* TestShellDelegate::CreateLauncherDelegate(
    ash::LauncherModel* model) {
  return new ash::test::TestLauncherDelegate(model);
}

ash::SystemTrayDelegate* TestShellDelegate::CreateSystemTrayDelegate() {
  return NULL;
}

ash::UserWallpaperDelegate* TestShellDelegate::CreateUserWallpaperDelegate() {
  return NULL;
}

ash::SessionStateDelegate* TestShellDelegate::CreateSessionStateDelegate() {
  DCHECK(!test_session_state_delegate_);
  test_session_state_delegate_ = new ash::test::TestSessionStateDelegate;
  return test_session_state_delegate_;
}

ash::CapsLockDelegate* TestShellDelegate::CreateCapsLockDelegate() {
  return new ash::CapsLockDelegateStub;
}

aura::client::UserActionClient* TestShellDelegate::CreateUserActionClient() {
  return NULL;
}

void TestShellDelegate::OpenFeedbackPage() {
}

void TestShellDelegate::RecordUserMetricsAction(
    ash::UserMetricsAction action) {
}

void TestShellDelegate::HandleMediaNextTrack() {
}

void TestShellDelegate::HandleMediaPlayPause() {
}

void TestShellDelegate::HandleMediaPrevTrack() {
}

base::string16 TestShellDelegate::GetTimeRemainingString(
    base::TimeDelta delta) {
  return base::string16();
}

base::string16 TestShellDelegate::GetTimeDurationLongString(
    base::TimeDelta delta) {
  return base::string16();
}

void TestShellDelegate::SaveScreenMagnifierScale(double scale) {
}

ui::MenuModel* TestShellDelegate::CreateContextMenu(aura::RootWindow* root) {
  return NULL;
}

double TestShellDelegate::GetSavedScreenMagnifierScale() {
  return std::numeric_limits<double>::min();
}

ash::RootWindowHostFactory* TestShellDelegate::CreateRootWindowHostFactory() {
  return ForeignWindowManager::GetInstance()->CreateRootWindowHostFactory();
}

string16 TestShellDelegate::GetProductName() const {
  return string16();
}

}  // namespace test
}  // namespace wm
