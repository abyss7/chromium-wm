// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/host/foreign_window_host_x11.h"

namespace wm {

// static
ForeignWindowHost* ForeignWindowHost::Create(
    gfx::PluginWindowHandle window_handle) {
  return new ForeignWindowHostX11(window_handle);
}

}  // namespace wm
