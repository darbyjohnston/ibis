# SPDX-License-Identifier: BSD-3-Clause
# Copyright Contributors to the feather-tk project.

import opentimelineio as otio
import ftkPy as ftk
import ibisPy as ibis

import DocumentTab

import weakref

class MainWindow(ftk.MainWindow):
    """
    Main window.
    """
    def __init__(self, context, app):
        ftk.MainWindow.__init__(self, context, app, ftk.Size2I(1280, 960))

        self._docTab = DocumentTab.DocumentTab(context, app)
        self.widget = self._docTab
