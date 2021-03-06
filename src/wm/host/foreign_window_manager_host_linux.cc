// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/host/foreign_window_manager_host_x11.h"

namespace wm {

// static
ForeignWindowManagerHost* ForeignWindowManagerHost::Create() {
  return new ForeignWindowManagerHostX11;
}

// static
void ForeignWindowManagerHost::SetX11ErrorHandlers() {
  ForeignWindowManagerHostX11::SetX11ErrorHandlers();
}

// static
void ForeignWindowManagerHost::UnsetX11ErrorHandlers() {
  ForeignWindowManagerHostX11::UnsetX11ErrorHandlers();
}

}  // namespace wm
