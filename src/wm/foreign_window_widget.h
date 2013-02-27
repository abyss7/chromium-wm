// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WM_FOREIGN_WINDOW_WIDGET_H_
#define WM_FOREIGN_WINDOW_WIDGET_H_

#include "ui/views/widget/widget.h"

namespace wm {
class ForeignWindow;

class ForeignWindowWidget : public views::Widget {
 public:
  static views::Widget* CreateWindow(ForeignWindow* foreign_window);

  virtual void Close() OVERRIDE;

 private:
  explicit ForeignWindowWidget(ForeignWindow* foreign_window);
  virtual ~ForeignWindowWidget();

  scoped_refptr<ForeignWindow> foreign_window_;

  DISALLOW_COPY_AND_ASSIGN(ForeignWindowWidget);
};

}  // namespace wm

#endif  // WM_FOREIGN_WINDOW_WIDGET_H_
