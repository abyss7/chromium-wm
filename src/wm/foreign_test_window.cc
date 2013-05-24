// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/foreign_test_window.h"

#include "base/bind.h"
#include "base/lazy_instance.h"
#include "base/run_loop.h"
#include "base/threading/thread.h"
#include "ui/aura/env.h"
#include "wm/foreign_window_manager.h"
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

void AddOnDestroyCallbackOnIO(ForeignTestWindowHost* host,
                              const base::Closure& callback) {
  host->AddOnDestroyCallback(callback);
}

void PostMessageOnIO(scoped_refptr<base::MessageLoopProxy> message_loop_proxy,
                     const base::Closure& callback) {
  message_loop_proxy->PostTask(FROM_HERE, callback);
}

void OnWindowDestroyed(scoped_ptr<ForeignTestWindow> window) {
  window.reset();  // Delete window
}

}  // namespace

ForeignTestWindow::CreateParams::CreateParams()
    : bounds(gfx::Rect(50, 50, 400, 300)),
      managed(true),
      show(true) {
}

// static
void ForeignTestWindow::Create(const CreateParams& params) {
  scoped_ptr<ForeignTestWindow> window(new ForeignTestWindow(params));
  if (params.show)
    window->Show();

  // Delete instance when foreign window has been destroyed.
  ForeignTestWindowHost* host = window->host_;
  g_foreign_test_window_thread.Pointer()->message_loop_proxy()->PostTask(
      FROM_HERE,
      base::Bind(&AddOnDestroyCallbackOnIO,
                 host,
                 base::Bind(&PostMessageOnIO,
                            MessageLoop::current()->message_loop_proxy(),
                            base::Bind(&OnWindowDestroyed,
                                       base::Passed(&window)))));
}

ForeignTestWindow::ForeignTestWindow(const CreateParams& params) {
  ForeignWindowManager* foreign_window_manager =
      ForeignWindowManager::GetInstance();
  host_ = ForeignTestWindowHost::Create(
      foreign_window_manager,
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
