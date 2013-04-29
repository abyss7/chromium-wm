// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/host/foreign_test_window_host_x11.h"

namespace wm {

// static
ForeignTestWindowHost* ForeignTestWindowHost::Create(
    ForeignWindowManager* window_manager) {
  return new ForeignTestWindowHostX11(window_manager);
}

// static
void ForeignTestWindowHost::RunAllPendingInMessageLoop() {
  ForeignTestWindowHostX11::RunAllPendingInMessageLoop();
}

}  // namespace wm
