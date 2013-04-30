// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WM_TEST_WM_TEST_BASE_H_
#define WM_TEST_WM_TEST_BASE_H_

#include "ash/shell.h"
#include "base/compiler_specific.h"
#include "base/message_loop.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/aura/env_observer.h"

namespace ui {
class ScopedAnimationDurationScaleMode;
}

namespace wm {
class ForeignWindowManager;

namespace test {

class WmTestBase : public testing::Test {
 public:
  WmTestBase();
  virtual ~WmTestBase();

  MessageLoopForUI* message_loop() { return &message_loop_; }

  ForeignWindowManager* foreign_window_manager() {
    return foreign_window_manager_.get();
  }

  // Overridden from testing::Test:
  virtual void SetUp() OVERRIDE;
  virtual void TearDown() OVERRIDE;

 protected:
  void RunAllPendingInMessageLoop();

 private:
  MessageLoopForUI message_loop_;

  scoped_ptr<ForeignWindowManager> foreign_window_manager_;

  scoped_ptr<ui::ScopedAnimationDurationScaleMode> zero_duration_mode_;

  DISALLOW_COPY_AND_ASSIGN(WmTestBase);
};

}  // namespace test
}  // namespace wm

#endif  // WM_TEST_WM_TEST_BASE_H_