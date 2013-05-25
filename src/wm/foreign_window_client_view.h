// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WM_FOREIGN_WINDOW_CLIENT_VIEW_H_
#define WM_FOREIGN_WINDOW_CLIENT_VIEW_H_

#include <deque>

#include "ui/aura/window_observer.h"
#include "ui/compositor/compositor_observer.h"
#include "ui/views/window/client_view.h"
#include "wm/gpu/foreign_window_texture_factory.h"

namespace wm {

class ForeignWindowClientView
    : public views::ClientView,
      public aura::WindowObserver,
      public ui::CompositorObserver,
      public ForeignWindowTextureFactoryObserver,
      public base::SupportsWeakPtr<ForeignWindowClientView> {
 public:
  ForeignWindowClientView(gfx::PluginWindowHandle window_handle,
                          gfx::Size preferred_size);
  virtual ~ForeignWindowClientView();

  // Overridden from views::View:
  virtual gfx::Size GetPreferredSize() OVERRIDE;
  virtual gfx::Size GetMinimumSize() OVERRIDE;
  virtual void OnBoundsChanged(const gfx::Rect& previous_bounds) OVERRIDE;

  // Overridden from aura::WindowObserver:
  virtual void OnWindowRemovingFromRootWindow(aura::Window* window) OVERRIDE;

  // Overridden from ui::CompositorObserver:
  virtual void OnCompositingDidCommit(ui::Compositor* compositor) OVERRIDE;
  virtual void OnCompositingStarted(ui::Compositor* compositor,
                                    base::TimeTicks start_time) OVERRIDE;
  virtual void OnCompositingEnded(ui::Compositor* compositor) OVERRIDE;
  virtual void OnCompositingAborted(ui::Compositor* compositor) OVERRIDE;
  virtual void OnCompositingLockStateChanged(
      ui::Compositor* compositor) OVERRIDE;
  virtual void OnUpdateVSyncParameters(ui::Compositor* compositor,
                                       base::TimeTicks timebase,
                                       base::TimeDelta interval) OVERRIDE;

  // Overridden from wm::ForeignWindowTextureFactoryObserver:
  virtual void OnLostResources() OVERRIDE;

  // Returns the native view.
  gfx::NativeView GetNativeView() const;

  // Called when foreign window contents have changed.
  void OnWindowContentsChanged(const gfx::Rect& area);

  // Called when foreign window size has changed.
  void OnWindowSizeChanged(const gfx::Size& size);

  // Called when foreign window visibility has changed.
  void OnWindowVisibilityChanged(bool visible);

  // Called when foreign window has been destroyed.
  void OnWindowDestroyed();

 private:
  void CreateTexture();
  void OnTextureCreated(scoped_refptr<ForeignWindowTexture> texture);
  void AddOnCommitCallback(const base::Closure& callback);
  void RunOnCommitCallbacks();
  ui::Compositor* GetCompositor();

  scoped_ptr<views::View> contents_view_;
  scoped_ptr<aura::Window> window_;
  gfx::PluginWindowHandle window_handle_;
  gfx::Size preferred_size_;
  gfx::Size window_size_;
  bool window_visible_;
  bool window_destroyed_;
  scoped_refptr<ForeignWindowTexture> texture_;
  bool texture_is_valid_;
  int pending_texture_created_count_;
  std::deque<base::Closure> on_commit_callbacks_;

  DISALLOW_COPY_AND_ASSIGN(ForeignWindowClientView);
};

}  // namespace wm

#endif  // WM_FOREIGN_WINDOW_CLIENT_VIEW_H_
