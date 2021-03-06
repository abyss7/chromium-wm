// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/foreign_window_manager.h"

#include "ash/host/root_window_host_factory.h"
#include "ash/wm/image_cursors.h"
#include "ui/aura/root_window.h"
#include "wm/host/foreign_window_manager_host.h"
#include "wm/shell_window_ids.h"

namespace wm {

namespace {

class RootWindowHostFactoryImpl : public ash::RootWindowHostFactory {
 public:
  RootWindowHostFactoryImpl(ForeignWindowManagerHost* host) : host_(host) {}

  // Overridden from ash::RootWindowHostFactory:
  virtual aura::RootWindowHost* CreateRootWindowHost(
      const gfx::Rect& initial_bounds) OVERRIDE {
    return host_->CreateRootWindowHost(initial_bounds);
  }

 private:
  ForeignWindowManagerHost* host_;
};

}  // namespace

// static
ForeignWindowManager* ForeignWindowManager::instance_ = NULL;

ForeignWindowManager::ForeignWindowManager()
    : host_(ForeignWindowManagerHost::Create()),
      image_cursors_(new ash::ImageCursors),
      initialized_(false) {
}

ForeignWindowManager::~ForeignWindowManager() {
}

// static
ForeignWindowManager* ForeignWindowManager::CreateInstance() {
  CHECK(!instance_);
  instance_ = new ForeignWindowManager;
  if (!instance_->Initialize()) {
    delete instance_;
    instance_ = NULL;
  }
  return instance_;
}

// static
ForeignWindowManager* ForeignWindowManager::CreateInstanceForTesting() {
  CHECK(!instance_);
  instance_ = new ForeignWindowManager;
  instance_->InitializeForTesting();
  return instance_;
}

// static
ForeignWindowManager* ForeignWindowManager::GetInstance() {
  DCHECK(instance_);
  return instance_;
}

// static
bool ForeignWindowManager::HasInstance() {
  return !!instance_;
}

// static
void ForeignWindowManager::DeleteInstance() {
  delete instance_;
  instance_ = NULL;
}

bool ForeignWindowManager::Initialize() {
  DCHECK(!initialized_);
  initialized_ = host_->Initialize();
  return initialized_;
}

void ForeignWindowManager::InitializeForTesting() {
  DCHECK(!initialized_);
  host_->InitializeForTesting();
  initialized_ = true;
}

ash::RootWindowHostFactory* ForeignWindowManager::
    CreateRootWindowHostFactory() {
  DCHECK(initialized_);
  return new RootWindowHostFactoryImpl(host_.get());
}

gfx::AcceleratedWidget ForeignWindowManager::GetAcceleratedWidget() {
  DCHECK(initialized_);
  return host_->GetAcceleratedWidget();
}

void ForeignWindowManager::SetDisplay(const gfx::Display& display) {
  DCHECK(initialized_);
  image_cursors_->SetDisplay(display);
}

void ForeignWindowManager::SetDefaultCursor(gfx::NativeCursor cursor) {
  DCHECK(initialized_);
  gfx::NativeCursor new_cursor = cursor;
  image_cursors_->SetPlatformCursor(&new_cursor);
  new_cursor.set_device_scale_factor(
      image_cursors_->GetDisplay().device_scale_factor());
  host_->SetDefaultCursor(new_cursor);
}

void ForeignWindowManager::CreateContainers(aura::RootWindow* root_window) {
  DCHECK(initialized_);
  aura::Window* container = new aura::Window(NULL);
  container->set_id(internal::kShellWindowId_UnmanagedWindowContainer);
  container->SetName("ForeignUnmanagedWindowContainer");
  container->Init(ui::LAYER_NOT_DRAWN);
  root_window->AddChild(container);
  container->Show();
}

void ForeignWindowManager::ShowForeignWindows() {
  DCHECK(initialized_);
  host_->ShowForeignWindows();
}

}  // namespace wm
