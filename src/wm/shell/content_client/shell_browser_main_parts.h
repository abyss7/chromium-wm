// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WM_SHELL_CONTENT_CLIENT_SHELL_BROWSER_MAIN_PARTS_H_
#define WM_SHELL_CONTENT_CLIENT_SHELL_BROWSER_MAIN_PARTS_H_

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "content/public/browser/browser_main_parts.h"

namespace content {
class ShellBrowserContext;
struct MainFunctionParams;
}

namespace ash {
namespace shell {
class WindowWatcher;
}
}

namespace wm {
namespace shell {

class ShellBrowserMainParts : public content::BrowserMainParts {
 public:
  explicit ShellBrowserMainParts(
      const content::MainFunctionParams& parameters);
  virtual ~ShellBrowserMainParts();

  // Overridden from content::BrowserMainParts:
  virtual void PreMainMessageLoopStart() OVERRIDE;
  virtual void PostMainMessageLoopStart() OVERRIDE;
  virtual void PreMainMessageLoopRun() OVERRIDE;
  virtual bool MainMessageLoopRun(int* result_code) OVERRIDE;
  virtual void PostMainMessageLoopRun() OVERRIDE;

  content::ShellBrowserContext* browser_context() {
    return browser_context_.get();
  }

 private:
  scoped_ptr<content::ShellBrowserContext> browser_context_;
  scoped_ptr<ash::shell::WindowWatcher> window_watcher_;

  DISALLOW_COPY_AND_ASSIGN(ShellBrowserMainParts);
};

}  // namespace shell
}  // namespace wm

#endif  // WM_SHELL_CONTENT_CLIENT_SHELL_BROWSER_MAIN_PARTS_H_
