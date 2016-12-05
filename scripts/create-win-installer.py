#!/usr/bin/env python
#  create-win-installer.py
#
#  This file is part of Charm, a task-based time tracking application.
#
#  Copyright (C) 2016-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
#
# Author: Hannah von Reth <hannah.vonreth@kdab.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

from __future__ import print_function

import os
import subprocess
import sys
import shutil
import argparse
import glob

class DeployHelper(object):
    def __init__(self, args):
        self.args = args
        self.gitDir = os.path.realpath(os.path.join(os.path.dirname(__file__), ".."))

        self.deployImage = os.path.realpath("./deployImage")
        if not os.path.exists(self.args.buildDir):
            self._die("Build dir %s does not exist" % self.args.buildDir)


    def _die(self, message):
        print(message, file=sys.stderr)
        exit(1)

    def _logExec(self, command):
        print(command)
        sys.stdout.flush()
        subprocess.check_call(command, shell=True)

    def _copyToImage(self, src, destDir = None, deploy = True):
        if destDir is None:
            destDir = self.deployImage
        print("Copy %s to %s" % (src, destDir))
        shutil.copy(src, destDir)
        if deploy:
            self._logExec("windeployqt --%s --dir \"%s\" --qmldir \"%s\"  \"%s\"" % (self.args.buildType, self.deployImage, self.gitDir, src))

    def _sign(self, fileName):
        if self.args.sign:
            self._logExec("signtool.exe sign  -t http://timestamp.globalsign.com/scripts/timestamp.dll  -fd SHA256 -v \"%s\"" % fileName)

    def cleanImage(self):
        if os.path.exists(self.deployImage):
            shutil.rmtree(self.deployImage)
        os.makedirs(self.deployImage)

    def _locateDll(self, dllName, fatal=True):
        pathext = os.environ.get("PATHEXT")
        os.environ["PATHEXT"] = ".dll"
        found = shutil.which(dllName, path=os.pathsep.join([self.deployImage, os.environ.get("PATH")]))
        os.environ["PATHEXT"] = pathext
        if not found and fatal:
            self._die("Unable to locate %s" % dllName)
        return found

    def deploy(self):
        if self.args.deployDlls:
            for dll in self.args.deployDlls.split(";"):
                self._copyToImage(self._locateDll(dll))

        app = os.path.join(self.args.buildDir, self.args.applicationFileName)
        shutil.copy(app, self.deployImage)
        self._logExec("windeployqt --%s --compiler-runtime --dir \"%s\" --qmldir \"%s\" \"%s\"" % (self.args.buildType, self.deployImage, self.gitDir, app))

        for folder in self.args.pluginFolders.split(";"):
            for f in glob.glob(os.path.join(self.args.buildDir, folder, "*.dll")):
                self._copyToImage(f)

        if self.args.deployOpenSSL:
            foundSomething = False
            for dll in ["libeay32.dll", "libssl32.dll", "ssleay32.dll" ]:
                src = os.path.join(self.args.deployOpenSSL, dll )
                if not os.path.exists(src):
                    src = self._locateDll(dll, fatal=False)
                if src:
                    foundSomething = True
                    print(src)
                    self._copyToImage(src, deploy=False)
            if not foundSomething:
                self._die("Failed to deploy openssl")

        if self.args.sign:
            for f in glob.glob(os.path.join(self.deployImage, "**/*.exe"), recursive=True):
                self._sign(f)
            for f in glob.glob(os.path.join(self.deployImage, "**/*.dll"), recursive=True):
                self._sign(f)

    def makeInstaller(self):
        if self.args.architecture == "x64":
            programDir = "$PROGRAMFILES64"
        else:
            programDir = "$PROGRAMFILES"

        defines = {}
        defines["productName"] = self.args.productName
        defines["companyName"] = self.args.companyName
        defines["productVersion"] = self.args.productVersion
        defines["setupname"] = self.args.installerName
        defines["applicationName"] = os.path.basename(self.args.applicationFileName)
        defines["applicationIcon"] = self.args.applicationIcon
        defines["programFilesDir"] = programDir
        defines["deployDir"] = self.deployImage
        defines["productLicence"] = "" if not self.args.productLicence else "!insertmacro MUI_PAGE_LICENSE \"%s\"" % self.args.productLicence

        redist = "vcredist_%s.exe" % self.args.architecture
        if os.path.exists(os.path.join(self.deployImage, redist)):
            defines["vcredist"] = "vcredist_%s.exe" % self.args.architecture
        else:
            defines["vcredist"] = "none"


        definestring = ""
        for key in defines:
            definestring += " /D%s=\"%s\"" % (key, defines[key])

        command = "makensis /NOCD %s %s" %\
                  (definestring, os.path.join(os.path.dirname(__file__), "NullsoftInstaller.nsi"))
        self._logExec(command)
        installer = os.path.realpath(self.args.installerName)
        self._sign(installer)
        print("""Generated package file: %s """ % installer)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--architecture", action = "store", default="x64" )
    parser.add_argument("--buildType", action = "store", default="release" )
    parser.add_argument("--installerName", action = "store" )
    parser.add_argument("--applicationFileName", action = "store" )
    parser.add_argument("--applicationIcon", action = "store" )
    parser.add_argument("--buildDir", action = "store" )
    parser.add_argument("--pluginFolders", action = "store", default="" )
    parser.add_argument("--productName", action = "store" )
    parser.add_argument("--companyName", action = "store" )
    parser.add_argument("--productVersion", action = "store" )
    parser.add_argument("--productLicence", action = "store" )
    parser.add_argument("--deployDlls", action = "store" )
    parser.add_argument("--deployOpenSSL", action = "store" )

    parser.add_argument("--sign", action = "store_true", default="False")


    args = parser.parse_args()

    helper = DeployHelper(args)
    helper.cleanImage()
    helper.deploy()
    helper.makeInstaller()
