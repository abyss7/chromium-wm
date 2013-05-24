// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ash/shell.h"
#include "ui/aura/root_window.h"
#include "wm/foreign_test_window.h"

namespace wm {
namespace shell {

namespace  {

// Default window size.
const int kWindowWidth = 300;
const int kWindowHeight = 200;

}  // namespace

void CreateUnmanagedWindow() {
  ForeignTestWindow::CreateParams params;
  gfx::Size size(kWindowWidth, kWindowHeight);
  gfx::Size root_window_size =
      ash::Shell::GetPrimaryRootWindow()->bounds().size();
  params.bounds = gfx::Rect((root_window_size.width() - size.width()) / 2,
                            (root_window_size.height() - size.height()) / 2,
                            size.width(), size.height());
  params.managed = false;
  ForeignTestWindow::Create(params);
}

}  // namespace shell
}  // namespace wm
