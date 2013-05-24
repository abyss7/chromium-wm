// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WM_SHELL_CONTENT_CLIENT_SHELL_MAIN_DELEGATE_H_
#define WM_SHELL_CONTENT_CLIENT_SHELL_MAIN_DELEGATE_H_

#include "base/memory/scoped_ptr.h"
#include "content/public/app/content_main_delegate.h"
#include "content/shell/common/shell_content_client.h"

namespace wm {
namespace shell {

class ShellContentBrowserClient;

class ShellMainDelegate : public content::ContentMainDelegate {
 public:
  ShellMainDelegate();
  virtual ~ShellMainDelegate();

  virtual bool BasicStartupComplete(int* exit_code) OVERRIDE;
  virtual void PreSandboxStartup() OVERRIDE;
  virtual content::ContentBrowserClient* CreateContentBrowserClient() OVERRIDE;

 private:
  void InitializeResourceBundle();

  scoped_ptr<ShellContentBrowserClient> browser_client_;
  content::ShellContentClient content_client_;

  DISALLOW_COPY_AND_ASSIGN(ShellMainDelegate);
};

}  // namespace shell
}  // namespace wm

#endif  // WM_SHELL_CONTENT_CLIENT_SHELL_MAIN_DELEGATE_H_
