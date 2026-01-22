# SPDX-License-Identifier: BSD-3-Clause
# Copyright Contributors to the feather-tk project.

import opentimelineio as otio
import ftkPy as ftk
import ibisPy as ibis

import weakref

class Widget(ftk.IWidget):
    """
    Document widget.
    """
    def __init__(self, context, document):
        ftk.IWidget.__init__(self, context, "DocumentWidget.Widget")

        self._nodeBrowser = ibis.ui.NodeBrowser(context)

        self._nodeEditor = ibis.ui.NodeEditor(context)

        self._viewport = ibis.ui.Viewport(context)

        self._nodeGraphCanvas = ibis.ui.NodeGraphCanvas(context)

        self._timelineWidget = ibis.ui.TimelineWidget(context)
        
        self._hSplitter = ftk.Splitter(context, ftk.Orientation.Horizontal, self)
        self._hSplitter.split = 0.75

        self._layout = ftk.VerticalLayout(context, self._hSplitter)
        self._layout.spacingRole = ftk.SizeRole._None
        self._splitter = ftk.Splitter(context, ftk.Orientation.Vertical, self._layout)
        self._splitter.split = 0.7
        self._viewport.parent = self._splitter
        self._nodeGraphCanvas.parent = self._splitter
        ftk.Divider(context, ftk.Orientation.Vertical, self._layout)
        self._timelineWidget.parent = self._layout

        self._tabWiget = ftk.TabWidget(context, self._hSplitter)
        self._tabWiget.addTab("Browser", self._nodeBrowser)
        self._tabWiget.addTab("Editor", self._nodeEditor)

    def getSizeHint(self):
        return self._hSplitter.getSizeHint()

    def setGeometry(self, value):
        ftk.IWidget.setGeometry(self, value)
        self._hSplitter.setGeometry(value)
