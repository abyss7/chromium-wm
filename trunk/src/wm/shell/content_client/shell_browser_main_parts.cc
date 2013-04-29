// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/shell/content_client/shell_browser_main_parts.h"

#include "ash/desktop_background/desktop_background_controller.h"
#include "ash/display/display_controller.h"
#include "ash/shell.h"
#include "ash/shell/window_watcher.h"
#include "content/shell/shell_browser_context.h"
#include "ui/aura/env.h"
#include "ui/aura/root_window.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/views/test/test_views_delegate.h"
#include "wm/foreign_test_window.h"
#include "wm/foreign_window_manager.h"
#include "wm/gpu/foreign_window_texture_factory.h"
#include "wm/shell/shell_delegate_impl.h"

#if defined(ENABLE_MESSAGE_CENTER)
#include "ui/message_center/message_center.h"
#endif

#if defined(OS_LINUX)
#include "ui/base/touch/touch_factory_x11.h"
#endif

#if defined(OS_CHROMEOS)
#include "chromeos/dbus/dbus_thread_manager.h"
#endif

namespace ash {
namespace shell {
void InitWindowTypeLauncher();
}  // namespace shell
}  // namespace ash

namespace wm {
namespace shell {

namespace {
class ShellViewsDelegate : public views::TestViewsDelegate {
 public:
  ShellViewsDelegate() {}
  virtual ~ShellViewsDelegate() {}

  // Overridden from views::TestViewsDelegate:
  virtual views::NonClientFrameView* CreateDefaultNonClientFrameView(
      views::Widget* widget) OVERRIDE {
    return ash::Shell::GetInstance()->CreateDefaultNonClientFrameView(widget);
  }
  bool UseTransparentWindows() const OVERRIDE {
    // Ash uses transparent window frames.
    return true;
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(ShellViewsDelegate);
};

}  // namespace

ShellBrowserMainParts::ShellBrowserMainParts(
    const content::MainFunctionParams& parameters)
    : BrowserMainParts() {
}

ShellBrowserMainParts::~ShellBrowserMainParts() {
}

void ShellBrowserMainParts::PreMainMessageLoopStart() {
#if defined(OS_LINUX)
  ui::TouchFactory::SetTouchDeviceListFromCommandLine();
#endif
}

void ShellBrowserMainParts::PostMainMessageLoopStart() {
#if defined(OS_CHROMEOS)
  chromeos::DBusThreadManager::Initialize();
#endif
}

void ShellBrowserMainParts::PreMainMessageLoopRun() {
  ForeignWindowTextureFactory::Initialize();

  browser_context_.reset(new content::ShellBrowserContext(false));

  // A ViewsDelegate is required.
  if (!views::ViewsDelegate::views_delegate)
    views::ViewsDelegate::views_delegate = new ShellViewsDelegate;

  wm::shell::ShellDelegateImpl* delegate = new wm::shell::ShellDelegateImpl;

  foreign_window_manager_.reset(new ForeignWindowManager);
  foreign_window_manager_->InitializeForTesting();
  delegate->SetForeignWindowManager(foreign_window_manager_.get());

#if defined(ENABLE_MESSAGE_CENTER)
  // The global message center state must be initialized absent
  // g_browser_process.
  message_center::MessageCenter::Initialize();
#endif
  ash::Shell::CreateInstance(delegate);
  ash::Shell::GetInstance()->set_browser_context(browser_context_.get());

  window_watcher_.reset(new ash::shell::WindowWatcher);
  delegate->SetWatcher(window_watcher_.get());

  ash::shell::InitWindowTypeLauncher();

  ash::DesktopBackgroundController* controller =
      ash::Shell::GetInstance()->desktop_background_controller();
  if (controller->GetAppropriateResolution() == ash::WALLPAPER_RESOLUTION_LARGE)
    controller->SetDefaultWallpaper(ash::kDefaultLargeWallpaper);
  else
    controller->SetDefaultWallpaper(ash::kDefaultSmallWallpaper);

  foreign_window_manager_->SetDisplay(
      ash::DisplayController::GetPrimaryDisplay());
  foreign_window_manager_->SetDefaultCursor(ui::kCursorPointer);

  ForeignTestWindow::CreateParams params(foreign_window_manager_.get());
  foreign_test_window_.reset(new ForeignTestWindow(params));
  foreign_test_window_->Show();

  ash::Shell::GetPrimaryRootWindow()->ShowRootWindow();
}

void ShellBrowserMainParts::PostMainMessageLoopRun() {
  browser_context_.reset();
  window_watcher_.reset();
  ash::Shell::DeleteInstance();
#if defined(ENABLE_MESSAGE_CENTER)
  // The global message center state must be shutdown absent
  // g_browser_process.
  message_center::MessageCenter::Shutdown();
#endif
  foreign_window_manager_.reset();
  aura::Env::DeleteInstance();
  ForeignWindowTextureFactory::Terminate();
}

bool ShellBrowserMainParts::MainMessageLoopRun(int* result_code) {
  MessageLoopForUI::current()->Run();
  return true;
}

}  // namespace shell
}  // namespace wm
