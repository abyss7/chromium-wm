// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WM_HOST_FOREIGN_WINDOW_HOST_H_
#define WM_HOST_FOREIGN_WINDOW_HOST_H_

#include "ui/gfx/native_widget_types.h"

namespace wm {
class ForeignWindowHostDelegate;

class ForeignWindowHost {
 public:
  virtual ~ForeignWindowHost() {}

  // Creates a new ForeignWindowHost. The caller owns the returned value.
  static ForeignWindowHost* Create(gfx::PluginWindowHandle window_handle);

  // Sets the delegate, which is normally done by the foreign window.
  virtual void SetDelegate(ForeignWindowHostDelegate* delegate) = 0;

  // Get window handle for foreign window.
  virtual gfx::PluginWindowHandle GetWindowHandle() const = 0;

  // Attempts to close the foreign window.
  virtual void Close() = 0;

  // Called when foreign window has been destroyed.
  virtual void OnWindowDestroyed() = 0;
};

}  // namespace wm

#endif  // WM_HOST_FOREIGN_WINDOW_HOST_H_
