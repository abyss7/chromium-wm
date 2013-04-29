// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WM_FOREIGN_WINDOW_MANAGER_H_
#define WM_FOREIGN_WINDOW_MANAGER_H_

#include "base/memory/scoped_ptr.h"
#include "ui/gfx/display.h"
#include "ui/gfx/native_widget_types.h"

namespace ash {
class ImageCursors;
class RootWindowHostFactory;
}

namespace wm {
class ForeignWindowManagerHost;

class ForeignWindowManager {
 public:
  ForeignWindowManager();
  virtual ~ForeignWindowManager();

  // Initialize window manager. Returns false if initialization failed.
  bool Initialize();

  // Initialize window manager for testing. This always succeeds.
  void InitializeForTesting();

  // Create factory that produce RootWindowHost instances that support
  // native window management.
  ash::RootWindowHostFactory* CreateRootWindowHostFactory();

  // Native window manager widget.
  gfx::AcceleratedWidget GetAcceleratedWidget();

  // Sets the display for the window manager.
  void SetDisplay(const gfx::Display& display);

  // Sets default cursor for foreign windows.
  void SetDefaultCursor(gfx::NativeCursor cursor);

 private:
  scoped_ptr<ForeignWindowManagerHost> host_;
  scoped_ptr<ash::ImageCursors> image_cursors_;
  bool initialized_;

  DISALLOW_COPY_AND_ASSIGN(ForeignWindowManager);
};

}  // namespace wm

#endif  // WM_FOREIGN_WINDOW_MANAGER_H_