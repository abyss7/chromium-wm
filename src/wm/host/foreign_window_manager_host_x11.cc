// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/host/foreign_window_manager_host_x11.h"

#include <X11/Xatom.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/shape.h>

#include <queue>
#include <string>
#include <vector>

#include "ash/ash_switches.h"
#include "ash/display/display_info.h"
#include "ash/shell.h"
#include "ash/shell_delegate.h"
#include "base/bind.h"
#include "base/command_line.h"
#include "base/lazy_instance.h"
#include "base/strings/string_split.h"
#include "ui/aura/env.h"
#include "ui/aura/root_window.h"
#include "ui/aura/root_window_host_x11.h"
#include "wm/foreign_window.h"
#include "wm/foreign_window_widget.h"

namespace wm {

namespace {

const char* kAtomsToCache[] = {
  "WM_DELETE_WINDOW",
  "WM_S0",
  "_NET_WM_PID",
  "_NET_WM_PING",
  NULL
};

class RootWindowHostImpl : public aura::RootWindowHostX11 {
 public:
  explicit RootWindowHostImpl(const gfx::Rect& bounds)
      : RootWindowHostX11(bounds) {}
  virtual ~RootWindowHostImpl() {}

 private:
  DISALLOW_COPY_AND_ASSIGN(RootWindowHostImpl);
};

typedef int(*X11ErrorHandler)(Display*, XErrorEvent*);
typedef int(*X11IOErrorHandler)(Display*);

X11ErrorHandler g_base_x11_error_handler = NULL;
X11IOErrorHandler g_base_x11_io_error_handler = NULL;

typedef std::queue<unsigned long> SequenceQueue;
base::LazyInstance<SequenceQueue> g_x11_errors_to_ignore_;

void DiscardXErrorsToIgnore(unsigned long serial) {
  SequenceQueue* sequences = g_x11_errors_to_ignore_.Pointer();

  // Discard passed sequences.
  while (!sequences->empty()) {
    if (sequences->front() >= serial)
      break;
    sequences->pop();
  }
}

bool ShouldIgnoreX11Error(XErrorEvent* error) {
  DiscardXErrorsToIgnore(error->serial);
  SequenceQueue* sequences = g_x11_errors_to_ignore_.Pointer();
  return !sequences->empty() && sequences->front() == error->serial;
}

int X11ErrorHandlerImpl(Display* display, XErrorEvent* error) {
  if (ShouldIgnoreX11Error(error))
    return 0;
  DCHECK(g_base_x11_error_handler);
  return (*g_base_x11_error_handler)(display, error);
}

int X11IOErrorHandlerImpl(Display* display) {
  DCHECK(g_base_x11_io_error_handler);
  return (*g_base_x11_io_error_handler)(display);
}

unsigned InitWindowChanges(const gfx::Rect& bounds,
                           ::Window sibling_to_stack_above,
                           XWindowChanges& wc) {
  wc.x = bounds.x();
  wc.y = bounds.y();
  wc.width = bounds.width();
  wc.height = bounds.height();
  if (!sibling_to_stack_above) {
    wc.stack_mode = Below;
    return CWX | CWY | CWWidth | CWHeight | CWStackMode;
  }

  wc.sibling = sibling_to_stack_above;
  wc.stack_mode = Above;
  return CWX | CWY | CWWidth | CWHeight | CWStackMode | CWSibling;
}

aura::Window* FindLowestCommonAncestor(
    aura::Window* root, const aura::Window* p, const aura::Window* q) {
  // Root is the LCA.
  if (root == p || root == q)
    return root;

  aura::Window* prev = NULL;
  const aura::Window::Windows& children = root->children();
  for (size_t i = 0; i < children.size(); ++i) {
    // Try to find LCA of p and q in subtree.
    aura::Window* next = FindLowestCommonAncestor(children[i], p, q);
    if (next) {
      // If a LCA was previously found, p and q must be in different subtrees.
      if (prev)
        return root;

      prev = next;
    }
  }

  return prev;
}

gfx::Vector2d GetTargetOffsetInRootWindow(const aura::Window* window) {
  gfx::Vector2d offset;

  const aura::Window* p = window;
  for (; p != window->GetRootWindow(); p = p->parent())
    offset += p->GetTargetBounds().OffsetFromOrigin();

  return offset;
}

void GetWindowAttributesForAllChildren(
    Display* display,
    ::Window window,
    std::map< ::Window, XWindowAttributes>* window_attributes) {
  ::Window root;
  ::Window parent;
  ::Window* children = NULL;
  unsigned num_children = 0;
  if (!XQueryTree(display, window, &root, &parent, &children, &num_children))
    return;

  for (unsigned i = 0; i < num_children; ++i) {
    XWindowAttributes attributes;
    if (!XGetWindowAttributes(display, children[i], &attributes))
      continue;
    (*window_attributes)[children[i]] = attributes;
  }

  XFree(children);
}

}  // namespace

ForeignWindowManagerHostX11::ForeignWindowManagerHostX11()
    : xdisplay_(base::MessagePumpAuraX11::GetDefaultXDisplay()),
      xwindow_(0),
      x_root_window_(DefaultRootWindow(xdisplay_)),
      configure_pending_(false),
      atom_cache_(xdisplay_, kAtomsToCache) {
  aura::Env::GetInstance()->AddObserver(this);
}

ForeignWindowManagerHostX11::~ForeignWindowManagerHostX11() {
  aura::Env::GetInstance()->RemoveObserver(this);
}

// static
void ForeignWindowManagerHostX11::SetX11ErrorHandlers() {
  // Set up error handlers that allow some errors to be ignored.
  g_base_x11_error_handler = XSetErrorHandler(X11ErrorHandlerImpl);
  g_base_x11_io_error_handler = XSetIOErrorHandler(X11IOErrorHandlerImpl);
}

// static
void ForeignWindowManagerHostX11::UnsetX11ErrorHandlers() {
  X11ErrorHandler old_x11_error_handler = XSetErrorHandler(
      g_base_x11_error_handler);
  DCHECK_EQ(X11ErrorHandlerImpl, old_x11_error_handler);
  X11IOErrorHandler old_x11_io_error_handler = XSetIOErrorHandler(
      g_base_x11_io_error_handler);
  DCHECK_EQ(X11IOErrorHandlerImpl, old_x11_io_error_handler);
}

// static
void ForeignWindowManagerHostX11::IgnoreX11Error(unsigned long sequence) {
  g_x11_errors_to_ignore_.Pointer()->push(sequence);
}

bool ForeignWindowManagerHostX11::Initialize() {
  // TODO(reveman): Improve existing WM check.
  if (XGetSelectionOwner(xdisplay_, atom_cache_.GetAtom("WM_S0")) != None) {
    LOG(ERROR) << "Existing WM running.";
    return false;
  }

  // This is for real. Use root window for window management.
  xwindow_ = x_root_window_;
  base::MessagePumpAuraX11::Current()->AddDispatcherForRootWindow(this);

  long event_mask = StructureNotifyMask |
                    SubstructureNotifyMask | SubstructureRedirectMask;

  // Grab server while selecting for events and retrieving the current
  // state of all child windows.
  XGrabServer(xdisplay_);

  // This selects for events. All child window changes will produce
  // events from now on.
  XSelectInput(xdisplay_, xwindow_, event_mask);

  // Get a list of all children and their current attributes.
  GetWindowAttributesForAllChildren(
      xdisplay_, xwindow_, &initial_window_attributes_);

  // We now have all child attributes we need. It's safe to release
  // the server grab.
  XUngrabServer(xdisplay_);

  // Redirect all sub-windows.
  XCompositeRedirectSubwindows(xdisplay_, xwindow_, CompositeRedirectManual);

  return true;
}

void ForeignWindowManagerHostX11::InitializeForTesting() {
  const std::string size_str =
      CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
          ash::switches::kAshHostWindowBounds);
  std::vector<std::string> parts;
  base::SplitString(size_str, ',', &parts);
  for (std::vector<std::string>::const_iterator iter = parts.begin();
       iter != parts.end(); ++iter) {
    AddDisplayFromSpec(*iter);
  }
  if (bounds_.IsEmpty())
    AddDisplayFromSpec(std::string() /* default */);

  // Create top-level window that behaves as root window for testing.
  xwindow_ = XCreateWindow(
      xdisplay_, x_root_window_,
      bounds_.x(), bounds_.y(), bounds_.width(), bounds_.height(),
      0,               // border width
      CopyFromParent,  // depth
      InputOutput,
      CopyFromParent,  // visual
      0,
      NULL);
  base::MessagePumpAuraX11::Current()->AddDispatcherForWindow(this, xwindow_);

  long event_mask = StructureNotifyMask |
                    SubstructureNotifyMask | SubstructureRedirectMask;
  // Select for WM events.
  XSelectInput(xdisplay_, xwindow_, event_mask);

  // Redirect all sub-windows.
  XCompositeRedirectSubwindows(xdisplay_, xwindow_, CompositeRedirectManual);

  // Protocols required to integrate with the desktop environment.
  ::Atom protocols[2];
  protocols[0] = atom_cache_.GetAtom("WM_DELETE_WINDOW");
  protocols[1] = atom_cache_.GetAtom("_NET_WM_PING");
  XSetWMProtocols(xdisplay_, xwindow_, protocols, 2);

  // We need a WM_CLIENT_MACHINE and WM_LOCALE_NAME value so we integrate with
  // the desktop environment.
  XSetWMProperties(xdisplay_, xwindow_, NULL, NULL, NULL, 0, NULL, NULL, NULL);

  // Likewise, the X server needs to know this window's pid so it knows which
  // program to kill if the window hangs.
  pid_t pid = getpid();
  XChangeProperty(xdisplay_,
                  xwindow_,
                  atom_cache_.GetAtom("_NET_WM_PID"),
                  XA_CARDINAL,
                  32,
                  PropModeReplace,
                  reinterpret_cast<unsigned char*>(&pid), 1);

  // Before we map the window, set size hints. Otherwise, some window managers
  // will ignore toplevel XMoveWindow commands.
  XSizeHints size_hints;
  size_hints.flags = PPosition | PWinGravity;
  size_hints.x = bounds_.x();
  size_hints.y = bounds_.y();
  // Set StaticGravity so that the window position is not affected by the
  // frame width when running with window manager.
  size_hints.win_gravity = StaticGravity;
  XSetWMNormalHints(xdisplay_, xwindow_, &size_hints);

  XMapWindow(xdisplay_, xwindow_);

  // We now block until our window is mapped. Some X11 APIs will crash and
  // burn if passed |xwindow_| before the window is mapped, and XMapWindow is
  // asynchronous.
  base::MessagePumpAuraX11::Current()->BlockUntilWindowMapped(xwindow_);
}

aura::RootWindowHost* ForeignWindowManagerHostX11::CreateRootWindowHost(
    const gfx::Rect& initial_bounds) {
  if (xwindow_ != x_root_window_) {
    // TODO(reveman): Refactor aura::RootWindowHostX11 so this gross
    // hack is not needed.
    DefaultRootWindow(xdisplay_) = xwindow_;
  }

  RootWindowHostImpl* host = new RootWindowHostImpl(
      initial_bounds - bounds_.OffsetFromOrigin());

  if (xwindow_ != x_root_window_)
    DefaultRootWindow(xdisplay_) = x_root_window_;

  // Unredirect native window.
  XCompositeUnredirectWindow(xdisplay_,
                             host->GetAcceleratedWidget(),
                             CompositeRedirectManual);

  return host;
}

gfx::AcceleratedWidget ForeignWindowManagerHostX11::GetAcceleratedWidget() {
  return xwindow_;
}

void ForeignWindowManagerHostX11::SetDefaultCursor(
    gfx::NativeCursor cursor) {
  XDefineCursor(xdisplay_, xwindow_, cursor.platform());
}

void ForeignWindowManagerHostX11::ShowForeignWindows() {
  for (WindowAttributesMap::iterator it = initial_window_attributes_.begin();
       it != initial_window_attributes_.end(); ++it) {
    int border_size = it->second.border_width * 2;
    gfx::Rect bounds(it->second.x,
                     it->second.y,
                     it->second.width + border_size,
                     it->second.height + border_size);

    // Create a foreign window for this X window.
    ForeignWindow::CreateParams params(it->first, bounds.size());
    params.managed = !it->second.override_redirect;
    scoped_refptr<ForeignWindow> window(new ForeignWindow(params));

    // Create top level window widget.
    views::Widget* widget = ForeignWindowWidget::CreateWindowWithBounds(
        window, bounds);
    if (it->second.map_state != IsUnmapped) {
      widget->Show();
      window->OnWindowVisibilityChanged(true);
    }

    // Add foreign window to map.
    foreign_windows_[it->first] = window.get();
  }
  initial_window_attributes_.clear();
}

bool ForeignWindowManagerHostX11::Dispatch(const base::NativeEvent& event) {
  XEvent* xev = event;

  // ShowForeignWindows() needs to be called before we start
  // processing events.
  DCHECK_EQ(0u, initial_window_attributes_.size());

  DiscardXErrorsToIgnore(xev->xany.serial);

  switch (xev->type) {
    case ClientMessage: {
      Atom message_type = static_cast<Atom>(xev->xclient.data.l[0]);
      if (message_type == atom_cache_.GetAtom("WM_DELETE_WINDOW")) {
        // We have received a close message from the window manager.
        ash::Shell::GetInstance()->delegate()->Exit();
      } else if (message_type == atom_cache_.GetAtom("_NET_WM_PING")) {
        XEvent reply_event = *xev;
        reply_event.xclient.window = x_root_window_;

        XSendEvent(xdisplay_,
                   reply_event.xclient.window,
                   False,
                   SubstructureRedirectMask | SubstructureNotifyMask,
                   &reply_event);
      }
      break;
    }
    case ConfigureNotify: {
      if (event->xconfigure.window == xwindow_) {
        if (xwindow_ != x_root_window_ && root_windows_.size() == 1) {
          for (WindowMap::iterator iter = root_windows_.begin();
               iter != root_windows_.end(); ++iter) {
            XResizeWindow(xdisplay_,
                          iter->first,
                          event->xconfigure.width,
                          event->xconfigure.height);
          }
        }
        return true;
      }
      ForeignWindowMap::iterator it = foreign_windows_.find(
          event->xconfigure.window);
      if (it != foreign_windows_.end()) {
        ForeignWindow* window = it->second;

        int border_size = event->xconfigure.border_width * 2;
        gfx::Rect bounds(event->xconfigure.x,
                         event->xconfigure.y,
                         event->xconfigure.width + border_size,
                         event->xconfigure.height + border_size);

        if (!window->IsManaged()) {
          views::Widget* widget = window->GetWidget();
          DCHECK(widget);
          widget->SetBounds(bounds);
        }

        window->OnWindowSizeChanged(bounds.size());
        return true;
      }
      break;
    }
    case MapNotify: {
      ForeignWindowMap::iterator it = foreign_windows_.find(
          event->xmap.window);
      if (it != foreign_windows_.end()) {
        ForeignWindow* window = it->second;

        if (!window->IsManaged()) {
          views::Widget* widget = window->GetWidget();
          DCHECK(widget);
          widget->Show();
        }

        window->OnWindowVisibilityChanged(true);
        return true;
      }
      break;
    }
    case UnmapNotify: {
      ForeignWindowMap::iterator it = foreign_windows_.find(
          event->xunmap.window);
      if (it != foreign_windows_.end()) {
        ForeignWindow* window = it->second;

        // Hide top level widget.
        views::Widget* widget = window->GetWidget();
        DCHECK(widget);
        widget->Hide();

        window->OnWindowVisibilityChanged(false);
        return true;
      }
      break;
    }
    case CreateNotify: {
      int border_size = event->xcreatewindow.border_width * 2;
      gfx::Rect bounds(event->xcreatewindow.x,
                       event->xcreatewindow.y,
                       event->xcreatewindow.width + border_size,
                       event->xcreatewindow.height + border_size);

      // Ignore native root windows.
      WindowMap::iterator it = root_windows_.find(event->xcreatewindow.window);
      if (it != root_windows_.end())
        return true;

      // Create a foreign window for this X window.
      ForeignWindow::CreateParams params(
          event->xcreatewindow.window, bounds.size());
      params.managed = !event->xcreatewindow.override_redirect;
      scoped_refptr<ForeignWindow> window(new ForeignWindow(params));

      // Create top level window widget.
      ForeignWindowWidget::CreateWindowWithBounds(window, bounds);

      // Add foreign window to map.
      foreign_windows_[event->xcreatewindow.window] = window.get();
      return true;
    }
    case DestroyNotify: {
      ForeignWindowMap::iterator it = foreign_windows_.find(
          event->xdestroywindow.window);
      if (it != foreign_windows_.end()) {
        ForeignWindow* window = it->second;

        // Tell foreign window that X window has been destroyed.
        window->OnWindowDestroyed();

        // Close top level widget.
        views::Widget* widget = window->GetWidget();
        DCHECK(widget);
        widget->Close();

        foreign_windows_.erase(it);
      }
      return true;
    }
    case MapRequest: {
      ForeignWindowMap::iterator it = foreign_windows_.find(
          event->xmaprequest.window);
      if (it != foreign_windows_.end()) {
        ForeignWindow* window = it->second;

        DCHECK(window->IsManaged());

        // Show top level widget.
        views::Widget* widget = window->GetWidget();
        DCHECK(widget);
        widget->Show();
      }
      return true;
    }
    case ConfigureRequest: {
      ForeignWindowMap::iterator it = foreign_windows_.find(
          event->xconfigurerequest.window);
      if (it != foreign_windows_.end()) {
        ForeignWindow* window = it->second;

        DCHECK(window->IsManaged());

        views::Widget* widget = window->GetWidget();
        DCHECK(widget);
        DCHECK(widget->non_client_view());
        DCHECK(widget->client_view());

        gfx::Rect bounds(widget->client_view()->GetBoundsInScreen());

        // TODO(reveman): Respect stacking order.
        if (event->xconfigurerequest.value_mask & CWX)
          bounds.set_x(event->xconfigurerequest.x);
        if (event->xconfigurerequest.value_mask & CWY)
          bounds.set_y(event->xconfigurerequest.y);
        if (event->xconfigurerequest.value_mask & CWWidth)
          bounds.set_width(event->xconfigurerequest.width);
        if (event->xconfigurerequest.value_mask & CWHeight)
          bounds.set_height(event->xconfigurerequest.height);

        widget->SetBounds(
            widget->non_client_view()->GetWindowBoundsForClientBounds(bounds));
      }
      return true;
    }
    case CirculateRequest: {
      // TODO(reveman): Respect CirculateRequest events.
      return true;
    }
  }

  return true;
}

void ForeignWindowManagerHostX11::OnWindowInitialized(aura::Window* window) {
  window->AddObserver(this);
}

void ForeignWindowManagerHostX11::OnRootWindowInitialized(
    aura::RootWindow* root_window) {
  ::Window xwindow = root_window->GetAcceleratedWidget();
  root_windows_[xwindow] = root_window;
  ScheduleConfigure(root_window);
}

void ForeignWindowManagerHostX11::OnWindowStackingChanged(
    aura::Window* window) {
  ScheduleConfigure(window->GetRootWindow());
}

void ForeignWindowManagerHostX11::OnWindowVisibilityChanged(
    aura::Window* window, bool visible) {
  if (window->GetRootWindow())
    ScheduleConfigure(window->GetRootWindow());
}

void ForeignWindowManagerHostX11::OnWindowBoundsChanged(
    aura::Window* window,
    const gfx::Rect& old_bounds,
    const gfx::Rect& new_bounds) {
  ScheduleConfigure(window->GetRootWindow());
}

void ForeignWindowManagerHostX11::OnWindowDestroyed(aura::Window* window) {
  window->RemoveObserver(this);
}

void ForeignWindowManagerHostX11::AddDisplayFromSpec(
    const std::string& spec) {
  ash::internal::DisplayInfo display_info =
      ash::internal::DisplayInfo::CreateFromSpec(spec);
  bounds_ = UnionRects(bounds_, display_info.bounds_in_pixel());
}

::Window ForeignWindowManagerHostX11::GetTopForeignWindow(
    const aura::Window* window) {
  // children are ordered back to front, so walk through it in reverse.
  const aura::Window::Windows& children = window->children();
  for (size_t i = children.size(); i; --i) {
    ::Window top = GetTopForeignWindow(children[i - 1]);
    if (top)
      return top;
  }

  ForeignWindow* foreign_window = ForeignWindow::GetForeignWindowForNativeView(
      const_cast<gfx::NativeView>(window));
  if (foreign_window) {
    // Ignore windows that we're not allowed to configure.
    if (foreign_window->IsManaged())
      return foreign_window->GetWindowHandle();
  }

  return 0;
}

::Window ForeignWindowManagerHostX11::FindForeignWindowToStackAbove(
    const aura::Window* window) {
  const aura::Window* parent = window->parent();
  if (!parent)
    return 0;

  ::Window above = 0;

  const aura::Window::Windows& children = parent->children();
  for (size_t i = 0; i < children.size(); ++i) {
    if (children[i] == window)
      break;

    ::Window top = GetTopForeignWindow(children[i]);
    if (top)
      above = top;
  }

  if (!above)
    above = FindForeignWindowToStackAbove(parent);

  return above;
}

void ForeignWindowManagerHostX11::MapWindowIfNeeded(
    ForeignWindow* window) {
  ForeignWindow::DisplayState current_display_state =
      window->GetDisplayState();

  if (current_display_state != ForeignWindow::DISPLAY_NORMAL) {
    IgnoreX11Error(NextRequest(xdisplay_));
    XMapWindow(xdisplay_, window->GetWindowHandle());

    window->SetDisplayState(ForeignWindow::DISPLAY_NORMAL);
  }
}

void ForeignWindowManagerHostX11::UnmapWindowIfNeeded(
    ForeignWindow* window) {
  ForeignWindow::DisplayState current_display_state =
      window->GetDisplayState();

  if (current_display_state == ForeignWindow::DISPLAY_NORMAL) {
    IgnoreX11Error(NextRequest(xdisplay_));
    XUnmapWindow(xdisplay_, window->GetWindowHandle());

    window->SetDisplayState(ForeignWindow::DISPLAY_ICONIC);
  }
}

void ForeignWindowManagerHostX11::RecursiveConfigure(
    aura::Window* window,
    gfx::Vector2d offset,
    ::Window* sibling_to_stack_above,
    SkRegion* input_region) {
  gfx::Rect bounds(gfx::Rect(window->GetTargetBounds().size()) + offset);
  SkRegion::Op input_region_op;

  ForeignWindow* foreign_window = ForeignWindow::GetForeignWindowForNativeView(
      const_cast<gfx::NativeView>(window));
  if (foreign_window) {
    DCHECK(!bounds.IsEmpty());

    // We should only be adjusting attributes of managed windows.
    if (foreign_window->IsManaged() && !foreign_window->HasBeenDestroyed()) {
      if (!window->IsVisible())
        UnmapWindowIfNeeded(foreign_window);

      XWindowChanges wc;
      unsigned mask = InitWindowChanges(bounds, *sibling_to_stack_above, wc);
      // Get rid of any borders.
      wc.border_width = 0;
      mask |= CWBorderWidth;
      IgnoreX11Error(NextRequest(xdisplay_));
      XConfigureWindow(xdisplay_,
                       foreign_window->GetWindowHandle(),
                       mask,
                       &wc);

      *sibling_to_stack_above = foreign_window->GetWindowHandle();

      if (window->IsVisible())
        MapWindowIfNeeded(foreign_window);
    }

    // Remove foreign window bounds from input region.
    bounds.Inset(window->hit_test_bounds_override_inner());
    input_region_op = SkRegion::kDifference_Op;
  } else {
    // Add window bounds to output region.
    bounds.Inset(window->hit_test_bounds_override_outer_mouse());
    input_region_op = SkRegion::kUnion_Op;
  }

  if (window->delegate() && window->IsVisible()) {
    input_region->op(bounds.x(),
                     bounds.y(),
                     bounds.right(),
                     bounds.bottom(),
                     input_region_op);
  }

  const aura::Window::Windows& children = window->children();
  for (size_t i = 0; i < children.size(); ++i) {
    RecursiveConfigure(
        children[i],
        offset + children[i]->GetTargetBounds().OffsetFromOrigin(),
        sibling_to_stack_above,
        input_region);
  }
}

void ForeignWindowManagerHostX11::Configure() {
  DCHECK(configure_pending_);
  configure_pending_ = false;

  for (WindowMap::iterator iter = configure_windows_.begin();
       iter != configure_windows_.end();
       ++iter) {
    aura::Window* configure_window = iter->second;
    aura::RootWindow* root_window = configure_window->GetRootWindow();
    gfx::Point root_origin = root_window->GetHostOrigin();
    gfx::Vector2d root_offset(root_origin - gfx::Point());

    SkRegion input_region;
    ::Window sibling_to_stack_above =
        FindForeignWindowToStackAbove(configure_window);
    RecursiveConfigure(
        configure_window,
        root_offset + GetTargetOffsetInRootWindow(configure_window),
        &sibling_to_stack_above,
        &input_region);

    if (configure_window == root_window) {
      ::Window xwindow = iter->first;
      DCHECK(xwindow);

      // Configure native root window above all other windows.
      XWindowChanges wc;
      gfx::Rect bounds(root_origin, root_window->GetHostSize());
      unsigned mask = InitWindowChanges(bounds, sibling_to_stack_above, wc);
      XConfigureWindow(xdisplay_, xwindow, mask, &wc);

      // Update input shape.
      std::vector<XRectangle> rectangles;
      for (SkRegion::Iterator iter(input_region); !iter.done(); iter.next()) {
        const SkIRect& sk_rect = iter.rect();
        XRectangle x_rect;
        x_rect.x = sk_rect.x() - root_offset.x();
        x_rect.y = sk_rect.y() - root_offset.y();
        x_rect.width = sk_rect.width();
        x_rect.height = sk_rect.height();
        rectangles.push_back(x_rect);
      }

      XShapeCombineRectangles(xdisplay_,
                              xwindow,
                              ShapeInput,
                              0,
                              0,
                              &rectangles.front(),
                              rectangles.size(),
                              ShapeSet,
                              Unsorted);
    }
  }

  configure_windows_.clear();
  XFlush(xdisplay_);
}

void ForeignWindowManagerHostX11::ScheduleConfigure(aura::Window* window) {
  DCHECK(window);
  DCHECK(window->GetRootWindow());
  aura::RootWindow* root_window = window->GetRootWindow();

  ::Window xwindow = root_window->GetAcceleratedWidget();
  // Ignore configure when root window is not yet initialized.
  if (root_windows_.find(xwindow) == root_windows_.end())
    return;

  if (!configure_pending_) {
    DCHECK(configure_windows_.find(xwindow) == configure_windows_.end());
    MessageLoop::current()->PostTask(
        FROM_HERE,
        base::Bind(&ForeignWindowManagerHostX11::Configure, AsWeakPtr()));
    configure_pending_ = true;
  }

  if (configure_windows_.find(xwindow) == configure_windows_.end()) {
    configure_windows_[xwindow] = window;
    return;
  }

  configure_windows_[xwindow] = FindLowestCommonAncestor(
      root_window, configure_windows_[xwindow], window);
}

}  // namespace wm
