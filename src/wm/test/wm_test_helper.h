// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WM_TEST_WM_TEST_HELPER_H_
#define WM_TEST_WM_TEST_HELPER_H_

#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"

namespace aura {
class RootWindow;
}  // namespace aura

namespace base {
class MessageLoopForUI;
}  // namespace base

namespace ui {
class ScopedAnimationDurationScaleMode;
}  // namespace ui

namespace wm {
namespace test {

class TestShellDelegate;

// A helper class that does common initialization required for Ash. Creates a
// root window and an ash::Shell instance with a test delegate.
class WmTestHelper {
 public:
  explicit WmTestHelper(base::MessageLoopForUI* message_loop);
  ~WmTestHelper();

  // Creates the ash::Shell and performs associated initialization.
  void SetUp();

  // Destroys the ash::Shell and performs associated cleanup.
  void TearDown();

  // Returns a RootWindow. Usually this is the active RootWindow, but that
  // method can return NULL sometimes, and in those cases, we fall back on the
  // primary RootWindow.
  aura::RootWindow* CurrentContext();

  void RunAllPendingInMessageLoop();

  base::MessageLoopForUI* message_loop() { return message_loop_; }
  TestShellDelegate* test_shell_delegate() { return test_shell_delegate_; }

 private:
  base::MessageLoopForUI* message_loop_;  // Not owned.
  TestShellDelegate* test_shell_delegate_;  // Owned by ash::Shell.
  scoped_ptr<ui::ScopedAnimationDurationScaleMode> zero_duration_mode_;

  DISALLOW_COPY_AND_ASSIGN(WmTestHelper);
};

}  // namespace test
}  // namespace wm

#endif  // WM_TEST_WM_TEST_HELPER_H_
