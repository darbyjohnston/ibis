// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "USDRender.h"

#include <ftk/GL/GL.h>
#include <ftk/GL/Init.h>
#include <ftk/Core/FileIO.h>
#include <ftk/Core/Format.h>
#include <ftk/Core/LRUCache.h>
#include <ftk/Core/Memory.h>

#include <pxr/pxr.h>
#include <pxr/base/tf/diagnostic.h>
#include <pxr/base/tf/token.h>
#include <pxr/usd/usd/timeCode.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usdGeom/camera.h>
#include <pxr/usd/usdGeom/bboxCache.h>
#include <pxr/usd/usdGeom/metrics.h>
#include <pxr/usd/usdUtils/pipeline.h>
#include <pxr/usdImaging/usdAppUtils/api.h>
#include <pxr/usdImaging/usdAppUtils/camera.h>
#include <pxr/usdImaging/usdAppUtils/frameRecorder.h>
#include <pxr/imaging/hd/renderBuffer.h>
#include <pxr/imaging/hdSt/hioConversions.h>
#include <pxr/imaging/hdSt/textureUtils.h>
#include <pxr/imaging/hdx/tokens.h>
#include <pxr/imaging/hdx/types.h>

#include <SDL3/SDL.h>

#include <filesystem>

using namespace PXR_NS;

namespace ibis
{
    namespace usd
    {
        struct Render::Private
        {
            std::weak_ptr<ftk::LogSystem> logSystem;

            SDL_Window* sdlWindow = nullptr;
            SDL_GLContext sdlGLContext = nullptr;
            
            struct InfoRequest
            {
                int64_t id = -1;
                std::string path;
                Options options;
                std::promise<Info> promise;
            };

            struct Request
            {
                int64_t id = -1;
                std::string path;
                OTIO_NS::RationalTime time;
                Options options;
                std::promise<std::shared_ptr<ftk::Image> > promise;
            };
            
            struct Mutex
            {
                std::list<std::shared_ptr<InfoRequest> > infoRequests;
                std::list<std::shared_ptr<Request> > requests;
                bool stopped = false;
                std::mutex mutex;
            };
            Mutex mutex;
            
            struct StageCacheItem
            {
                UsdStageRefPtr stage;
                std::shared_ptr<UsdImagingGLEngine> engine;
            };
            
            struct Thread
            {
                ftk::LRUCache<std::string, StageCacheItem> stageCache;
                std::chrono::steady_clock::time_point logTimer;
                std::condition_variable cv;
                std::thread thread;
                std::atomic<bool> running;
            };
            Thread thread;
        };
        
        void Render::_init(const std::shared_ptr<ftk::LogSystem>& logSystem)
        {
            FTK_P();

            p.logSystem = logSystem;

            try
            {
#if defined(__APPLE__)
                const int glVersionMinor = 1;
                const int glProfile = SDL_GL_CONTEXT_PROFILE_CORE;
#else //__APPLE__
                const int glVersionMinor = 5;
                const int glProfile = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY;
#endif //__APPLE__
                SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
                SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, glVersionMinor);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, glProfile);
                p.sdlWindow = SDL_CreateWindow(
                    "USD",
                    100,
                    100,
                    SDL_WINDOW_OPENGL |
                    SDL_WINDOW_RESIZABLE |
                    SDL_WINDOW_HIDDEN);
                if (!p.sdlWindow)
                {
                    throw std::runtime_error(ftk::Format("Cannot create window: {0}").
                        arg(SDL_GetError()));
                }

                p.sdlGLContext = SDL_GL_CreateContext(p.sdlWindow);
                if (!p.sdlGLContext)
                {
                    throw std::runtime_error(ftk::Format("Cannot create OpenGL context: {0}").
                        arg(SDL_GetError()));
                }
            }
            catch (const std::exception&e)
            {
                logSystem->print(
                    "tl::usd::Render",
                    e.what(),
                    ftk::LogType::Error);
            }

            /*ftk::gl::initGLAD();

            std::string glVendor;
            std::string glRenderer;
            std::string glVersion;
            if (const GLubyte* glString = glGetString(GL_VENDOR))
            {
                glVendor = std::string((const char*)glString);
            }
            if (const GLubyte* glString = glGetString(GL_RENDERER))
            {
                glRenderer = std::string((const char*)glString);
            }
            if (const GLubyte* glString = glGetString(GL_VERSION))
            {
                glVersion = std::string((const char*)glString);
            }
            logSystem->print(
                "tl::usd::Render",
                ftk::Format(
                    "\n"
                    "    glVendor:   {0}\n"
                    "    glRenderer: {1}\n"
                    "    glVersion:  {2}").
                arg(glVendor).
                arg(glRenderer).
                arg(glVersion));*/

            SDL_GL_MakeCurrent(p.sdlWindow, nullptr);

            p.thread.logTimer = std::chrono::steady_clock::now();
            p.thread.running = true;
            p.thread.thread = std::thread(
                [this]
                {
                    FTK_P();
                    if (p.sdlWindow && p.sdlGLContext)
                    {
                        SDL_GL_MakeCurrent(p.sdlWindow, p.sdlGLContext);
                    }
                    _run();
                    p.thread.stageCache.clear();
                    _finish();
                    if (p.sdlWindow && p.sdlGLContext)
                    {
                        SDL_GL_MakeCurrent(p.sdlWindow, nullptr);
                    }
                });
            
            {
                std::vector<std::string> renderers;
                for (const auto& id : UsdImagingGLEngine::GetRendererPlugins())
                {
                    renderers.push_back(UsdImagingGLEngine::GetRendererDisplayName(id));
                }
                logSystem->print(
                    "tl::usd::Render",
                    ftk::Format(
                        "\n"
                        "    * Renderers: {0}").
                    arg(ftk::join(renderers, ", ")));
            }
        }

        Render::Render() :
            _p(new Private)
        {}

        Render::~Render()
        {
            FTK_P();
            p.thread.running = false;
            if (p.thread.thread.joinable())
            {
                p.thread.thread.join();
            }
            if (p.sdlGLContext)
            {
                SDL_GL_DestroyContext(p.sdlGLContext);
            }
            if (p.sdlWindow)
            {
                SDL_DestroyWindow(p.sdlWindow);
            }
        }

        std::shared_ptr<Render> Render::create(
            const std::shared_ptr<ftk::LogSystem>& logSystem)
        {
            auto out = std::shared_ptr<Render>(new Render);
            out->_init(logSystem);
            return out;
        }
        
        namespace
        {
            std::atomic<int64_t> requestID = 0;
        }

        InfoRequest Render::getInfo(
            const std::string& path,
            const Options& options)
        {
            FTK_P();
            auto request = std::make_shared<Private::InfoRequest>();
            request->id = ++requestID;
            request->path = path;
            request->options = options;
            InfoRequest out;
            out.id = request->id;
            out.future = request->promise.get_future();
            bool valid = false;
            {
                std::unique_lock<std::mutex> lock(p.mutex.mutex);
                if (!p.mutex.stopped)
                {
                    valid = true;
                    p.mutex.infoRequests.push_back(request);
                }
            }
            if (valid)
            {
                p.thread.cv.notify_one();
            }
            else
            {
                request->promise.set_value(Info());
            }
            return out;
        }

        Request Render::render(
            const std::string& path,
            const OTIO_NS::RationalTime& time,
            const Options& options)
        {
            FTK_P();
            auto request = std::make_shared<Private::Request>();
            request->id = ++requestID;
            request->path = path;
            request->time = time;
            request->options = options;
            Request out;
            out.id = request->id;
            out.future = request->promise.get_future();
            bool valid = false;
            {
                std::unique_lock<std::mutex> lock(p.mutex.mutex);
                if (!p.mutex.stopped)
                {
                    valid = true;
                    p.mutex.requests.push_back(request);
                }
            }
            if (valid)
            {
                p.thread.cv.notify_one();
            }
            else
            {
                request->promise.set_value(nullptr);
            }
            return out;
        }
        
        void Render::cancelRequests(int64_t id)
        {
            FTK_P();
            std::list<std::shared_ptr<Private::InfoRequest> > infoRequests;
            std::list<std::shared_ptr<Private::Request> > requests;
            {
                std::unique_lock<std::mutex> lock(p.mutex.mutex);
                auto i = p.mutex.infoRequests.begin();
                while (i != p.mutex.infoRequests.end())
                {
                    if (id == (*i)->id)
                    {
                        infoRequests.push_back(*i);
                        i = p.mutex.infoRequests.erase(i);
                    }
                    else
                    {
                        ++i;
                    }
                }
                auto j = p.mutex.requests.begin();
                while (j != p.mutex.requests.end())
                {
                    if (id == (*j)->id)
                    {
                        requests.push_back(*j);
                        j = p.mutex.requests.erase(j);
                    }
                    else
                    {
                        ++j;
                    }
                }
            }
            for (auto& request : infoRequests)
            {
                request->promise.set_value(Info());
            }
            for (auto& request : requests)
            {
                request->promise.set_value(nullptr);
            }
        }
                        
        namespace
        {
            UsdGeomCamera getCamera(
                const UsdStageRefPtr& stage,
                const std::string& name = std::string())
            {
                UsdGeomCamera out;
                if (!name.empty())
                {
                    out = UsdAppUtilsGetCameraAtPath(stage, SdfPath(name));
                }
                if (!out)
                {
                    const TfToken primaryCameraName = UsdUtilsGetPrimaryCameraName();
                    out = UsdAppUtilsGetCameraAtPath(stage, SdfPath(primaryCameraName));
                }
                if (!out)
                {
                    for (const auto& prim : stage->Traverse())
                    {
                        if (prim.IsA<UsdGeomCamera>())
                        {
                            out = UsdGeomCamera(prim);
                            break;
                        }
                    }
                }
                return out;
            }

            GfCamera getCameraToFrameStage(
                const UsdStagePtr& stage,
                UsdTimeCode timeCode,
                const TfTokenVector& includedPurposes)
            {
                GfCamera gfCamera;
                UsdGeomBBoxCache bboxCache(timeCode, includedPurposes, true);
                const GfBBox3d bbox = bboxCache.ComputeWorldBound(stage->GetPseudoRoot());
                const GfVec3d center = bbox.ComputeCentroid();
                const GfRange3d range = bbox.ComputeAlignedRange();
                const GfVec3d dim = range.GetSize();
                const TfToken upAxis = UsdGeomGetStageUpAxis(stage);

                GfVec2d planeCorner;
                if (upAxis == UsdGeomTokens->y)
                {
                    planeCorner = GfVec2d(dim[0], dim[1]) / 2;
                }
                else
                {
                    planeCorner = GfVec2d(dim[0], dim[2]) / 2;
                }
                const float planeRadius = sqrt(GfDot(planeCorner, planeCorner));

                const float halfFov = gfCamera.GetFieldOfView(GfCamera::FOVHorizontal) / 2.0;
                float distance = planeRadius / tan(GfDegreesToRadians(halfFov));

                if (upAxis == UsdGeomTokens->y)
                {
                    distance += dim[2] / 2;
                }
                else
                {
                    distance += dim[1] / 2;
                }

                GfMatrix4d xf;
                if (upAxis == UsdGeomTokens->y)
                {
                    xf.SetTranslate(center + GfVec3d(0, 0, distance));
                } else
                {
                    xf.SetRotate(GfRotation(GfVec3d(1, 0, 0), 90));
                    xf.SetTranslateOnly(center + GfVec3d(0, -distance, 0));
                }
                gfCamera.SetTransform(xf);
                return gfCamera;
            }
            
            UsdImagingGLDrawMode toUSD(DrawMode value)
            {
                const std::vector<UsdImagingGLDrawMode> data =
                {
                    UsdImagingGLDrawMode::DRAW_POINTS,
                    UsdImagingGLDrawMode::DRAW_WIREFRAME,
                    UsdImagingGLDrawMode::DRAW_WIREFRAME_ON_SURFACE,
                    UsdImagingGLDrawMode::DRAW_SHADED_FLAT,
                    UsdImagingGLDrawMode::DRAW_SHADED_SMOOTH,
                    UsdImagingGLDrawMode::DRAW_GEOM_ONLY,
                    UsdImagingGLDrawMode::DRAW_GEOM_FLAT,
                    UsdImagingGLDrawMode::DRAW_GEOM_SMOOTH
                };
                return data[static_cast<size_t>(value)];
            };
        }

        void Render::_open(
            const std::string& path,
            UsdStageRefPtr& stage,
            std::shared_ptr<UsdImagingGLEngine>& engine)
        {
            FTK_P();
            stage = UsdStage::Open(path);
            const bool gpuEnabled = true;
            engine = std::make_shared<UsdImagingGLEngine>(HdDriver(), TfToken(), gpuEnabled);
            if (stage && engine)
            {
                if (auto logSystem = p.logSystem.lock())
                {
                    const std::string renderer =
                        UsdImagingGLEngine::GetRendererDisplayName(
                            engine->GetCurrentRendererId());
                    std::vector<std::string> aovs;
                    for (const auto& i : engine->GetRendererAovs())
                    {
                        aovs.push_back(i.GetText());
                    }
                    logSystem->print(
                        "tl::usd::Render",
                        ftk::Format(
                            "\n"
                            "    * Path: {0}\n"
                            "    * Time code: {1}-{2}:{3}\n"
                            "    * GPU enabled: {4}\n"
                            "    * Renderer ID: {5}\n"
                            "    * Renderer AOVs available: {6}").
                        arg(path).
                        arg(stage->GetStartTimeCode()).
                        arg(stage->GetEndTimeCode()).
                        arg(stage->GetTimeCodesPerSecond()).
                        arg(engine->GetGPUEnabled()).
                        arg(renderer).
                        arg(ftk::join(aovs, ", ")));
                }
            }
        }
        
        void Render::_run()
        {
            FTK_P();
                        
            TfDiagnosticMgr::GetInstance().SetQuiet(true);

            const TfTokenVector purposes({ UsdGeomTokens->default_, UsdGeomTokens->proxy });

            size_t stageCacheCount = 10;
            while (p.thread.running)
            {
                // Check requests.
                std::shared_ptr<Private::InfoRequest> infoRequest;
                std::shared_ptr<Private::Request> request;
                {
                    std::unique_lock<std::mutex> lock(p.mutex.mutex);
                    if (p.thread.cv.wait_for(
                        lock,
                        std::chrono::milliseconds(5),
                        [this]
                        {
                            return
                                !_p->mutex.infoRequests.empty() ||
                                !_p->mutex.requests.empty();
                        }))
                    {
                        if (!p.mutex.infoRequests.empty())
                        {
                            infoRequest = p.mutex.infoRequests.front();
                            p.mutex.infoRequests.pop_front();
                        }
                        else if (!p.mutex.requests.empty())
                        {
                            request = p.mutex.requests.front();
                            p.mutex.requests.pop_front();
                        }
                    }
                }

                // Set options.
                Options options;
                if (infoRequest)
                {
                    options = infoRequest->options;
                }
                else if (request)
                {
                    options = request->options;
                }
                p.thread.stageCache.setMax(options.stageCacheCount);

                // Handle information requests.
                std::string cameraName = options.cameraName;
                if (infoRequest)
                {
                    const std::string path = infoRequest->path;
                    Private::StageCacheItem stageCacheItem;
                    if (!p.thread.stageCache.get(path, stageCacheItem))
                    {
                        _open(path, stageCacheItem.stage, stageCacheItem.engine);
                        p.thread.stageCache.add(path, stageCacheItem);
                    }
                    Info info;
                    if (stageCacheItem.stage)
                    {
                        const double startTimeCode = stageCacheItem.stage->GetStartTimeCode();
                        const double endTimeCode = stageCacheItem.stage->GetEndTimeCode();
                        const double timeCodesPerSecond = stageCacheItem.stage->GetTimeCodesPerSecond();
                        info.timeRange = OTIO_NS::TimeRange::range_from_start_end_time_inclusive(
                            OTIO_NS::RationalTime(startTimeCode, timeCodesPerSecond),
                            OTIO_NS::RationalTime(endTimeCode, timeCodesPerSecond));
                        //std::cout << path << " range: " << info.timeRange << std::endl;
                    }
                    infoRequest->promise.set_value(info);
                }

                // Handle requests.
                if (request)
                {
                    std::shared_ptr<ftk::Image> image;
                    try
                    {
                        // Check the stage cache for a previously opened stage.
                        const std::string path = request->path;
                        Private::StageCacheItem stageCacheItem;
                        if (!p.thread.stageCache.get(path, stageCacheItem))
                        {
                            _open(path, stageCacheItem.stage, stageCacheItem.engine);
                            p.thread.stageCache.add(path, stageCacheItem);
                        }
                        if (stageCacheItem.stage && stageCacheItem.engine)
                        {
                            const double timeCode = request->time.rescaled_to(
                                stageCacheItem.stage->GetTimeCodesPerSecond()).value();
                            //std::cout << path << " timeCode: " << timeCode << std::endl;

                            // Setup the camera.
                            GfCamera gfCamera;
                            auto camera = getCamera(stageCacheItem.stage, cameraName);
                            if (camera)
                            {
                                gfCamera = camera.GetCamera(timeCode);
                            }
                            else
                            {
                                gfCamera = getCameraToFrameStage(stageCacheItem.stage, timeCode, purposes);
                            }
                            const GfFrustum frustum = gfCamera.GetFrustum();
                            const GfVec3d cameraPos = frustum.GetPosition();
                            stageCacheItem.engine->SetCameraState(
                                frustum.ComputeViewMatrix(),
                                frustum.ComputeProjectionMatrix());
                            float aspectRatio = gfCamera.GetAspectRatio();
                            if (GfIsClose(aspectRatio, 0.F, 1e-4))
                            {
                                aspectRatio = 1.F;
                            }
                            const size_t renderHeight = options.renderWidth / aspectRatio;
                            stageCacheItem.engine->SetRenderViewport(GfVec4d(
                                0.0,
                                0.0,
                                static_cast<double>(options.renderWidth),
                                static_cast<double>(renderHeight)));

                            //for (const auto& token : stageCacheItem.engine->GetRendererAovs())
                            //{
                            //    std::cout << token.GetText() << std::endl;
                            //}
                            stageCacheItem.engine->SetRendererAov(HdAovTokens->color);

                            // Setup a light.
                            GlfSimpleLight cameraLight(
                                GfVec4f(cameraPos[0], cameraPos[1], cameraPos[2], 1.F));
                            cameraLight.SetAmbient(GfVec4f(.01F, .01F, .01F, 01.F));
                            const GlfSimpleLightVector lights({ cameraLight });

                            // Setup a material.
                            GlfSimpleMaterial material;
                            material.SetAmbient(GfVec4f(0.2f, 0.2f, 0.2f, 1.0));
                            material.SetSpecular(GfVec4f(0.1f, 0.1f, 0.1f, 1.0f));
                            material.SetShininess(32.F);
                            const GfVec4f ambient(0.01f, 0.01f, 0.01f, 1.0f);
                            stageCacheItem.engine->SetLightingState(lights, material, ambient);

                            // Render the frame.
                            UsdImagingGLRenderParams renderParams;
                            renderParams.frame = timeCode;
                            renderParams.complexity = options.complexity;
                            renderParams.drawMode = toUSD(options.drawMode);
                            renderParams.enableLighting = options.enableLighting;
                            renderParams.clearColor = GfVec4f(0.F, 0.F, 0.F, 0.F);
                            renderParams.colorCorrectionMode = options.sRGB ?
                                HdxColorCorrectionTokens->sRGB :
                                HdxColorCorrectionTokens->disabled;
                            const UsdPrim& pseudoRoot = stageCacheItem.stage->GetPseudoRoot();
                            unsigned int sleepTime = 10;
                            while (p.thread.running)
                            {
                                stageCacheItem.engine->Render(pseudoRoot, renderParams);
                                if (stageCacheItem.engine->IsConverged())
                                {
                                    break;
                                }
                                else
                                {
                                    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
                                    sleepTime = std::min(100u, sleepTime + 5);
                                }
                            }

                            // Copy the rendered frame.
                            if (stageCacheItem.engine->GetGPUEnabled())
                            {
                                const auto colorTextureHandle = stageCacheItem.engine->GetAovTexture(HdAovTokens->color);
                                if (colorTextureHandle)
                                {
                                    size_t size = 0;
                                    const auto mappedColorTextureBuffer = HdStTextureUtils::HgiTextureReadback(
                                        stageCacheItem.engine->GetHgi(),
                                        colorTextureHandle,
                                        &size);
                                    //std::cout << colorTextureHandle->GetDescriptor().format << std::endl;
                                    switch (HdxGetHioFormat(colorTextureHandle->GetDescriptor().format))
                                    {
                                    case HioFormat::HioFormatFloat16Vec4:
                                        image = ftk::Image::create(
                                            options.renderWidth,
                                            renderHeight,
                                            ftk::ImageType::RGBA_F16);
                                        memcpy(image->getData(), mappedColorTextureBuffer.get(), image->getInfo().getByteCount());
                                        break;
                                    default: break;
                                    }
                                }
                            }
                            else
                            {
                                const auto colorRenderBuffer = stageCacheItem.engine->GetAovRenderBuffer(HdAovTokens->color);
                                if (colorRenderBuffer)
                                {
                                    colorRenderBuffer->Resolve();
                                    colorRenderBuffer->Map();
                                    switch (HdStHioConversions::GetHioFormat(colorRenderBuffer->GetFormat()))
                                    {
                                    case HioFormat::HioFormatFloat16Vec4:
                                        image = ftk::Image::create(
                                            options.renderWidth,
                                            renderHeight,
                                            ftk::ImageType::RGBA_F16);
                                        memcpy(image->getData(), colorRenderBuffer->Map(), image->getInfo().getByteCount());
                                        break;
                                    default: break;
                                    }
                                }
                            }
                        }
                    }
                    catch (const std::exception& e)
                    {
                        //std::cout << e.what() << std::endl;
                        if (auto logSystem = p.logSystem.lock())
                        {
                            const std::string id = ftk::Format("tl::usd::Render ({0}: {1})").
                                arg(__FILE__).
                                arg(__LINE__);
                            logSystem->print(id, e.what(), ftk::LogType::Error);
                        }
                    }

                    request->promise.set_value(image);
                }

                // Logging.
                {
                    const auto now = std::chrono::steady_clock::now();
                    const std::chrono::duration<float> diff = now - p.thread.logTimer;
                    if (diff.count() > 10.F)
                    {
                        p.thread.logTimer = now;
                        if (auto logSystem = p.logSystem.lock())
                        {
                            size_t requestsSize = 0;
                            {
                                std::unique_lock<std::mutex> lock(p.mutex.mutex);
                                requestsSize = p.mutex.requests.size();
                            }
                            logSystem->print(
                                "tl::usd::Render",
                                ftk::Format(
                                    "\n"
                                    "    * Requests: {0}\n"
                                    "    * Stage cache: {1}/{2}").
                                arg(requestsSize).
                                arg(p.thread.stageCache.getSize()).
                                arg(p.thread.stageCache.getMax()));
                        }
                    }
                }
            }
        }

        void Render::_finish()
        {
            FTK_P();
            std::list<std::shared_ptr<Private::InfoRequest> > infoRequests;
            std::list<std::shared_ptr<Private::Request> > requests;
            {
                std::unique_lock<std::mutex> lock(p.mutex.mutex);
                p.mutex.stopped = true;
                infoRequests = std::move(p.mutex.infoRequests);
                requests = std::move(p.mutex.requests);
            }
            for (auto& request : infoRequests)
            {
                request->promise.set_value(Info());
            }
            for (auto& request : requests)
            {
                request->promise.set_value(nullptr);
            }
        }
    }
}

