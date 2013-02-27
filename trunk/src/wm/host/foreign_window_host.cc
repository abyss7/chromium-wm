// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/host/foreign_window_host.h"

#include "base/compiler_specific.h"

namespace wm {

namespace {

class ForeignWindowHostImpl : public ForeignWindowHost {
 public:
  explicit ForeignWindowHostImpl(gfx::PluginWindowHandle window_handle)
      : window_handle_(window_handle) {}

  // Overridden from wm::ForeignWindowHost:
  virtual void SetDelegate(ForeignWindowHostDelegate* delegate) OVERRIDE {}
  virtual gfx::PluginWindowHandle GetWindowHandle() const OVERRIDE {
    return window_handle_;
  }
  virtual void Close() OVERRIDE {}
  virtual void OnWindowDestroyed() OVERRIDE {}

 private:
  gfx::PluginWindowHandle window_handle_;
};

}  // namespace

// static
ForeignWindowHost* ForeignWindowHost::Create(
    gfx::PluginWindowHandle window_handle) {
  return new ForeignWindowHostImpl(window_handle);
}

}  // namespace wm
