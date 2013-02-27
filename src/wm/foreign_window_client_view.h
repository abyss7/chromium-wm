// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WM_FOREIGN_WINDOW_CLIENT_VIEW_H_
#define WM_FOREIGN_WINDOW_CLIENT_VIEW_H_

#include "ui/views/window/client_view.h"
#include "wm/gpu/foreign_window_texture_factory.h"

namespace wm {

class ForeignWindowClientView
    : public views::ClientView,
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

  // Overridden from wm::ForeignWindowTextureFactoryObserver:
  virtual void OnLostResources() OVERRIDE;

  // Returns the native view.
  gfx::NativeView GetNativeView() const;

  // Called when foreign window contents have changed.
  void OnWindowContentsChanged();

  // Called when foreign window size has changed.
  void OnWindowSizeChanged(const gfx::Size& size);

  // Called when foreign window visibility has changed.
  void OnWindowVisibilityChanged(bool visible);

  // Called when foreign window has been destroyed.
  void OnWindowDestroyed();

 private:
  void CreateTexture();
  void OnTextureCreated(scoped_refptr<ForeignWindowTexture> texture);

  scoped_ptr<views::View> contents_view_;
  scoped_ptr<aura::Window> window_;
  gfx::PluginWindowHandle window_handle_;
  gfx::Size preferred_size_;
  gfx::Size window_size_;
  bool window_visible_;
  bool window_destroyed_;
  scoped_refptr<ForeignWindowTexture> texture_;
  int pending_texture_created_count_;

  DISALLOW_COPY_AND_ASSIGN(ForeignWindowClientView);
};

}  // namespace wm

#endif  // WM_FOREIGN_WINDOW_CLIENT_VIEW_H_
