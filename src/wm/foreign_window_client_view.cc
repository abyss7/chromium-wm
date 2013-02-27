// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/foreign_window_client_view.h"

#include "base/bind.h"
#include "ui/aura/window.h"
#include "ui/aura/window_delegate.h"
#include "ui/base/hit_test.h"
#include "ui/gfx/canvas.h"
#include "ui/views/widget/widget.h"
#include "wm/gpu/foreign_window_texture_factory.h"

namespace {

class ContentsViewImpl : public views::View {
 public:
  ContentsViewImpl() {}
  virtual ~ContentsViewImpl() {}

  // Overridden from views::View:
  virtual void OnPaint(gfx::Canvas* canvas) OVERRIDE {
    canvas->FillRect(GetLocalBounds(), SK_ColorDKGRAY);
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(ContentsViewImpl);
};

class WindowDelegateImpl : public aura::WindowDelegate {
 public:
  WindowDelegateImpl() {}
  virtual ~WindowDelegateImpl() {}

  // Overridden from aura::WindowDelegate:
  virtual gfx::Size GetMinimumSize() const OVERRIDE {
    return gfx::Size();
  }
  virtual gfx::Size GetMaximumSize() const OVERRIDE {
    return gfx::Size();
  }
  virtual void OnBoundsChanged(const gfx::Rect& old_bounds,
                               const gfx::Rect& new_bounds) OVERRIDE {}
  virtual gfx::NativeCursor GetCursor(const gfx::Point& point) OVERRIDE {
    return gfx::kNullCursor;
  }
  virtual int GetNonClientComponent(const gfx::Point& point) const OVERRIDE {
    return HTCLIENT;
  }
  virtual bool ShouldDescendIntoChildForEventHandling(
      aura::Window* child,
      const gfx::Point& location) OVERRIDE {
    return false;
  }
  virtual bool CanFocus() OVERRIDE {
    return true;
  }
  virtual void OnCaptureLost() OVERRIDE {}
  virtual void OnPaint(gfx::Canvas* canvas) OVERRIDE {}
  virtual void OnDeviceScaleFactorChanged(float device_scale_factor) OVERRIDE {
  }
  virtual void OnWindowDestroying() OVERRIDE {}
  virtual void OnWindowDestroyed() OVERRIDE {
    delete this;
  }
  virtual void OnWindowTargetVisibilityChanged(bool visible) OVERRIDE {}
  virtual bool HasHitTestMask() const OVERRIDE {
    return false;
  }
  virtual void GetHitTestMask(gfx::Path* mask) const OVERRIDE {}
  virtual scoped_refptr<ui::Texture> CopyTexture() OVERRIDE {
    NOTIMPLEMENTED();
    return scoped_refptr<ui::Texture>();
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(WindowDelegateImpl);
};

}  // namespace

namespace wm {

ForeignWindowClientView::ForeignWindowClientView(
    gfx::PluginWindowHandle window_handle,
    gfx::Size preferred_size)
    : views::ClientView(NULL, NULL),
      window_handle_(window_handle),
      preferred_size_(preferred_size),
      window_visible_(false),
      window_destroyed_(false),
      pending_texture_created_count_(0) {
  ForeignWindowTextureFactory::GetInstance()->AddObserver(this);
  contents_view_.reset(new ContentsViewImpl);
  set_contents_view(contents_view_.get());
  window_.reset(new aura::Window(new WindowDelegateImpl));
  window_->set_owned_by_parent(false);
  window_->SetType(aura::client::WINDOW_TYPE_CONTROL);
  window_->SetTransparent(false);
  window_->Init(ui::LAYER_TEXTURED);
  window_->layer()->SetMasksToBounds(true);
  window_->SetName("ForeignWindowClientView");
}

ForeignWindowClientView::~ForeignWindowClientView() {
  ForeignWindowTextureFactory::GetInstance()->RemoveObserver(this);
}

gfx::Size ForeignWindowClientView::GetPreferredSize() {
  return preferred_size_;
}

gfx::Size ForeignWindowClientView::GetMinimumSize() {
  return gfx::Size(1, 1);
}

void ForeignWindowClientView::OnBoundsChanged(
    const gfx::Rect& previous_bounds) {
  window_->SetBounds(bounds());
}

void ForeignWindowClientView::OnLostResources() {
  // ForeignWindowTextureFactory guarantees that it's safe to keep the
  // existing |texture_| reference around until we have a new texture. By
  // not resetting |texture_| here we verify this behavior. The compositor
  // however doesn't like using this texture so we reset the external
  // texture here to make sure that doesn't happen.
  window_->SetExternalTexture(NULL);

  if (window_visible_)
    CreateTexture();
}

gfx::NativeView ForeignWindowClientView::GetNativeView() const {
  return window_.get();
}

void ForeignWindowClientView::OnWindowContentsChanged() {
  if (pending_texture_created_count_)
    return;

  if (texture_) {
    texture_->OnContentsChanged();

    GetNativeView()->SchedulePaintInRect(
        gfx::Rect(GetNativeView()->bounds().size()));
  } else {
    CreateTexture();
  }
}

void ForeignWindowClientView::OnWindowSizeChanged(const gfx::Size& size) {
  if (window_size_ == size)
    return;

  window_size_ = size;

  if (window_visible_)
    CreateTexture();
}

void ForeignWindowClientView::OnWindowVisibilityChanged(bool visible) {
  window_visible_ = visible;

  // Drop |texture_| reference when invisible to make sure texture is
  // destroyed when no longer used by the compositor. Don't reset external
  // texture for |window_| as that would prevent the compositor from
  // drawing using this texture.
  if (!visible)
    texture_ = NULL;
}

void ForeignWindowClientView::OnWindowDestroyed() {
  window_destroyed_ = true;
}

void ForeignWindowClientView::CreateTexture() {
  ++pending_texture_created_count_;

  DCHECK(!window_destroyed_);
  ForeignWindowTextureFactory* factory =
      ForeignWindowTextureFactory::GetInstance();
  factory->CreateTextureForForeignWindow(
      window_handle_,
      false,
      1.0,
      base::Bind(&ForeignWindowClientView::OnTextureCreated, AsWeakPtr()));
}

void ForeignWindowClientView::OnTextureCreated(
    scoped_refptr<ForeignWindowTexture> texture) {
  --pending_texture_created_count_;

  if (!texture)
    return;

  texture_ = texture;
  window_->SetExternalTexture(texture);
}

}  // namespace wm
