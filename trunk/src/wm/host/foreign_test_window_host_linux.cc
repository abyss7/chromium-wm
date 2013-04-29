// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/host/foreign_test_window_host_linux.h"

#include "base/run_loop.h"
#include "ui/aura/env.h"
#include "wm/foreign_window_manager.h"

namespace wm {

ForeignTestWindowHostLinux::ForeignTestWindowHostLinux(
    ForeignWindowManager* window_manager)
    : display_(NULL),
      parent_(window_manager->GetAcceleratedWidget()),
      window_(0),
      font_info_(NULL),
      gc_(0) {
}

ForeignTestWindowHostLinux::~ForeignTestWindowHostLinux() {
}

void ForeignTestWindowHostLinux::Initialize() {
  DCHECK(parent_);
  display_ = XOpenDisplay(NULL);
  size_ = gfx::Size(300, 300);
  window_ = XCreateSimpleWindow(
      display_,
      parent_,
      30, 30, size_.width(), size_.height(), 0,
      BlackPixel(display_, DefaultScreen(display_)),
      WhitePixel(display_, DefaultScreen(display_)));
  XSelectInput(display_, window_, ExposureMask | StructureNotifyMask);
  font_info_ = XLoadQueryFont(display_, "9x15");
  DCHECK(font_info_);
  gc_ = XCreateGC(display_, window_, 0, NULL);
  XSetFont(display_, gc_, font_info_->fid);
  XSetForeground(display_, gc_, BlackPixel(display_, DefaultScreen(display_)));

  MessageLoopForIO::current()->WatchFileDescriptor(
      ConnectionNumber(display_), true, MessageLoopForIO::WATCH_READ,
      &connection_watcher_, this);

  PumpXEvents();
}

void ForeignTestWindowHostLinux::Delete() {
  connection_watcher_.StopWatchingFileDescriptor();
  XFreeGC(display_, gc_);
  XFreeFont(display_, font_info_);
  XCloseDisplay(display_);
  delete this;
}

void ForeignTestWindowHostLinux::Show() {
  DCHECK(window_);
  XMapWindow(display_, window_);
}

void ForeignTestWindowHostLinux::Hide() {
  DCHECK(window_);
  XUnmapWindow(display_, window_);
}

void ForeignTestWindowHostLinux::Destroy() {
  XDestroyWindow(display_, window_);
  window_ = 0;
}

void ForeignTestWindowHostLinux::Sync() {
  XSync(display_, False);
}

void ForeignTestWindowHostLinux::ProcessXEvent(XEvent *event) {
  switch (event->type) {
    case Expose: {
      if (event->xexpose.count != 0)
        break;

      std::string message("Hello, X Window System!");

      // Center text.
      int width = XTextWidth(font_info_, message.c_str(), message.length());
      int msg_x  = (size_.width() - width) / 2;

      int font_height = font_info_->ascent + font_info_->descent;
      int msg_y  = (size_.height() + font_height) / 2;

      XDrawString(display_,
                  window_,
                  gc_,
                  msg_x, msg_y,
                  message.c_str(), message.length());
    } break;
    case ConfigureNotify: {
      size_ = gfx::Size(event->xconfigure.width, event->xconfigure.height);
    } break;
  }
}

void ForeignTestWindowHostLinux::PumpXEvents() {
  while (XPending(display_)) {
    XEvent event;
    XNextEvent(display_, &event);
    ProcessXEvent(&event);
  }
}

void ForeignTestWindowHostLinux::OnFileCanReadWithoutBlocking(int fd) {
  PumpXEvents();
}

void ForeignTestWindowHostLinux::OnFileCanWriteWithoutBlocking(int fd) {
}

// static
ForeignTestWindowHost* ForeignTestWindowHost::Create(
    ForeignWindowManager* window_manager) {
  return new ForeignTestWindowHostLinux(window_manager);
}

// static
void ForeignTestWindowHost::RunAllPendingInMessageLoop() {
  Display* display = base::MessagePumpAuraX11::GetDefaultXDisplay();
  XSync(display, False);
  base::RunLoop run_loop(aura::Env::GetInstance()->GetDispatcher());
  run_loop.RunUntilIdle();
}

}  // namespace wm
