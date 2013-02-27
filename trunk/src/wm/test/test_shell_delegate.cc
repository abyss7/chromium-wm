// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/test/test_shell_delegate.h"

#include "ash/caps_lock_delegate_stub.h"
#include "ash/test/test_launcher_delegate.h"
#include "content/public/test/test_browser_context.h"
#include "wm/host/root_window_host_factory.h"

namespace wm {
namespace test {

TestShellDelegate::TestShellDelegate()
    : screen_magnifier_enabled_(false),
      screen_magnifier_type_(ash::kDefaultMagnifierType) {
}

TestShellDelegate::~TestShellDelegate() {
}

bool TestShellDelegate::IsUserLoggedIn() const {
  return true;
}

bool TestShellDelegate::IsSessionStarted() const {
  return true;
}

bool TestShellDelegate::IsFirstRunAfterBoot() const {
  return false;
}

bool TestShellDelegate::CanLockScreen() const {
  return false;
}

void TestShellDelegate::LockScreen() {
}

void TestShellDelegate::UnlockScreen() {
}

bool TestShellDelegate::IsScreenLocked() const {
  return false;
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

string16 TestShellDelegate::GetTimeRemainingString(base::TimeDelta delta) {
  return string16();
}

string16 TestShellDelegate::GetTimeDurationLongString(base::TimeDelta delta) {
  return string16();
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
  return RootWindowHostFactory::Create();
}

string16 TestShellDelegate::GetProductName() const {
  return string16();
}

}  // namespace test
}  // namespace wm
