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
#include "ui/message_center/message_center.h"
#include "wm/foreign_test_window.h"
#include "wm/foreign_window_manager.h"
#include "wm/gpu/foreign_window_texture_factory.h"
#include "wm/host/foreign_test_window_host.h"
#include "wm/test/test_shell_delegate.h"

#if defined(OS_CHROMEOS)
#include "chromeos/audio/cras_audio_handler.h"
#include "chromeos/power/power_manager_handler.h"
#endif

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
  ForeignWindowManager::CreateInstanceForTesting();

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

  ForeignWindowManager::GetInstance()->CreateContainers(
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

  // The global message center state must be shutdown absent
  // g_browser_process.
  message_center::MessageCenter::Shutdown();

#if defined(OS_CHROMEOS)
  if (ash::switches::UseNewAudioHandler())
    chromeos::CrasAudioHandler::Shutdown();
  chromeos::PowerManagerHandler::Shutdown();
#endif

  // Destroy the WM.
  ForeignWindowManager::DeleteInstance();

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
