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

namespace aura {
class RootWindow;
}

namespace wm {
class ForeignWindowManagerHost;

class ForeignWindowManager {
 public:
  // Instantiate window manager. Returns false if initialization failed.
  static ForeignWindowManager* CreateInstance();

  // Instantiate window manager for testing. This always succeeds.
  static ForeignWindowManager* CreateInstanceForTesting();

  // Should never be called before |CreateInstance()|.
  static ForeignWindowManager* GetInstance();

  // Returns true if the window manager has been instantiated.
  static bool HasInstance();

  static void DeleteInstance();

  bool Initialize();

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

  // Create containers used for unmanaged windows.
  void CreateContainers(aura::RootWindow* root_window);

 private:
  ForeignWindowManager();
  virtual ~ForeignWindowManager();

  static ForeignWindowManager* instance_;

  scoped_ptr<ForeignWindowManagerHost> host_;
  scoped_ptr<ash::ImageCursors> image_cursors_;
  bool initialized_;

  DISALLOW_COPY_AND_ASSIGN(ForeignWindowManager);
};

}  // namespace wm

#endif  // WM_FOREIGN_WINDOW_MANAGER_H_
