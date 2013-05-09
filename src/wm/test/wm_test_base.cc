// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/test/wm_test_base.h"

#include "ash/ash_switches.h"
#include "base/command_line.h"
#include "base/run_loop.h"
#include "ui/aura/env.h"
#include "ui/aura/root_window.h"
#include "ui/base/ime/text_input_test_support.h"
#include "ui/compositor/layer_animator.h"
#include "ui/compositor/scoped_animation_duration_scale_mode.h"
#if defined(ENABLE_MESSAGE_CENTER)
#include "ui/message_center/message_center.h"
#endif
#include "wm/foreign_test_window.h"
#include "wm/foreign_window_manager.h"
#include "wm/gpu/foreign_window_texture_factory.h"
#include "wm/host/foreign_test_window_host.h"
#include "wm/test/test_shell_delegate.h"

namespace wm {
namespace test {

WmTestBase::WmTestBase() {
}

WmTestBase::~WmTestBase() {
}

void WmTestBase::SetUp() {
  content::ImageTransportFactory::Initialize();
  ForeignWindowTextureFactory::Initialize();

  // Use the origin (1,1) so that it doesn't over
  // lap with the native mouse cursor.
  CommandLine::ForCurrentProcess()->AppendSwitchASCII(
      ash::switches::kAshHostWindowBounds, "1+1-800x600");
  // Disable animations during tests.
  zero_duration_mode_.reset(new ui::ScopedAnimationDurationScaleMode(
      ui::ScopedAnimationDurationScaleMode::ZERO_DURATION));
  ui::TextInputTestSupport::Initialize();

  // Creates Shell and hook with Desktop.
  TestShellDelegate* delegate = new TestShellDelegate;

  // Create the WM.
  foreign_window_manager_.reset(new ForeignWindowManager);
  foreign_window_manager_->InitializeForTesting();
  delegate->SetForeignWindowManager(foreign_window_manager_.get());

#if defined(ENABLE_MESSAGE_CENTER)
  // Creates MessageCenter since g_browser_process is not created in WmTestBase
  // tests.
  message_center::MessageCenter::Initialize();
#endif

  ash::Shell::CreateInstance(delegate);

  foreign_window_manager_->CreateContainers(
      ash::Shell::GetPrimaryRootWindow());

  ash::Shell::GetPrimaryRootWindow()->Show();
  ash::Shell::GetPrimaryRootWindow()->ShowRootWindow();
  // Move the mouse cursor to far away so that native events doesn't
  // interfere test expectations.
  ash::Shell::GetPrimaryRootWindow()->MoveCursorTo(gfx::Point(-1000, -1000));
  ash::Shell::GetInstance()->cursor_manager()->EnableMouseEvents();
}

void WmTestBase::TearDown() {
  // Flush the message loop to finish pending release tasks.
  RunAllPendingInMessageLoop();

  // Tear down the shell.
  ash::Shell::DeleteInstance();

#if defined(ENABLE_MESSAGE_CENTER)
  // Remove global message center state.
  message_center::MessageCenter::Shutdown();
#endif

  // Destroy the WM.
  foreign_window_manager_.reset();

  aura::Env::DeleteInstance();
  ui::TextInputTestSupport::Shutdown();

  ForeignWindowTextureFactory::Terminate();
  content::ImageTransportFactory::Terminate();
}

void WmTestBase::RunAllPendingInMessageLoop() {
  DCHECK(MessageLoopForUI::current() == &message_loop_);
  ForeignTestWindowHost::RunAllPendingInMessageLoop();
}

}  // namespace test
}  // namespace wm
