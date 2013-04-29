// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/foreign_window_widget.h"

#include "ash/shell.h"
#include "ash/wm/property_util.h"
#include "ui/aura/root_window.h"
#include "ui/aura/window.h"
#include "ui/gfx/canvas.h"
#include "ui/views/widget/widget.h"
#include "ui/views/widget/widget_delegate.h"
#include "wm/foreign_window.h"

namespace wm {

ForeignWindowWidget::ForeignWindowWidget(ForeignWindow* foreign_window)
    : foreign_window_(foreign_window) {
  views::Widget::InitParams params(views::Widget::InitParams::TYPE_WINDOW);
  params.delegate = foreign_window->CreateWidgetDelegate();
  params.context = ash::Shell::GetActiveRootWindow();
  Init(params);
  set_focus_on_creation(false);
  GetNativeView()->SetName("ForeignWindowWidget");
  SetPersistsAcrossAllWorkspaces(
      GetNativeView(),
      ash::WINDOW_PERSISTS_ACROSS_ALL_WORKSPACES_VALUE_YES);
}

ForeignWindowWidget::~ForeignWindowWidget() {
}

void ForeignWindowWidget::Close() {
  // Call base class Close() when native window has been destroyed.
  if (foreign_window_->HasBeenDestroyed()) {
    views::Widget::Close();
    return;
  }

  foreign_window_->Close();
}

// static
views::Widget* ForeignWindowWidget::CreateWindow(
    ForeignWindow* foreign_window) {
  return new ForeignWindowWidget(foreign_window);
}

}  // namespace wm
