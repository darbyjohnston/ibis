# SPDX-License-Identifier: BSD-3-Clause
# Copyright Contributors to the feather-tk project.

import opentimelineio as otio
import ftkPy as ftk
import ibisPy as ibis

import weakref

class Actions:
    """
    File actions.
    """
    def __init__(self, context, app, mainWindow):

        appWeak = weakref.ref(app)
        mainWindowWeak = weakref.ref(mainWindow)
        self.actions = {}

        self.actions["New"] = ftk.Action(
            "New",
            "FileNew",
            ftk.KeyShortcut(ftk.Key.N, ftk.commandKeyModifier),
            lambda: appWeak().documentModel().newDocument())
        self.actions["New"].tooltip = "Create a new file."

        self.actions["Open"] = ftk.Action(
            "Open",
            "FileOpen",
            ftk.KeyShortcut(ftk.Key.O, ftk.commandKeyModifier),
            lambda: context.getSystemByName("ftk::FileBrowserSystem").open(
                mainWindowWeak(),
                appWeak().documentModel().open))
        self.actions["Open"].tooltip = "Open a file."

        self.actions["Close"] = ftk.Action(
            "Close",
            "FileClose",
            ftk.KeyShortcut(ftk.Key.E, ftk.commandKeyModifier),
            lambda: appWeak().documentModel().close(mainWindowWeak().currentTab()))
        self.actions["Close"].tooltip = "Close the current file."

        self.actions["CloseAll"] = ftk.Action(
            "Close All",
            "FileCloseAll",
            ftk.KeyShortcut(ftk.Key.E, ftk.commandKeyModifier, ftk.KeyModifier.Shift),
            lambda: appWeak().documentModel().closeAll())
        self.actions["CloseAll"].tooltip = "Close all files."

        self.actions["Exit"] = ftk.Action(
            "Exit",
            ftk.KeyShortcut(ftk.Key.Q, ftk.commandKeyModifier),
            lambda: appWeak().exit())

        selfWeak = weakref.ref(self)
        self._documentsObserver = ibis.models.DocumentListObserver(
            app.documentModel().observe,
            lambda documents: selfWeak()._documentsCallback(documents))

    def _documentsCallback(self, documents):
        self.actions["Close"].enabled = len(documents) > 0
        self.actions["CloseAll"].enabled = len(documents) > 0
