// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/host/foreign_window_host_x11.h"

#include <X11/extensions/Xdamage.h>

#include "wm/host/foreign_window_host_delegate.h"
#include "wm/host/foreign_window_manager_host_x11.h"

namespace wm {

namespace {

const char* kAtomsToCache[] = {
  "WM_DELETE_WINDOW",
  "WM_PROTOCOLS",
  NULL
};

}  // namespace

ForeignWindowHostX11::ForeignWindowHostX11(
    gfx::PluginWindowHandle window_handle)
    : xdisplay_(base::MessagePumpAuraX11::GetDefaultXDisplay()),
      window_handle_(window_handle),
      delegate_(NULL),
      atom_cache_(xdisplay_, kAtomsToCache) {
  DCHECK(window_handle_);
  // Damage resource is automatically freed when the window is destroyed or
  // we close our connection to the X server.
  ForeignWindowManagerHostX11::IgnoreX11Error(NextRequest(xdisplay_));
  XDamageCreate(xdisplay_, window_handle_, XDamageReportRawRectangles);
  base::MessagePumpAuraX11::Current()->AddDispatcherForWindow(
      this, window_handle_);
}

ForeignWindowHostX11::~ForeignWindowHostX11() {
  if (window_handle_) {
    base::MessagePumpAuraX11::Current()->RemoveDispatcherForWindow(
        window_handle_);
  }
}

void ForeignWindowHostX11::SetDelegate(ForeignWindowHostDelegate* delegate) {
  DCHECK(!delegate_);
  delegate_ = delegate;
}

gfx::PluginWindowHandle ForeignWindowHostX11::GetWindowHandle() const {
  return window_handle_;
}

void ForeignWindowHostX11::Close() {
  DCHECK(window_handle_);

  XEvent event;
  event.xclient.type = ClientMessage;
  event.xclient.window = window_handle_;
  event.xclient.message_type = atom_cache_.GetAtom("WM_PROTOCOLS");
  event.xclient.format = 32;
  event.xclient.data.l[0] = atom_cache_.GetAtom("WM_DELETE_WINDOW");
  event.xclient.data.l[1] = 0;
  event.xclient.data.l[2] = 0;
  event.xclient.data.l[3] = 0;
  event.xclient.data.l[5] = 0;

  ForeignWindowManagerHostX11::IgnoreX11Error(NextRequest(xdisplay_));
  XSendEvent(xdisplay_, window_handle_, false, 0, &event);
}

void ForeignWindowHostX11::OnWindowDestroyed() {
  DCHECK(window_handle_);
  base::MessagePumpAuraX11::Current()->RemoveDispatcherForWindow(
      window_handle_);
  window_handle_ = 0;
}

bool ForeignWindowHostX11::Dispatch(const base::NativeEvent& event) {
  DCHECK(window_handle_);
  // TODO(reveman): Only schedule repaint of the area that was damaged.
  delegate_->OnWindowContentsChanged();
  return true;
}

}  // namespace wm
