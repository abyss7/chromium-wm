// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/host/foreign_window_manager_host_x11.h"

namespace wm {

// static
ForeignWindowManagerHost* ForeignWindowManagerHost::Create() {
  return new ForeignWindowManagerHostX11;
}

}  // namespace wm
