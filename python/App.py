# SPDX-License-Identifier: BSD-3-Clause
# Copyright Contributors to the feather-tk project.

import opentimelineio as otio
import ftkPy as ftk
import ibisPy as ibis

import MainWindow

import pathlib
import weakref

class App(ftk.App):
    """
    Application.
    """
    def __init__(self, context, argv):
                
        ftk.App.__init__(
            self,
            context,
            argv,
            "ibis",
            "ibis compositor")

    def run(self):

        self._recentFilesModel = ftk.RecentFilesModel(self.context)

        self._documentModel = ibis.models.DocumentModel(self.context)
        self._documentModel.newDocument()
        self._documentModel.newDocument()

        self._window = MainWindow.MainWindow(self.context, self)

        super().run()

    def recentFilesModel(self):
        return self._recentFilesModel

    def documentModel(self):
        return self._documentModel
