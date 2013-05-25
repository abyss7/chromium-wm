// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/host/foreign_test_window_host_x11.h"

#include "base/run_loop.h"
#include "ui/aura/env.h"
#include "ui/base/x/x11_atom_cache.h"
#include "wm/foreign_window_manager.h"

namespace wm {

namespace {

const char* kAtomsToCache[] = {
  "WM_DELETE_WINDOW",
  NULL
};

}  // namespace

ForeignTestWindowHostX11::ForeignTestWindowHostX11(
    ForeignWindowManager* window_manager,
    const gfx::Rect& bounds,
    bool managed)
    : display_(NULL),
      parent_(window_manager->GetAcceleratedWidget()),
      window_(0),
      bounds_(bounds),
      managed_(managed),
      font_info_(NULL),
      gc_(0) {
}

ForeignTestWindowHostX11::~ForeignTestWindowHostX11() {
}

void ForeignTestWindowHostX11::Initialize() {
  DCHECK(parent_);
  display_ = XOpenDisplay(NULL);
  atom_cache_.reset(new ui::X11AtomCache(display_, kAtomsToCache));
  XSetWindowAttributes swa;
  memset(&swa, 0, sizeof(swa));
  swa.background_pixel = WhitePixel(display_, DefaultScreen(display_));
  swa.override_redirect = !managed_;
  window_ = XCreateWindow(
      display_,
      parent_,
      bounds_.x(), bounds_.y(), bounds_.width(), bounds_.height(), 0,
      CopyFromParent,
      InputOutput,
      CopyFromParent,
      CWBackPixel | CWOverrideRedirect,
      &swa);
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

void ForeignTestWindowHostX11::Delete() {
  connection_watcher_.StopWatchingFileDescriptor();
  XFreeGC(display_, gc_);
  XFreeFont(display_, font_info_);
  XCloseDisplay(display_);
  delete this;
}

void ForeignTestWindowHostX11::Show() {
  DCHECK(window_);
  XMapWindow(display_, window_);
  XFlush(display_);
}

void ForeignTestWindowHostX11::Hide() {
  DCHECK(window_);
  XUnmapWindow(display_, window_);
  XFlush(display_);
}

void ForeignTestWindowHostX11::Destroy() {
  DCHECK(window_);
  XDestroyWindow(display_, window_);
  XFlush(display_);
  window_ = 0;
}

void ForeignTestWindowHostX11::Sync() {
  XSync(display_, False);
}

void ForeignTestWindowHostX11::SetBounds(const gfx::Rect& bounds) {
  XMoveResizeWindow(display_,
                    window_,
                    bounds.x(),
                    bounds.y(),
                    bounds.width(),
                    bounds.height());
  XFlush(display_);
}

void ForeignTestWindowHostX11::GetBounds(gfx::Rect* bounds) {
  XSync(display_, False);
  PumpXEvents();
  *bounds = bounds_;
}

void ForeignTestWindowHostX11::ChangeContents(const gfx::Rect& area) {
  XClearArea(display_,
             window_,
             area.x(),
             area.y(),
             area.width(),
             area.height(),
             true);
  XFlush(display_);
}

void ForeignTestWindowHostX11::AddOnDestroyCallback(
    const base::Closure& callback) {
  on_destroy_callbacks_.push_back(callback);
}

void ForeignTestWindowHostX11::ProcessXEvent(XEvent *event) {
  switch (event->type) {
    case ClientMessage: {
      Atom message_type = static_cast<Atom>(event->xclient.data.l[0]);
      if (message_type == atom_cache_->GetAtom("WM_DELETE_WINDOW"))
        Destroy();
      break;
    }
    case Expose: {
      std::string message("Hello, X Window System!");

      // Center text.
      int width = XTextWidth(font_info_, message.c_str(), message.length());
      int msg_x  = (bounds_.width() - width) / 2;

      int font_height = font_info_->ascent + font_info_->descent;
      int msg_y  = (bounds_.height() + font_height) / 2;

      XRectangle rectangle = { event->xexpose.x,
                               event->xexpose.y,
                               event->xexpose.width,
                               event->xexpose.height };
      XSetClipRectangles(display_, gc_, 0, 0, &rectangle, 1, YXBanded);
      XDrawString(display_,
                  window_,
                  gc_,
                  msg_x, msg_y,
                  message.c_str(), message.length());
      break;
    }
    case ConfigureNotify: {
      bounds_ = gfx::Rect(event->xconfigure.x,
                          event->xconfigure.y,
                          event->xconfigure.width,
                          event->xconfigure.height);
      break;
    }
    case DestroyNotify: {
      RunOnDestroyCallbacks();
      break;
    }
  }
}

void ForeignTestWindowHostX11::PumpXEvents() {
  while (XPending(display_)) {
    XEvent event;
    XNextEvent(display_, &event);
    ProcessXEvent(&event);
  }
  XFlush(display_);
}

void ForeignTestWindowHostX11::RunOnDestroyCallbacks() {
  while (!on_destroy_callbacks_.empty()) {
    on_destroy_callbacks_.front().Run();
    on_destroy_callbacks_.pop_front();
  }
}

void ForeignTestWindowHostX11::OnFileCanReadWithoutBlocking(int fd) {
  PumpXEvents();
}

void ForeignTestWindowHostX11::OnFileCanWriteWithoutBlocking(int fd) {
}

// static
void ForeignTestWindowHostX11::RunAllPendingInMessageLoop() {
  Display* display = base::MessagePumpAuraX11::GetDefaultXDisplay();
  XSync(display, False);
  base::RunLoop run_loop(aura::Env::GetInstance()->GetDispatcher());
  run_loop.RunUntilIdle();
}

}  // namespace wm
