# SPDX-License-Identifier: BSD-3-Clause
# Copyright Contributors to the feather-tk project.

import opentimelineio as otio
import ftkPy as ftk
import ibisPy as ibis

import os
import weakref

class Menu(ftk.Menu):
    """
    File menu.
    """
    def __init__(self, context, app, actions, parent = None):
        ftk.Menu.__init__(self, context, parent)

        self._app = weakref.ref(app)
        
        self.addAction(actions.actions["New"])
        self.addAction(actions.actions["Open"])
        self.addAction(actions.actions["Close"])
        self.addAction(actions.actions["CloseAll"])
        self.recentMenu = self.addSubMenu("Recent")
        self.addDivider();
        self.addAction(actions.actions["Exit"])
        
        selfWeak = weakref.ref(self)
        self.recentObserver = ftk.PathListObserver(
            app.recentFilesModel().observeRecent,
            lambda recentList: selfWeak()._recentUpdate(recentList))

    def _recentCallback(self, recent):
        if (self._app):
            self._app().documentModel().open(ftk.Path(str(recent)))

    def _recentUpdate(self, recentList):
        self.recentMenu.clear()
        for recent in reversed(recentList):
            action = ftk.Action(
                os.path.basename(recent),
                lambda captured = recent: self._recentCallback(captured))
            self.recentMenu.addAction(action)
