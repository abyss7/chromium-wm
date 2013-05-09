// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/host/foreign_test_window_host.h"

#include "base/compiler_specific.h"

namespace wm {

namespace {

class ForeignTestWindowHostImpl : public ForeignTestWindowHost {
 public:
  ForeignTestWindowHostImpl() {}

  // Overridden from wm::ForeignTestWindowHost:
  virtual void Initialize() OVERRIDE {}
  virtual void Delete() OVERRIDE { delete this; }
  virtual void Show() OVERRIDE {}
  virtual void Hide() OVERRIDE {}
  virtual void Destroy() OVERRIDE {}
  virtual void Sync() OVERRIDE {}
  virtual void SetBounds(const gfx::Rect& bounds) OVERRIDE {}
};

}  // namespace

// static
ForeignTestWindowHost* ForeignTestWindowHost::Create(
    ForeignWindowManager* window_manager,
    const gfx::Rect& bounds,
    bool managed) {
  return new ForeignTestWindowHostImpl();
}

// static
void ForeignTestWindowHost::RunAllPendingInMessageLoop() {
}

}  // namespace wm
