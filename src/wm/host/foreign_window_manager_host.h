// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WM_HOST_FOREIGN_WINDOW_MANAGER_HOST_H_
#define WM_HOST_FOREIGN_WINDOW_MANAGER_HOST_H_

#include "ui/gfx/native_widget_types.h"
#include "ui/gfx/rect.h"

namespace aura {
class RootWindowHost;
}

namespace wm {

class ForeignWindowManagerHost {
 public:
  virtual ~ForeignWindowManagerHost() {}

  virtual bool Initialize() = 0;

  virtual void InitializeForTesting() = 0;

  virtual aura::RootWindowHost* CreateRootWindowHost(
      const gfx::Rect& initial_bounds) = 0;

  virtual gfx::AcceleratedWidget GetAcceleratedWidget() = 0;

  virtual void SetDefaultCursor(gfx::NativeCursor cursor) = 0;

  virtual void ShowForeignWindows() = 0;

  // Creates a new ForeignWindowManagerHost. The caller owns the returned
  // value.
  static ForeignWindowManagerHost* Create();

  // Set X11 error handlers.
  static void SetX11ErrorHandlers();
  static void UnsetX11ErrorHandlers();
};

}  // namespace wm

#endif  // WM_HOST_FOREIGN_WINDOW_MANAGER_HOST_H_
