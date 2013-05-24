// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/test/wm_test_base.h"

#include "ash/ash_switches.h"
#include "base/command_line.h"
#include "ui/aura/root_window.h"
#include "wm/foreign_window_manager.h"
#include "wm/gpu/foreign_window_texture_factory.h"
#include "wm/test/test_shell_delegate.h"
#include "wm/test/wm_test_helper.h"

namespace wm {
namespace test {

WmTestBase::WmTestBase() {
  // Must initialize |wm_test_helper_| here because some tests rely on
  // WmTestBase methods before they call WmTestBase::SetUp().
  wm_test_helper_.reset(new WmTestHelper(&message_loop_));
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

  // Create the WM.
  ForeignWindowManager::CreateInstanceForTesting();

  wm_test_helper_->SetUp();

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

  wm_test_helper_->TearDown();

  // Destroy the WM.
  ForeignWindowManager::DeleteInstance();

  ForeignWindowTextureFactory::Terminate();
  content::ImageTransportFactory::Terminate();

  // Some tests set an internal display id,
  // reset it here, so other tests will continue in a clean environment.
  gfx::Display::SetInternalDisplayId(gfx::Display::kInvalidDisplayID);
}

void WmTestBase::RunAllPendingInMessageLoop() {
  wm_test_helper_->RunAllPendingInMessageLoop();
}

}  // namespace test
}  // namespace wm
