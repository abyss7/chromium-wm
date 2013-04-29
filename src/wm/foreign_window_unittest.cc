// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/foreign_window.h"

#include "base/memory/ref_counted.h"
#include "ui/aura/root_window.h"
#include "ui/aura/window_tracker.h"
#include "wm/foreign_test_window.h"
#include "wm/test/wm_test_base.h"

namespace wm {
namespace test {

namespace {

// Add all windows with ForeignWindows to |window_tracker|.
void AddForeignWindowsToWindowTracker(
    aura::Window* window,
    aura::WindowTracker& window_tracker) {
  if (ForeignWindow::GetForeignWindowForNativeView(window))
    window_tracker.Add(window);
  for (size_t i = 0; i < window->children().size(); ++i)
    AddForeignWindowsToWindowTracker(window->children()[i], window_tracker);
}

}  // namespace

typedef wm::test::WmTestBase ForeignWindowTest;

// Test that aura windows are added and removed correctly when creating and
// destroying foreign windows.
TEST_F(ForeignWindowTest, AddRemove) {
  ForeignTestWindow::CreateParams params(foreign_window_manager());
  scoped_ptr<ForeignTestWindow> test_window1(new ForeignTestWindow(params));
  scoped_ptr<ForeignTestWindow> test_window2(new ForeignTestWindow(params));
  test_window1->Sync();
  test_window2->Sync();
  RunAllPendingInMessageLoop();
  aura::WindowTracker tracker;
  AddForeignWindowsToWindowTracker(
      ash::Shell::GetPrimaryRootWindow(), tracker);
  EXPECT_EQ(2u, tracker.windows().size());
  test_window1->Destroy();
  test_window1->Sync();
  RunAllPendingInMessageLoop();
  EXPECT_EQ(1u, tracker.windows().size());
  test_window2->Destroy();
  test_window2->Sync();
  RunAllPendingInMessageLoop();
  EXPECT_EQ(0u, tracker.windows().size());
}

// Test that aura window properties are updated correctly when foreign
// is mapped/unmapped.
TEST_F(ForeignWindowTest, IsVisible) {
  ForeignTestWindow::CreateParams params(foreign_window_manager());
  scoped_ptr<ForeignTestWindow> test_window(new ForeignTestWindow(params));
  test_window->Sync();
  RunAllPendingInMessageLoop();
  aura::WindowTracker tracker;
  AddForeignWindowsToWindowTracker(
      ash::Shell::GetPrimaryRootWindow(), tracker);
  EXPECT_EQ(1u, tracker.windows().size());
  EXPECT_FALSE((*tracker.windows().begin())->IsVisible());
  test_window->Show();
  test_window->Sync();
  RunAllPendingInMessageLoop();
  EXPECT_TRUE((*tracker.windows().begin())->IsVisible());
  test_window->Hide();
  test_window->Sync();
  RunAllPendingInMessageLoop();
  EXPECT_FALSE((*tracker.windows().begin())->IsVisible());
}

}  // namespace test
}  // namespace wm
