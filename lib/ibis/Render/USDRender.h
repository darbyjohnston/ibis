// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

#include <ftk/Core/Image.h>
#include <ftk/Core/LogSystem.h>

#include <opentimelineio/version.h>

#include <pxr/usd/usd/stage.h>
#include <pxr/usdImaging/usdImagingGL/engine.h>

#include <future>

namespace ibis
{
    namespace usd
    {
        //! USD drawing mode.
        enum class DrawMode
        {
            Points,
            Wireframe,
            WireframeOnSurface,
            ShadedFlat,
            ShadedSmooth,
            GeomOnly,
            GeomFlat,
            GeomSmooth,

            Count,
            First = Points
        };
        FTK_ENUM(DrawMode);

        //! USD options.
        struct Options
        {
            int           renderWidth = 1920;
            std::string   cameraName;
            float         complexity = 1.F;
            usd::DrawMode drawMode = usd::DrawMode::ShadedSmooth;
            bool          enableLighting = true;
            bool          sRGB = true;
            size_t        stageCacheCount = 10;

            bool operator == (const Options&) const;
            bool operator != (const Options&) const;
        };

        //! USD file information.
        struct Info
        {
            OTIO_NS::TimeRange timeRange = OTIO_NS::TimeRange(
                OTIO_NS::RationalTime(0.0, 0.0),
                OTIO_NS::RationalTime(0.0, 0.0));
        };

        //! USD information request.
        struct InfoRequest
        {
            int64_t id = -1;
            std::future<Info> future;
        };

        //! USD render request.
        struct Request
        {
            int64_t id = -1;
            std::future<std::shared_ptr<ftk::Image> > future;
        };

        //! USD renderer.
        class Render : public std::enable_shared_from_this<Render>
        {
        protected:
            void _init(const std::shared_ptr<ftk::LogSystem>&);

            Render();

        public:
            ~Render();

            //! Create a new renderer.
            static std::shared_ptr<Render> create(
                const std::shared_ptr<ftk::LogSystem>&);
            
            //! Get information.
            InfoRequest getInfo(
                const std::string& path,
                const Options& = Options());
            
            //! Render an image.
            Request render(
                const std::string& path,
                const OTIO_NS::RationalTime& time,
                const Options& = Options());

            //! Cancel requests.
            void cancelRequests(int64_t id);

        private:
            void _open(
                const std::string&,
                PXR_NS::UsdStageRefPtr&,
                std::shared_ptr<PXR_NS::UsdImagingGLEngine>&);
            void _run();
            void _finish();

            FTK_PRIVATE();
        };
    }
}
