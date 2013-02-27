// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WM_SHELL_CONTENT_CLIENT_SHELL_CONTENT_BROWSER_CLIENT_H_
#define WM_SHELL_CONTENT_CLIENT_SHELL_CONTENT_BROWSER_CLIENT_H_

#include "content/public/browser/content_browser_client.h"

namespace content {
class ShellBrowserContext;
class ShellBrowserMainParts;
}

namespace wm {
namespace shell {

class ShellBrowserMainParts;

class ShellContentBrowserClient : public content::ContentBrowserClient {
 public:
  ShellContentBrowserClient();
  virtual ~ShellContentBrowserClient();

  // Overridden from content::ContentBrowserClient:
  virtual content::BrowserMainParts* CreateBrowserMainParts(
      const content::MainFunctionParams& parameters) OVERRIDE;

  content::ShellBrowserContext* browser_context();

 private:
  ShellBrowserMainParts* shell_browser_main_parts_;

  DISALLOW_COPY_AND_ASSIGN(ShellContentBrowserClient);
};

}  // namespace shell
}  // namespace wm

#endif  // WM_SHELL_CONTENT_CLIENT_SHELL_CONTENT_BROWSER_CLIENT_H_
