// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WM_SHELL_EXAMPLE_FACTORY_H_
#define WM_SHELL_EXAMPLE_FACTORY_H_

namespace app_list {
class AppListViewDelegate;
}

namespace wm {
namespace shell {

void CreateManagedWindow();

void CreateUnmanagedWindow();

app_list::AppListViewDelegate* CreateAppListViewDelegate();

}  // namespace shell
}  // namespace wm

#endif  // WM_SHELL_EXAMPLE_FACTORY_H_
