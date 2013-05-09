// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WM_FOREIGN_TEST_WINDOW_H_
#define WM_FOREIGN_TEST_WINDOW_H_

#include "base/basictypes.h"
#include "base/callback.h"
#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "ui/gfx/rect.h"

namespace wm {
class ForeignTestWindowHost;
class ForeignWindowManager;

// This class implements a foreign window to use for testing purposes.
class ForeignTestWindow {
 public:
  struct CreateParams {
    explicit CreateParams(ForeignWindowManager* window_manager);

    ForeignWindowManager* window_manager;
    gfx::Rect bounds;
    bool managed;
  };
  explicit ForeignTestWindow(const CreateParams& params);
  virtual ~ForeignTestWindow();

  void Destroy();
  void Hide();
  void SetBounds(const gfx::Rect& bounds);
  void Show();
  void Sync();

 private:
  ForeignTestWindowHost* host_;

  DISALLOW_COPY_AND_ASSIGN(ForeignTestWindow);
};

}  // namespace wm

#endif  // WM_FOREIGN_TEST_WINDOW_H_
