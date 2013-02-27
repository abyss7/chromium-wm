// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WM_FOREIGN_WINDOW_HOST_LINUX_H_
#define WM_FOREIGN_WINDOW_HOST_LINUX_H_

#include <X11/Xlib.h>

// Get rid of a macro from Xlib.h that conflicts with Aura's RootWindow class.
#undef RootWindow

#include "base/message_loop.h"
#include "wm/host/foreign_window_host.h"

namespace wm {

class ForeignWindowHostLinux : public ForeignWindowHost,
                               public MessageLoop::Dispatcher {
 public:
  explicit ForeignWindowHostLinux(gfx::PluginWindowHandle window_handle);
  virtual ~ForeignWindowHostLinux();

  // Overridden from wm::ForeignWindowHost:
  virtual void SetDelegate(ForeignWindowHostDelegate* delegate) OVERRIDE;
  virtual gfx::PluginWindowHandle GetWindowHandle() const OVERRIDE;
  virtual void Close() OVERRIDE;
  virtual void OnWindowDestroyed() OVERRIDE;

  // Overridden from Dispatcher:
  virtual bool Dispatch(const base::NativeEvent& event) OVERRIDE;

 private:
  Display* xdisplay_;
  gfx::PluginWindowHandle window_handle_;
  ForeignWindowHostDelegate* delegate_;

  DISALLOW_COPY_AND_ASSIGN(ForeignWindowHostLinux);
};

}  // namespace wm

#endif  // WM_FOREIGN_WINDOW_HOST_LINUX_H_
