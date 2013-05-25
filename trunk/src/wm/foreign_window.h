// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WM_FOREIGN_WINDOW_H_
#define WM_FOREIGN_WINDOW_H_

#include "base/memory/ref_counted.h"
#include "ui/views/widget/widget_delegate.h"
#include "wm/host/foreign_window_host_delegate.h"

namespace views {
class ClientView;
}

namespace wm {
class ForeignWindowClientView;
class ForeignWindowHost;

class ForeignWindow : public base::RefCounted<ForeignWindow>,
                      public ForeignWindowHostDelegate,
                      public views::WidgetDelegate {
 public:
  enum DisplayState {
    DISPLAY_NORMAL,
    DISPLAY_ICONIC,
    DISPLAY_WITHDRAWN
  };
  struct CreateParams {
    CreateParams(gfx::PluginWindowHandle window_handle,
                 gfx::Size preferred_size);

    gfx::PluginWindowHandle window_handle;
    gfx::Size preferred_size;
    bool managed;
  };
  explicit ForeignWindow(const CreateParams& params);

  // Retrieves the ForeignWindow implementation associated with the
  // given NativeView, or NULL if the supplied handle has no associated
  // ForeignWindow.
  static ForeignWindow* GetForeignWindowForNativeView(
      gfx::NativeView native_view);

  // Overridden from wm::ForeignWindowHostDelegate:
  virtual void OnWindowContentsChanged(const gfx::Rect& area) OVERRIDE;

  // Overridden from views::WidgetDelegate:
  virtual views::Widget* GetWidget() OVERRIDE;
  virtual const views::Widget* GetWidget() const OVERRIDE;
  virtual views::ClientView* CreateClientView(views::Widget* widget) OVERRIDE;
  virtual views::NonClientFrameView* CreateNonClientFrameView(
      views::Widget* widget) OVERRIDE;
  virtual void DeleteDelegate() OVERRIDE;
  virtual bool CanResize() const OVERRIDE;
  virtual bool CanMaximize() const OVERRIDE;
  virtual bool CanActivate() const OVERRIDE;

  // Called by widget. Attempts to close the foreign window.
  void Close();

  // Create a new views::WidgetDelegate.
  views::WidgetDelegate* CreateWidgetDelegate();

  // Returns the window handle.
  gfx::PluginWindowHandle GetWindowHandle() const;

  // Returns the native view.
  gfx::NativeView GetNativeView() const;

  // Set foreign window display state.
  void SetDisplayState(DisplayState state);
  DisplayState GetDisplayState() const;

  // Returns true if foreign window is managed.
  bool IsManaged() const;

  // Returns true if foreign window handle has been destroyed.
  bool HasBeenDestroyed() const;

  // Called when foreign window size has changed.
  void OnWindowSizeChanged(const gfx::Size& size);

  // Called when foreign window visibility has changed.
  void OnWindowVisibilityChanged(bool visible);

  // Called when foreign window has been destroyed.
  void OnWindowDestroyed();

 private:
  friend class base::RefCounted<ForeignWindow>;
  virtual ~ForeignWindow();

  scoped_ptr<ForeignWindowHost> host_;
  gfx::Size preferred_size_;
  bool managed_;
  base::WeakPtr<ForeignWindowClientView> client_view_;
  DisplayState display_state_;
  bool destroyed_;

  DISALLOW_COPY_AND_ASSIGN(ForeignWindow);
};

}  // namespace wm

#endif  // WM_FOREIGN_WINDOW_H_
