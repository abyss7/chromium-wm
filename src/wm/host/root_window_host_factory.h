// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WM_HOST_ROOT_WINDOW_FACTORY_HOST_H_
#define WM_HOST_ROOT_WINDOW_FACTORY_HOST_H_

#include "ash/host/root_window_host_factory.h"

namespace wm {

class RootWindowHostFactory : public ash::RootWindowHostFactory {
 public:
  virtual ~RootWindowHostFactory() {}

  static RootWindowHostFactory* Create();

 protected:
  RootWindowHostFactory() {}
};

}  // namespace wm

#endif  // WM_HOST_ROOT_WINDOW_HOST_FACTORY_H_
