// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/public/app/content_main.h"
#include "wm/shell/content_client/shell_main_delegate.h"

int main(int argc, const char** argv) {
  wm::shell::ShellMainDelegate delegate;
  return content::ContentMain(argc, argv, &delegate);
}
