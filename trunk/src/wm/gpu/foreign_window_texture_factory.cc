// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/gpu/foreign_window_texture_factory.h"

#include "base/lazy_instance.h"
#include "base/observer_list.h"
#include "content/browser/gpu/browser_gpu_channel_host_factory.h"
#include "content/common/gpu/client/gl_helper.h"
#include "third_party/WebKit/Source/Platform/chromium/public/WebGraphicsContext3D.h"
#include "third_party/khronos/GLES2/gl2.h"
#include "ui/compositor/compositor_setup.h"

namespace {

class DefaultGpuChannelHostFactory : public content::GpuChannelHostFactory {
 public:
  DefaultGpuChannelHostFactory() {}

  virtual bool IsMainThread() OVERRIDE {
    return false;
  }
  virtual bool IsIOThread() OVERRIDE {
    return false;
  }
  virtual MessageLoop* GetMainLoop() OVERRIDE {
    return NULL;
  }
  virtual scoped_refptr<base::MessageLoopProxy> GetIOLoopProxy() OVERRIDE {
    return NULL;
  }
  virtual base::WaitableEvent* GetShutDownEvent() OVERRIDE { return NULL; }
  virtual scoped_ptr<base::SharedMemory> AllocateSharedMemory(
      size_t size) OVERRIDE {
    return scoped_ptr<base::SharedMemory>();
  }
  virtual int32 CreateViewCommandBuffer(
      int32 surface_id,
      const GPUCreateCommandBufferConfig& init_params) OVERRIDE {
    return 0;
  }
  virtual content::GpuChannelHost* EstablishGpuChannelSync(
      content::CauseForGpuLaunch) OVERRIDE {
    return NULL;
  }
  virtual void CreateImage(
      gfx::PluginWindowHandle window_handle,
      int32 image_id,
      const CreateImageCallback& callback) OVERRIDE {
    callback.Run(gfx::Size());
  }
  virtual void DeleteImage(int32 image_id, int32 sync_point) OVERRIDE {}
};

base::LazyInstance<DefaultGpuChannelHostFactory> g_default_factory =
    LAZY_INSTANCE_INITIALIZER;

}  // namespace

namespace wm {

// static
ForeignWindowTextureFactory* ForeignWindowTextureFactory::instance_ = NULL;

ForeignWindowTexture::ForeignWindowTexture(
    content::GpuChannelHostFactory* factory,
    WebKit::WebGraphicsContext3D* host_context,
    bool flipped,
    const gfx::Size& size,
    float device_scale_factor,
    int32 image_id)
    : ui::Texture(flipped, size, device_scale_factor),
      factory_(factory),
      host_context_(host_context),
      image_id_(image_id),
      texture_id_(0),
      contents_changed_(true) {
  ForeignWindowTextureFactory::GetInstance()->AddObserver(this);
}

ForeignWindowTexture::~ForeignWindowTexture() {
  ForeignWindowTextureFactory::GetInstance()->RemoveObserver(this);
  if (texture_id_)
    host_context_->deleteTexture(texture_id_);
  if (image_id_ && !host_context_->isContextLost()) {
    factory_->DeleteImage(image_id_,
                          texture_id_ ? host_context_->insertSyncPoint() : 0);
  }
}

unsigned int ForeignWindowTexture::PrepareTexture() {
  if (contents_changed_) {
    unsigned texture_id = texture_id_;

    if (!texture_id)
      texture_id = host_context_->createTexture();

    host_context_->bindTexture(GL_TEXTURE_2D, texture_id);
    if (texture_id_)
      host_context_->releaseTexImage2DCHROMIUM(GL_TEXTURE_2D, image_id_);
    host_context_->bindTexImage2DCHROMIUM(GL_TEXTURE_2D, image_id_);
    host_context_->bindTexture(GL_TEXTURE_2D, 0);
    host_context_->flush();

    texture_id_ = texture_id;
    contents_changed_ = false;
  }

  return texture_id_;
}

WebKit::WebGraphicsContext3D* ForeignWindowTexture::HostContext3D() {
  DCHECK(host_context_);  // This should never be called after lost context.
  return host_context_;
}

void ForeignWindowTexture::OnLostResources() {
  if (texture_id_) {
    host_context_->deleteTexture(texture_id_);
    texture_id_ = 0;
  }
  contents_changed_ = false;
  image_id_ = 0;
  host_context_ = NULL;
}

void ForeignWindowTexture::OnContentsChanged() {
  if (image_id_)
    contents_changed_ = true;
}

ForeignWindowTextureFactory::ForeignWindowTextureFactory(
    content::GpuChannelHostFactory* factory) : factory_(factory) {
  DCHECK(factory);
  DCHECK(content::ImageTransportFactory::GetInstance());
  content::ImageTransportFactory::GetInstance()->AddObserver(this);
}

ForeignWindowTextureFactory::~ForeignWindowTextureFactory() {
  content::ImageTransportFactory::GetInstance()->RemoveObserver(this);
}

void ForeignWindowTextureFactory::OnLostResources() {
  content::ImageTransportFactory* transport_factory =
      content::ImageTransportFactory::GetInstance();
  // Make sure a new gpu channel is established before any calls to
  // CreateTextureForForeignWindow() are made.
  WebKit::WebGraphicsContext3D* context =
      transport_factory->GetGLHelper()->context();
  CHECK(context);

  FOR_EACH_OBSERVER(ForeignWindowTextureFactoryObserver,
                    observer_list_,
                    OnLostResources());
}

void ForeignWindowTextureFactory::CreateTextureForForeignWindow(
    gfx::PluginWindowHandle window_handle,
    bool flipped,
    float device_scale_factor,
    const CreateTextureCallback& callback) {
  DCHECK(window_handle);

  int32 image_id = GenerateImageID();
  factory_->CreateImage(
      window_handle,
      image_id,
      base::Bind(&ForeignWindowTextureFactory::OnImageCreated,
                 base::Unretained(this),
                 callback,
                 flipped,
                 device_scale_factor,
                 image_id));
}

void ForeignWindowTextureFactory::AddObserver(
    ForeignWindowTextureFactoryObserver* observer) {
  observer_list_.AddObserver(observer);
}

void ForeignWindowTextureFactory::RemoveObserver(
    ForeignWindowTextureFactoryObserver* observer) {
  observer_list_.RemoveObserver(observer);
}

int ForeignWindowTextureFactory::GenerateImageID() {
  static unsigned int image_id = 0;
  return ++image_id;
}

void ForeignWindowTextureFactory::OnImageCreated(
    const CreateTextureCallback& callback,
    bool flipped,
    float device_scale_factor,
    int32 image_id,
    const gfx::Size size) {
  if (size.IsEmpty()) {
    callback.Run(NULL);
    return;
  }

  content::ImageTransportFactory* transport_factory =
      content::ImageTransportFactory::GetInstance();
  scoped_refptr<ForeignWindowTexture> texture(
      new ForeignWindowTexture(factory_,
                               transport_factory->GetGLHelper()->context(),
                               flipped,
                               size,
                               device_scale_factor,
                               image_id));

  callback.Run(texture);
}

// static
void ForeignWindowTextureFactory::Initialize() {
  CHECK(!instance_);
  if (ui::IsTestCompositorEnabled()) {
    instance_ = new ForeignWindowTextureFactory(&g_default_factory.Get());
  } else {
    instance_ = new ForeignWindowTextureFactory(
        content::BrowserGpuChannelHostFactory::instance());
  }
}

// static
void ForeignWindowTextureFactory::Terminate() {
  CHECK(instance_);
  delete instance_;
  instance_ = NULL;
}

// static
ForeignWindowTextureFactory* ForeignWindowTextureFactory::GetInstance() {
  DCHECK(instance_);
  return instance_;
}

}  // namespace wm
