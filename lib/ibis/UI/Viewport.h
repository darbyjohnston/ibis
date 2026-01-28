// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ftk/UI/IWidget.h>

namespace ibis
{
    namespace models
    {
        class Document;
    }

    namespace ui
    {
        //! Viewport.
        class Viewport : public ftk::IWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::Document>&,
                const std::shared_ptr<ftk::IWidget>& parent);

            Viewport();

        public:
            virtual ~Viewport();

            //! Create a new widget.
            static std::shared_ptr<Viewport> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<models::Document>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            //! \name View
            ///@{

            //! Get the view position.
            const ftk::V2I& getViewPos() const;

            //! Observe the view position.
            std::shared_ptr<ftk::IObservable<ftk::V2I> > observeViewPos() const;

            //! Get the view zoom.
            double getViewZoom() const;

            //! Observe the view zoom.
            std::shared_ptr<ftk::IObservable<double> > observeViewZoom() const;

            //! Get the view position and zoom.
            std::pair<ftk::V2I, double> getViewPosAndZoom() const;

            //! Observe the view position and zoom.
            std::shared_ptr<ftk::IObservable<std::pair<ftk::V2I, double> > > observeViewPosAndZoom() const;

            //! Set the view position and zoom.
            void setViewPosAndZoom(const ftk::V2I&, double);

            //! Set the view zoom.
            void setViewZoom(double, const ftk::V2I& focus = ftk::V2I());

            //! Get whether the view is framed automatically.
            bool hasFrameView() const;

            //! Observe whether the view is framed automatically.
            std::shared_ptr<ftk::IObservable<bool> > observeFrameView() const;

            //! Observe when the view is framed.
            std::shared_ptr<ftk::IObservable<bool> > observeFramed() const;

            //! Set whether the view is framed automatically.
            void setFrameView(bool);

            //! Reset the view zoom to 1:1.
            void viewZoomReset();

            //! Zoom the view in.
            void viewZoomIn();

            //! Zoom the view out.
            void viewZoomOut();

            ///@}

            //! \name Color Sample
            ///@{

            //! Sample a color from the viewport.
            ftk::Color4F getColorSample(const ftk::V2I&);

            ///@}

            //! \name Input
            ///@{

            //! Set the mouse wheel scale.
            void setMouseWheelScale(float);

            ///@}

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;
            void sizeHintEvent(const ftk::SizeHintEvent&) override;
            void drawEvent(const ftk::Box2I&, const ftk::DrawEvent&) override;
            void mouseEnterEvent(ftk::MouseEnterEvent&) override;
            void mouseLeaveEvent() override;
            void mouseMoveEvent(ftk::MouseMoveEvent&) override;
            void mousePressEvent(ftk::MouseClickEvent&) override;
            void mouseReleaseEvent(ftk::MouseClickEvent&) override;
            void scrollEvent(ftk::ScrollEvent&) override;
            void keyPressEvent(ftk::KeyEvent&) override;
            void keyReleaseEvent(ftk::KeyEvent&) override;

        private:
            bool _isMouseInside() const;
            const ftk::V2I& _getMousePressPos() const;
            ftk::V2I _getViewportCenter() const;
            void _frameView();

            FTK_PRIVATE();
        };
    }
}
