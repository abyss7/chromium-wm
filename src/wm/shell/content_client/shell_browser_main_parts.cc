// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/shell/content_client/shell_browser_main_parts.h"

#include "ash/ash_switches.h"
#include "ash/desktop_background/desktop_background_controller.h"
#include "ash/display/display_controller.h"
#include "ash/shell.h"
#include "ash/shell/window_watcher.h"
#include "content/shell/shell_browser_context.h"
#include "ui/aura/env.h"
#include "ui/aura/root_window.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/message_center/message_center.h"
#include "ui/views/test/test_views_delegate.h"
#include "wm/foreign_test_window.h"
#include "wm/foreign_window_manager.h"
#include "wm/gpu/foreign_window_texture_factory.h"
#include "wm/shell/shell_delegate_impl.h"

#if defined(OS_LINUX)
#include "ui/base/touch/touch_factory_x11.h"
#endif

#if defined(OS_CHROMEOS)
#include "chromeos/audio/cras_audio_handler.h"
#include "chromeos/dbus/dbus_thread_manager.h"
#include "chromeos/power/power_manager_handler.h"
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

  // The global message center state must be initialized absent
  // g_browser_process.
  message_center::MessageCenter::Initialize();

#if defined(OS_CHROMEOS)
  if (ash::switches::UseNewAudioHandler()) {
    // Create CrasAuidoHandler for testing since g_browser_process is not
    // created in AshTestBase tests.
    chromeos::CrasAudioHandler::InitializeForTesting();
  }
  chromeos::PowerManagerHandler::Initialize();
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

  // TODO(reveman): Properly integrate with
  // ash::RootWindowController::CreateContainers.
  foreign_window_manager_->CreateContainers(
      ash::Shell::GetPrimaryRootWindow());

  ForeignTestWindow::CreateParams params(foreign_window_manager_.get());
  foreign_test_window_.reset(new ForeignTestWindow(params));
  foreign_test_window_->Show();

  ForeignTestWindow::CreateParams unmanaged_params(
      foreign_window_manager_.get());
  unmanaged_params.bounds = gfx::Rect(10, 10, 300, 200);
  unmanaged_params.managed = false;
  unmanaged_foreign_test_window_.reset(
      new ForeignTestWindow(unmanaged_params));
  unmanaged_foreign_test_window_->Show();

  ash::Shell::GetPrimaryRootWindow()->ShowRootWindow();
}

void ShellBrowserMainParts::PostMainMessageLoopRun() {
  browser_context_.reset();
  window_watcher_.reset();
  ash::Shell::DeleteInstance();
  // The global message center state must be shutdown absent
  // g_browser_process.
  message_center::MessageCenter::Shutdown();

#if defined(OS_CHROMEOS)
  if (ash::switches::UseNewAudioHandler())
    chromeos::CrasAudioHandler::Shutdown();
  chromeos::PowerManagerHandler::Shutdown();
#endif

  foreign_window_manager_.reset();
  aura::Env::DeleteInstance();

  // The keyboard may have created a WebContents. The WebContents is destroyed
  // with the UI, and it needs the BrowserContext to be alive during its
  // destruction. So destroy all of the UI elements before destroying the
  // browser context.
  browser_context_.reset();

  ForeignWindowTextureFactory::Terminate();
}

bool ShellBrowserMainParts::MainMessageLoopRun(int* result_code) {
  MessageLoopForUI::current()->Run();
  return true;
}

}  // namespace shell
}  // namespace wm
