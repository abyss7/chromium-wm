// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/host/foreign_window_manager_host.h"

#include "ash/host/root_window_host_factory.h"

namespace wm {

namespace {

class ForeignWindowManagerHostImpl : public ForeignWindowManagerHost {
 public:
  ForeignWindowManagerHostImpl()
      : factory_(ash::RootWindowHostFactory::Create()) {}

  // Overridden from ForeignWindowManagerHost:
  virtual bool Initialize() OVERRIDE { return true; }
  virtual void InitializeForTesting() OVERRIDE {}
  virtual aura::RootWindowHost* CreateRootWindowHost(
      const gfx::Rect& initial_bounds) OVERRIDE {
    return factory_->CreateRootWindowHost(initial_bounds);
  }
  virtual gfx::AcceleratedWidget GetAcceleratedWidget() OVERRIDE {
    return gfx::kNullAcceleratedWidget;
  }
  virtual void SetDefaultCursor(gfx::NativeCursor cursor) OVERRIDE {}

 private:
  scoped_ptr<ash::RootWindowHostFactory> factory_;
};

}  // namespace

// static
ForeignWindowManagerHost* ForeignWindowManagerHost::Create() {
  return new ForeignWindowManagerHostImpl;
}

}  // namespace wm
