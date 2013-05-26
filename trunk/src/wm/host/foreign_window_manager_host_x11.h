// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WM_HOST_FOREIGN_WINDOW_MANAGER_HOST_X11_H_
#define WM_HOST_FOREIGN_WINDOW_MANAGER_HOST_X11_H_

#include <X11/Xlib.h>

#include <map>

// Get rid of a macro from Xlib.h that conflicts with Aura's RootWindow class.
#undef RootWindow

#include "base/hash_tables.h"
#include "base/memory/weak_ptr.h"
#include "base/message_loop.h"
#include "ui/aura/env_observer.h"
#include "ui/aura/window_observer.h"
#include "ui/base/x/x11_atom_cache.h"
#include "wm/host/foreign_window_manager_host.h"

namespace wm {
class ForeignWindow;

class ForeignWindowManagerHostX11
    : public ForeignWindowManagerHost,
      public aura::EnvObserver,
      public aura::WindowObserver,
      public MessageLoop::Dispatcher,
      public base::SupportsWeakPtr<ForeignWindowManagerHostX11> {
 public:
  ForeignWindowManagerHostX11();
  virtual ~ForeignWindowManagerHostX11();

  // Set X11 error handlers.
  static void SetX11ErrorHandlers();
  static void UnsetX11ErrorHandlers();

  // Ignore any X11 errors from this sequence.
  static void IgnoreX11Error(unsigned long sequence);

  // Overridden from ForeignWindowManagerHost:
  virtual bool Initialize() OVERRIDE;
  virtual void InitializeForTesting() OVERRIDE;
  virtual aura::RootWindowHost* CreateRootWindowHost(
      const gfx::Rect& initial_bounds) OVERRIDE;
  virtual gfx::AcceleratedWidget GetAcceleratedWidget() OVERRIDE;
  virtual void SetDefaultCursor(gfx::NativeCursor cursor) OVERRIDE;
  virtual void ShowForeignWindows() OVERRIDE;

  // Overridden from MessageLoop::Dispatcher:
  virtual bool Dispatch(const base::NativeEvent& event) OVERRIDE;

  // Overridden from aura::EnvObserver:
  virtual void OnWindowInitialized(aura::Window* window) OVERRIDE;
  virtual void OnRootWindowInitialized(aura::RootWindow* root_window) OVERRIDE;

    // Overridden from aura::WindowObserver:
  virtual void OnWindowStackingChanged(aura::Window* window) OVERRIDE;
  virtual void OnWindowVisibilityChanged(
      aura::Window* window, bool visible) OVERRIDE;
  virtual void OnWindowBoundsChanged(aura::Window* window,
                                     const gfx::Rect& old_bounds,
                                     const gfx::Rect& new_bounds) OVERRIDE;
  virtual void OnWindowDestroyed(aura::Window* window) OVERRIDE;

 private:
  void AddDisplayFromSpec(const std::string& spec);

  ::Window GetTopForeignWindow(const aura::Window* window);
  ::Window FindForeignWindowToStackAbove(const aura::Window* window);
  void MapWindowIfNeeded(ForeignWindow* window);
  void UnmapWindowIfNeeded(ForeignWindow* window);

  void RecursiveConfigure(
      aura::Window* window,
      gfx::Vector2d offset,
      ::Window* sibling_to_stack_above,
      SkRegion* input_region);

  // Configure native windows.
  void Configure();

  // Schedule deferred configure of native windows.
  void ScheduleConfigure(aura::Window* window);

  Display* xdisplay_;
  ::Window xwindow_;

  // The native root window.
  ::Window x_root_window_;

  // The bounds of |xwindow_|.
  gfx::Rect bounds_;

  typedef std::map< ::Window, XWindowAttributes> WindowAttributesMap;
  WindowAttributesMap initial_window_attributes_;

  typedef base::hash_map< ::Window, scoped_refptr<ForeignWindow> >
      ForeignWindowMap;
  ForeignWindowMap foreign_windows_;

  typedef std::map< ::Window, aura::Window*> WindowMap;
  WindowMap root_windows_;

  bool configure_pending_;
  WindowMap configure_windows_;

  ui::X11AtomCache atom_cache_;

  DISALLOW_COPY_AND_ASSIGN(ForeignWindowManagerHostX11);
};

}  // namespace wm

#endif  // WM_HOST_FOREIGN_WINDOW_MANAGER_HOST_X11_H_
