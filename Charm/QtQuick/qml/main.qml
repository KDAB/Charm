/*
  main.qml

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Frank Osterfeld <frank.osterfeld@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Charm")

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                id: importDatabse
                text: qsTr("Import database from previous export ...")
            }
            MenuItem {
                id: exportDatabse
                text: qsTr("Export database ...")
            }
            MenuItem {
                id: downloadTaskDefinitions
                text: qsTr("Download Task Definitions ...")
            }

            // do we still need the following two menu entries on a mobile device ?
//            MenuItem {
//                id: importTaskDefinitions
//                text: qsTr("Import and Merge Task Definitions ...")
//            }
//            MenuItem {
//                id: exportTaskDefinitions
//                text: qsTr("Export and Merge Task Definitions ...")
//            }

            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
            }
        }
        Menu {
            title: qsTr("Edit")
            MenuItem {
                id: stopTask
                text: qsTr("Stop task")
            }
            MenuItem {
                id: editComment
                text: qsTr("Edit comment")
            }
            MenuItem {
                id: startTask
                text: qsTr("Start other task")
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 2
        TableView {
            id: tableView
            Layout.fillHeight: true
            Layout.fillWidth: true

            TableViewColumn{ role: "task"  ; title: qsTr("Task"); width: tableView.width - 8 * tableColumn.width; movable: false }
            TableViewColumn{ role: "monday" ; title: qsTr("Mon"); movable: false; horizontalAlignment: Text.AlignRight; id: tableColumn }
            TableViewColumn{ role: "tuesday" ; title: qsTr("Tue"); movable: false; horizontalAlignment: Text.AlignRight }
            TableViewColumn{ role: "wednesday" ; title: qsTr("Wed"); movable: false; horizontalAlignment: Text.AlignRight }
            TableViewColumn{ role: "thursday" ; title: qsTr("Thu"); movable: false; horizontalAlignment: Text.AlignRight }
            TableViewColumn{ role: "friday" ; title: qsTr("Fri"); movable: false; horizontalAlignment: Text.AlignRight }
            TableViewColumn{ role: "saturday" ; title: qsTr("Sat"); movable: false; horizontalAlignment: Text.AlignRight }
            TableViewColumn{ role: "sunday" ; title: qsTr("Sun"); movable: false; horizontalAlignment: Text.AlignRight }
            TableViewColumn{ role: "total" ; title: qsTr("Total"); movable: false; horizontalAlignment: Text.AlignRight }

            model: ListModel {
                ListElement{
                    task: "8714 IT Infrastructure/Internal karm/charm development"
                    wednesday: "5.30"
                }
                ListElement{
                    task: "8914 Qt Contributions/Android"
                    monday: "1.50"
                    tuesday: "4.00"
                    wednesday: "2.00"
                }
            }
        }

        ToolBar {
            id: toolBar
            anchors.bottom: parent.bottom
            Layout.fillWidth: true

            RowLayout {
                anchors.fill: parent
                Button {
                    id: stopTaskButton
                    text: qsTr("Stop task")
                    onClicked: stopTask.trigger()
                }

                Button {
                    id: editCommentButton
                    text: qsTr("Edit comment")
                    onClicked: editComment.trigger()
                }

                ComboBox {
                    Layout.fillWidth: true
                    model: [ "8714 IT Infrastructure/Internal karm/charm development", "8914 Qt Contributions/Android" ]
                }
            }
        }
    }
}
