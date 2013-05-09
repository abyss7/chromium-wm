// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/foreign_test_window.h"

#include "base/bind.h"
#include "base/lazy_instance.h"
#include "base/run_loop.h"
#include "base/threading/thread.h"
#include "ui/aura/env.h"
#include "wm/host/foreign_test_window_host.h"

namespace wm {

namespace {

const char kForeignTestWindowThreadName[] = "ForeignTestWindowThread";

class ForeignTestWindowThread : public base::Thread {
 public:
  ForeignTestWindowThread() : base::Thread(kForeignTestWindowThreadName) {
    base::Thread::Options options;
    options.message_loop_type = MessageLoop::TYPE_IO;
    StartWithOptions(options);
  }
  virtual ~ForeignTestWindowThread() {
    Stop();
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(ForeignTestWindowThread);
};

base::LazyInstance<ForeignTestWindowThread> g_foreign_test_window_thread =
    LAZY_INSTANCE_INITIALIZER;

void InitializeOnIO(ForeignTestWindowHost* host) {
  host->Initialize();
}

void DeleteOnIO(ForeignTestWindowHost* host) {
  host->Delete();
}

void ShowOnIO(ForeignTestWindowHost* host) {
  host->Show();
}

void HideOnIO(ForeignTestWindowHost* host) {
  host->Hide();
}

void DestroyOnIO(ForeignTestWindowHost* host) {
  host->Destroy();
}

void SyncOnIO(ForeignTestWindowHost* host) {
  host->Sync();
}

void SyncCompleted(base::RunLoop* run_loop) {
  run_loop->Quit();
}

void SetBoundsOnIO(ForeignTestWindowHost* host, const gfx::Rect& bounds) {
  host->SetBounds(bounds);
}

}  // namespace

ForeignTestWindow::CreateParams::CreateParams(
    ForeignWindowManager* window_manager)
    : window_manager(window_manager),
      bounds(gfx::Rect(50, 50, 400, 300)),
      managed(true) {
}

ForeignTestWindow::ForeignTestWindow(const CreateParams& params) {
  host_ = ForeignTestWindowHost::Create(
      params.window_manager,
      params.bounds,
      params.managed);
  g_foreign_test_window_thread.Pointer()->message_loop_proxy()->PostTask(
      FROM_HERE,
      base::Bind(&InitializeOnIO, host_));
}

ForeignTestWindow::~ForeignTestWindow() {
  g_foreign_test_window_thread.Pointer()->message_loop_proxy()->PostTask(
      FROM_HERE,
      base::Bind(&DeleteOnIO, host_));
}

void ForeignTestWindow::Show() {
  g_foreign_test_window_thread.Pointer()->message_loop_proxy()->PostTask(
      FROM_HERE,
      base::Bind(&ShowOnIO, host_));
}

void ForeignTestWindow::Hide() {
  g_foreign_test_window_thread.Pointer()->message_loop_proxy()->PostTask(
      FROM_HERE,
      base::Bind(&HideOnIO, host_));
}

void ForeignTestWindow::Destroy() {
  g_foreign_test_window_thread.Pointer()->message_loop_proxy()->PostTask(
      FROM_HERE,
      base::Bind(&DestroyOnIO, host_));
}

void ForeignTestWindow::Sync() {
  base::RunLoop run_loop(aura::Env::GetInstance()->GetDispatcher());
  g_foreign_test_window_thread.Pointer()->message_loop_proxy()->
      PostTaskAndReply(FROM_HERE,
                       base::Bind(&SyncOnIO, host_),
                       base::Bind(&SyncCompleted,
                                  base::Unretained(&run_loop)));
  run_loop.Run();
}

void ForeignTestWindow::SetBounds(const gfx::Rect& bounds) {
  g_foreign_test_window_thread.Pointer()->message_loop_proxy()->PostTask(
      FROM_HERE,
      base::Bind(&SetBoundsOnIO, host_, bounds));
}

}  // namespace wm
