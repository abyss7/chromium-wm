// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/host/root_window_host_factory.h"

#include "ui/aura/root_window_host.h"

namespace wm {

namespace {

class RootWindowHostFactoryImpl : public RootWindowHostFactory {
 public:
  RootWindowHostFactoryImpl() {
    impl_.reset(ash::RootWindowHostFactory::Create());
  }

  // Overridden from RootWindowHostFactory:
  virtual aura::RootWindowHost* CreateRootWindowHost(
      const gfx::Rect& initial_bounds) OVERRIDE {
    return impl_->CreateRootWindowHost(initial_bounds);
  }

  scoped_ptr<ash::RootWindowHostFactory> impl_;
};

}  // namespace

// static
RootWindowHostFactory* RootWindowHostFactory::Create() {
  return new RootWindowHostFactoryImpl;
}

}  // namespace wm
