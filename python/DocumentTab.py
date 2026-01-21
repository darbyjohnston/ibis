# SPDX-License-Identifier: BSD-3-Clause
# Copyright Contributors to the feather-tk project.

import opentimelineio as otio
import ftkPy as ftk
import ibisPy as ibis

import weakref

class DocumentTab(ftk.IWidget):
    """
    Document tab.
    """
    def __init__(self, context, app):
        ftk.IWidget.__init__(self, context, "DocumentTab")

        self._timelineWidget = ibis.ui.TimelineWidget(context)

        self._splitter = ftk.Splitter(context, ftk.Orientation.Vertical, self)
        self._timelineWidget.parent = self._splitter

    def getSizeHint(self):
        return self._splitter.getSizeHint()

    def setGeometry(self, value):
        ftk.IWidget.setGeometry(self, value)
        self._splitter.setGeometry(value)
