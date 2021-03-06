// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/foreign_window.h"

#include "cc/base/region.h"
#include "ui/aura/root_window.h"
#include "ui/aura/window_observer.h"
#include "ui/aura/window_tracker.h"
#include "ui/views/widget/widget.h"
#include "wm/foreign_test_window.h"
#include "wm/gpu/foreign_window_texture_factory.h"
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
  ForeignTestWindow::CreateParams params;
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
  ForeignTestWindow::CreateParams params;
  scoped_ptr<ForeignTestWindow> test_window(new ForeignTestWindow(params));
  test_window->Sync();
  RunAllPendingInMessageLoop();
  aura::WindowTracker tracker;
  AddForeignWindowsToWindowTracker(
      ash::Shell::GetPrimaryRootWindow(), tracker);
  ASSERT_EQ(1u, tracker.windows().size());
  aura::Window* window = *tracker.windows().begin();
  EXPECT_FALSE(window->IsVisible());
  test_window->Show();
  test_window->Sync();
  RunAllPendingInMessageLoop();
  EXPECT_TRUE(window->IsVisible());
  test_window->Hide();
  test_window->Sync();
  RunAllPendingInMessageLoop();
  EXPECT_FALSE(window->IsVisible());
}

// Test handling of unmanaged windows.
TEST_F(ForeignWindowTest, Unmanaged) {
  gfx::Rect initial_bounds(10, 10, 200, 50);
  ForeignTestWindow::CreateParams params;
  params.bounds = initial_bounds;
  params.managed = false;
  scoped_ptr<ForeignTestWindow> test_window(new ForeignTestWindow(params));
  test_window->Show();
  test_window->Sync();
  RunAllPendingInMessageLoop();
  aura::WindowTracker tracker;
  AddForeignWindowsToWindowTracker(
      ash::Shell::GetPrimaryRootWindow(), tracker);
  ASSERT_EQ(1u, tracker.windows().size());
  aura::Window* window = *tracker.windows().begin();
  EXPECT_TRUE(window->IsVisible());
  // Bounds should match initial bounds.
  EXPECT_EQ(initial_bounds.ToString(),
            window->GetBoundsInRootWindow().ToString());
  // Check if window movement is handled correctly.
  gfx::Point new_origin(100, 100);
  test_window->SetBounds(gfx::Rect(new_origin, initial_bounds.size()));
  test_window->Sync();
  RunAllPendingInMessageLoop();
  EXPECT_EQ(new_origin.ToString(),
            window->GetBoundsInRootWindow().origin().ToString());
  // Test simultaneous move and resize.
  gfx::Rect new_bounds(50, 50, 50, 50);
  test_window->SetBounds(new_bounds);
  test_window->Sync();
  RunAllPendingInMessageLoop();
  EXPECT_EQ(new_bounds.ToString(),
            window->GetBoundsInRootWindow().ToString());
  // Check if offscreen bounds are handled correctly.
  gfx::Rect offscreen_bounds(-100, -100, 75, 75);
  test_window->SetBounds(offscreen_bounds);
  test_window->Sync();
  RunAllPendingInMessageLoop();
  EXPECT_EQ(offscreen_bounds.ToString(),
            window->GetBoundsInRootWindow().ToString());
  // And partially offscreen bounds.
  gfx::Rect partial_offscreen_bounds(-25, -25, 75, 75);
  test_window->SetBounds(partial_offscreen_bounds);
  test_window->Sync();
  RunAllPendingInMessageLoop();
  EXPECT_EQ(partial_offscreen_bounds.ToString(),
            window->GetBoundsInRootWindow().ToString());
  test_window->Hide();
  test_window->Sync();
  RunAllPendingInMessageLoop();
  EXPECT_FALSE(window->IsVisible());
}

// Test that window size changes are propagated to foreign window correctly.
TEST_F(ForeignWindowTest, Resize) {
  ForeignTestWindow::CreateParams params;
  scoped_ptr<ForeignTestWindow> test_window(new ForeignTestWindow(params));
  test_window->Show();
  test_window->Sync();
  RunAllPendingInMessageLoop();
  aura::WindowTracker tracker;
  AddForeignWindowsToWindowTracker(
      ash::Shell::GetPrimaryRootWindow(), tracker);
  ASSERT_EQ(1u, tracker.windows().size());
  aura::Window* window = *tracker.windows().begin();
  views::Widget* widget =
      ForeignWindow::GetForeignWindowForNativeView(window)->GetWidget();
  EXPECT_TRUE(window->IsVisible());
  gfx::Rect initial_bounds(test_window->GetBounds());
  gfx::Rect new_bounds(initial_bounds);
  new_bounds.Inset(100, 100);
  widget->SetBounds(
      widget->non_client_view()->GetWindowBoundsForClientBounds(new_bounds));
  RunAllPendingInMessageLoop();
  EXPECT_EQ(new_bounds.ToString(), test_window->GetBounds().ToString());
  new_bounds.Inset(-50, -50);
  widget->SetBounds(
      widget->non_client_view()->GetWindowBoundsForClientBounds(new_bounds));
  RunAllPendingInMessageLoop();
  EXPECT_EQ(new_bounds.ToString(), test_window->GetBounds().ToString());
}

// Check if bounds change requests by foreign window are handled correctly.
TEST_F(ForeignWindowTest, RequestSetBounds) {
  ForeignTestWindow::CreateParams params;
  scoped_ptr<ForeignTestWindow> test_window(new ForeignTestWindow(params));
  test_window->Show();
  test_window->Sync();
  RunAllPendingInMessageLoop();
  aura::WindowTracker tracker;
  AddForeignWindowsToWindowTracker(
      ash::Shell::GetPrimaryRootWindow(), tracker);
  ASSERT_EQ(1u, tracker.windows().size());
  aura::Window* window = *tracker.windows().begin();
  EXPECT_TRUE(window->IsVisible());
  // Check if window movement requests are handled correctly.
  gfx::Rect initial_bounds(test_window->GetBounds());
  gfx::Point new_origin(100, 100);
  test_window->SetBounds(gfx::Rect(new_origin, initial_bounds.size()));
  test_window->Sync();
  RunAllPendingInMessageLoop();
  EXPECT_EQ(new_origin.ToString(),
            window->GetBoundsInRootWindow().origin().ToString());
  // Check if window resize requests are handled correctly.
  gfx::Size new_size(500, 500);
  test_window->SetBounds(gfx::Rect(new_origin, new_size));
  test_window->Sync();
  RunAllPendingInMessageLoop();
  EXPECT_EQ(new_size.ToString(),
            window->GetBoundsInRootWindow().size().ToString());
  // Check if window move+resize requests are handled correctly.
  gfx::Rect new_bounds(50, 50, 200, 200);
  test_window->SetBounds(new_bounds);
  test_window->Sync();
  RunAllPendingInMessageLoop();
  EXPECT_EQ(new_bounds.ToString(),
            window->GetBoundsInRootWindow().ToString());
  test_window->Hide();
  test_window->Sync();
  RunAllPendingInMessageLoop();
  EXPECT_FALSE(window->IsVisible());
}

// Test closing of foreign window.
TEST_F(ForeignWindowTest, CloseWindow) {
  ForeignTestWindow::CreateParams params;
  scoped_ptr<ForeignTestWindow> test_window(new ForeignTestWindow(params));
  test_window->Show();
  test_window->Sync();
  RunAllPendingInMessageLoop();
  aura::WindowTracker tracker;
  AddForeignWindowsToWindowTracker(
      ash::Shell::GetPrimaryRootWindow(), tracker);
  ASSERT_EQ(1u, tracker.windows().size());
  aura::Window* window = *tracker.windows().begin();
  EXPECT_TRUE(window->IsVisible());
  views::Widget* widget =
      ForeignWindow::GetForeignWindowForNativeView(window)->GetWidget();
  widget->Close();
  RunAllPendingInMessageLoop();
  test_window->Sync();
  RunAllPendingInMessageLoop();
  EXPECT_EQ(0u, tracker.windows().size());
}

class SchedulePaintTracker : public aura::WindowObserver {
 public:
  SchedulePaintTracker(aura::Window* window) : window_(window) {
    window_->AddObserver(this);
  }
  ~SchedulePaintTracker() {
    window_->RemoveObserver(this);
  }

  // Overridden from aura::WindowObserver:
  virtual void OnWindowPaintScheduled(aura::Window* window,
                                      const gfx::Rect& region) OVERRIDE {
    paint_scheduled_.Union(region);
  }

  const cc::Region& paint_scheduled() const { return paint_scheduled_; }

 private:
  aura::Window* window_;
  cc::Region paint_scheduled_;
};

// Test contents changes.
TEST_F(ForeignWindowTest, ContentsChanges) {
  ForeignTestWindow::CreateParams params;
  params.bounds = gfx::Rect(0, 0, 20, 20);
  scoped_ptr<ForeignTestWindow> test_window(new ForeignTestWindow(params));
  test_window->Show();
  test_window->Sync();
  RunAllPendingInMessageLoop();
  aura::WindowTracker tracker;
  AddForeignWindowsToWindowTracker(
      ash::Shell::GetPrimaryRootWindow(), tracker);
  ASSERT_EQ(1u, tracker.windows().size());
  aura::Window* window = *tracker.windows().begin();
  EXPECT_TRUE(window->IsVisible());
  // Change contents of top-left corner.
  SchedulePaintTracker small_repaint(window);
  gfx::Rect top_left(0, 0, 10, 10);
  test_window->ChangeContents(top_left);
  test_window->Sync();
  RunAllPendingInMessageLoop();
  EXPECT_TRUE(small_repaint.paint_scheduled().Contains(top_left));
  // Change contents of top-left corner and center.
  SchedulePaintTracker medium_repaint(window);
  gfx::Rect center(5, 5, 10, 10);
  test_window->ChangeContents(top_left);
  test_window->ChangeContents(center);
  test_window->Sync();
  RunAllPendingInMessageLoop();
  EXPECT_TRUE(medium_repaint.paint_scheduled().Contains(top_left));
  EXPECT_TRUE(medium_repaint.paint_scheduled().Contains(center));
  // Change contents of top-left corner, center and bottom-right corner.
  SchedulePaintTracker large_repaint(window);
  gfx::Rect bottom_right(10, 10, 10, 10);
  test_window->ChangeContents(top_left);
  test_window->ChangeContents(center);
  test_window->ChangeContents(bottom_right);
  test_window->Sync();
  RunAllPendingInMessageLoop();
  EXPECT_TRUE(large_repaint.paint_scheduled().Contains(top_left));
  EXPECT_TRUE(large_repaint.paint_scheduled().Contains(center));
  EXPECT_TRUE(large_repaint.paint_scheduled().Contains(bottom_right));
}

}  // namespace test
}  // namespace wm
