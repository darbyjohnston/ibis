# SPDX-License-Identifier: BSD-3-Clause
# Copyright Contributors to the feather-tk project.

import opentimelineio as otio
import ftkPy as ftk
import ibisPy as ibis

import DocumentWidget

import weakref
from pathlib import Path

class MainWindow(ftk.MainWindow):
    """
    Main window.
    """
    def __init__(self, context, app):
        ftk.MainWindow.__init__(self, context, app, ftk.Size2I(1700, 960))

        self._documents = []
        self._widgets = {}

        self._tabBar = ftk.TabBar(context)
        self._tabBar.tabsClosable = True

        self._stackLayout = ftk.StackLayout(context)
        self._stackLayout.vStretch = ftk.Stretch.Expanding

        self._layout = ftk.VerticalLayout(context)
        self._layout.spacingRole = ftk.SizeRole._None
        self._tabBar.parent = self._layout
        ftk.Divider(context, ftk.Orientation.Vertical, self._layout)
        self._stackLayout.parent = self._layout
        self.widget = self._layout

        selfWeak = weakref.ref(self)
        self._tabBar.setCallback(lambda index: selfWeak()._tabCallback(index))

        self.documentsObserver = ibis.models.DocumentListObserver(
            app.documentModel().observeDocuments,
            lambda documents: selfWeak()._documentsUpdate(documents))

    def _tabCallback(self, index):
        self._stackLayout.currentIndex = index

    def _documentsUpdate(self, documents):
        
        self._tabBar.clear()
        self._stackLayout.clear()

        widgets = {}
        for document in documents:

            fileName = Path(document.path).name
            self._tabBar.addTab(fileName, str(document.path))

            widget = self._widgets.get(document)
            if widget is None:
                widget = DocumentWidget.DocumentWidget(self.context, document)
            widgets[document] = widget
            widget.parent = self._stackLayout

        self._documents = documents
        self._widgets = widgets
