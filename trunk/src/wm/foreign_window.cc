// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/foreign_window.h"

#include "ui/aura/window.h"
#include "ui/aura/window_property.h"
#include "ui/views/widget/widget.h"
#include "wm/foreign_window_client_view.h"
#include "wm/host/foreign_window_host.h"

namespace wm {

namespace {

DECLARE_WINDOW_PROPERTY_TYPE(ForeignWindow*)
DEFINE_LOCAL_WINDOW_PROPERTY_KEY(ForeignWindow*, kForeignWindowKey, NULL);

ForeignWindowHost* CreateHost(ForeignWindow* foreign_window,
                              gfx::PluginWindowHandle window_handle) {
  ForeignWindowHost* host = ForeignWindowHost::Create(window_handle);
  host->SetDelegate(foreign_window);
  return host;
}

}  // namespace

ForeignWindow::CreateParams::CreateParams(
    gfx::PluginWindowHandle a_window_handle,
    gfx::Size a_preferred_size)
    : window_handle(a_window_handle),
      preferred_size(a_preferred_size) {
}

ForeignWindow::ForeignWindow(const CreateParams& params)
    : ALLOW_THIS_IN_INITIALIZER_LIST(
        host_(CreateHost(this, params.window_handle))),
      preferred_size_(params.preferred_size),
      display_state_(DISPLAY_WITHDRAWN),
      destroyed_(false) {
}

ForeignWindow::~ForeignWindow() {
  if (!client_view_)
    return;

  client_view_->GetNativeView()->ClearProperty(kForeignWindowKey);
}

void ForeignWindow::OnWindowContentsChanged() {
  if (!client_view_)
    return;

  client_view_->OnWindowContentsChanged();
}

views::WidgetDelegate* ForeignWindow::CreateWidgetDelegate() {
  AddRef();  // Balanced in DeleteDelegate();
  return this;
}

views::ClientView* ForeignWindow::CreateClientView(views::Widget* widget) {
  DCHECK(!client_view_);
  ForeignWindowClientView* client_view = new ForeignWindowClientView(
      host_->GetWindowHandle(), preferred_size_);
  client_view->GetNativeView()->SetProperty(kForeignWindowKey, this);
  client_view->GetNativeView()->Show();
  widget->GetNativeView()->AddChild(client_view->GetNativeView());

  // Keep a weak reference to the client view.
  client_view_ = client_view->AsWeakPtr();

  return client_view;
}

views::Widget* ForeignWindow::GetWidget() {
  if (!client_view_)
    return NULL;

  return client_view_->GetWidget();
}

const views::Widget* ForeignWindow::GetWidget() const {
  if (!client_view_)
    return NULL;

  return client_view_->GetWidget();
}

void ForeignWindow::DeleteDelegate() {
  // The window has finished closing. Allow ourself to be deleted.
  Release();
}

bool ForeignWindow::CanResize() const {
  return false;
}

bool ForeignWindow::CanMaximize() const {
  return false;
}

bool ForeignWindow::CanActivate() const {
  return true;
}

void ForeignWindow::Close() {
  host_->Close();
}

gfx::PluginWindowHandle ForeignWindow::GetWindowHandle() const {
  return host_->GetWindowHandle();
}

gfx::NativeView ForeignWindow::GetNativeView() const {
  if (!client_view_)
    return NULL;

  return client_view_->GetNativeView();
}

void ForeignWindow::SetDisplayState(ForeignWindow::DisplayState state) {
  display_state_ = state;
}

ForeignWindow::DisplayState ForeignWindow::GetDisplayState() const {
  return display_state_;
}

bool ForeignWindow::IsManaged() const {
  return true;
}

bool ForeignWindow::HasBeenDestroyed() const {
  return destroyed_;
}

void ForeignWindow::OnWindowSizeChanged(const gfx::Size& size) {
  if (!client_view_)
    return;

  client_view_->OnWindowSizeChanged(size);
}

void ForeignWindow::OnWindowVisibilityChanged(bool visible) {
  if (!client_view_)
    return;

  client_view_->OnWindowVisibilityChanged(visible);
}

void ForeignWindow::OnWindowDestroyed() {
  DCHECK(!destroyed_);
  destroyed_ = true;

  if (!client_view_)
    return;

  client_view_->OnWindowDestroyed();
}

// static
ForeignWindow* ForeignWindow::GetForeignWindowForNativeView(
    gfx::NativeView native_view) {
  return native_view->GetProperty(kForeignWindowKey);
}

}  // namespace wm
