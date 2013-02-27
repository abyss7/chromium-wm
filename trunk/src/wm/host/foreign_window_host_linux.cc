// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/host/foreign_window_host_linux.h"

#include <X11/extensions/Xdamage.h>

#include "wm/host/foreign_window_host_delegate.h"

namespace wm {

ForeignWindowHostLinux::ForeignWindowHostLinux(
    gfx::PluginWindowHandle window_handle) :
    xdisplay_(base::MessagePumpAuraX11::GetDefaultXDisplay()),
    window_handle_(window_handle),
    delegate_(NULL) {
  DCHECK(window_handle_);
  // Damage resource is automatically freed when the window is destroyed or
  // we close our connection to the X server.
  // TODO(reveman): Ignore possible X error.
  XDamageCreate(xdisplay_, window_handle_, XDamageReportRawRectangles);
  base::MessagePumpAuraX11::Current()->AddDispatcherForWindow(
      this, window_handle_);
}

ForeignWindowHostLinux::~ForeignWindowHostLinux() {
  if (window_handle_) {
    base::MessagePumpAuraX11::Current()->RemoveDispatcherForWindow(
        window_handle_);
  }
}

void ForeignWindowHostLinux::SetDelegate(ForeignWindowHostDelegate* delegate) {
  DCHECK(!delegate_);
  delegate_ = delegate;
}

gfx::PluginWindowHandle ForeignWindowHostLinux::GetWindowHandle() const {
  return window_handle_;
}

void ForeignWindowHostLinux::Close() {
  DCHECK(window_handle_);
  // TODO(reveman): Close managed window by sending a client message.
}

void ForeignWindowHostLinux::OnWindowDestroyed() {
  DCHECK(window_handle_);
  base::MessagePumpAuraX11::Current()->RemoveDispatcherForWindow(
      window_handle_);
  window_handle_ = 0;
}

bool ForeignWindowHostLinux::Dispatch(const base::NativeEvent& event) {
  DCHECK(window_handle_);
  // TODO(reveman): Only schedule repaint of the area that was damaged.
  delegate_->OnWindowContentsChanged();
  return true;
}

// static
ForeignWindowHost* ForeignWindowHost::Create(
    gfx::PluginWindowHandle window_handle) {
  return new ForeignWindowHostLinux(window_handle);
}

}  // namespace wm
