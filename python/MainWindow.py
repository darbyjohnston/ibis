# SPDX-License-Identifier: BSD-3-Clause
# Copyright Contributors to the feather-tk project.

from tkinter import CURRENT
import opentimelineio as otio
import ftkPy as ftk
import ibisPy as ibis

import DocumentWidget
import FileActions
import FileMenu

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

        self._fileActions = FileActions.Actions(context, app, self)

        self._menuBar = ftk.MenuBar(context)
        self._menuBar.addMenu("File", FileMenu.Menu(context, app, self._fileActions))
        self.menuBar = self._menuBar

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
        self._tabBar.setCurrentTabCallback(lambda index: selfWeak()._currentTabCallback(index))
        self._tabBar.setTabCloseCallback(lambda index: selfWeak()._tabCloseCallback(index))

        self.documentsObserver = ibis.models.DocumentListObserver(
            app.documentModel().observe,
            lambda documents: selfWeak()._documentsCallback(documents))

        self.newDocumentObserver = ftk.IntObserver(
            app.documentModel().observeNewDocument,
            lambda index: selfWeak()._newDocumentCallback(index))

    def currentTab(self):
        return self._tabBar.currentTab

    def _currentTabCallback(self, index):
        self._stackLayout.currentIndex = index

    def _tabCloseCallback(self, index):
        self.app.documentModel().close(index)

    def _documentsCallback(self, documents):

        currentWidget = self._stackLayout.currentWidget
        self._tabBar.clear()
        self._stackLayout.clear()

        widgets = {}
        for document in documents:

            fileName = Path(document.path).name
            self._tabBar.addTab(fileName, str(document.path))

            widget = self._widgets.get(document)
            if widget is None:
                widget = DocumentWidget.Widget(self.context, document)
            widgets[document] = widget
            widget.parent = self._stackLayout

        self._documents = documents
        self._widgets = widgets
        self._stackLayout.currentWidget = currentWidget

    def _newDocumentCallback(self, index):
        self._tabBar.currentTab = index
        self._stackLayout.currentIndex = index
