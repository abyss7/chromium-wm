// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WM_HOST_FOREIGN_TEST_WINDOW_HOST_X11_H_
#define WM_HOST_FOREIGN_TEST_WINDOW_HOST_X11_H_

#include <X11/Xlib.h>

// Get rid of a macro from Xlib.h that conflicts with Aura's RootWindow class.
#undef RootWindow

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/message_loop.h"
#include "ui/gfx/size.h"
#include "wm/host/foreign_test_window_host.h"

namespace wm {
class ForeignWindowManager;

class ForeignTestWindowHostX11 : public ForeignTestWindowHost,
                                 public MessageLoopForIO::Watcher {
 public:
  explicit ForeignTestWindowHostX11(ForeignWindowManager* window_manager);

  // This is guaranteed to run all tasks up to the last Sync() call.
  static void RunAllPendingInMessageLoop();

  // Overridden from ForeignTestWindowHost:
  virtual void Initialize() OVERRIDE;
  virtual void Delete() OVERRIDE;
  virtual void Show() OVERRIDE;
  virtual void Hide() OVERRIDE;
  virtual void Destroy() OVERRIDE;
  virtual void Sync() OVERRIDE;

  // Overridden from MessageLoopForIO::Watcher:
  virtual void OnFileCanReadWithoutBlocking(int fd) OVERRIDE;
  virtual void OnFileCanWriteWithoutBlocking(int fd) OVERRIDE;

 protected:
  virtual ~ForeignTestWindowHostX11();

 private:
  void ProcessXEvent(XEvent* event);
  void PumpXEvents();

  // The display and the native X window.
  Display* display_;
  ::Window parent_;
  ::Window window_;
  gfx::Size size_;
  XFontStruct* font_info_;
  ::GC gc_;

  MessageLoopForIO::FileDescriptorWatcher connection_watcher_;

  DISALLOW_COPY_AND_ASSIGN(ForeignTestWindowHostX11);
};

}  // namespace wm

#endif  // WM_HOST_FOREIGN_TEST_WINDOW_HOST_X11_H_
