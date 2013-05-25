// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WM_HOST_FOREIGN_WINDOW_HOST_DELEGATE_H_
#define WM_HOST_FOREIGN_WINDOW_HOST_DELEGATE_H_

#include "ui/gfx/rect.h"

namespace wm {

// A private interface used by ForeignWindowHost implementations to communicate
// with their owning ForeignWindow.
class ForeignWindowHostDelegate {
 public:
  virtual void OnWindowContentsChanged(const gfx::Rect& area) = 0;

 protected:
  virtual ~ForeignWindowHostDelegate() {}
};

}  // namespace wm

#endif  // WM_HOST_FOREIGN_WINDOW_HOST_DELEGATE_H_
