// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WM_HOST_FOREIGN_TEST_WINDOW_HOST_H_
#define WM_HOST_FOREIGN_TEST_WINDOW_HOST_H_

namespace wm {
class ForeignWindowManager;

class ForeignTestWindowHost {
 public:
  virtual ~ForeignTestWindowHost() {}

  // Creates a new ForeignTestWindowHost. Caller owns return value and must
  // call Destroy() to delete instance.
  static ForeignTestWindowHost* Create(ForeignWindowManager* window_manager);

  // This is guaranteed to run all tasks up to the last Sync() call.
  static void RunAllPendingInMessageLoop();

  // These functions should only be called on a thread with an IO message loop.
  virtual void Initialize() = 0;
  virtual void Delete() = 0;
  virtual void Show() = 0;
  virtual void Hide() = 0;
  virtual void Destroy() = 0;
  virtual void Sync() = 0;
};

}  // namespace wm

#endif  // WM_HOST_FOREIGN_TEST_WINDOW_HOST_H_
