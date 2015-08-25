/*
  CharmCommandSession.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2015-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Guillermo A. Amaral <gamaral@kdab.com>

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

#include "CharmCommandSession.h"

#include <QBuffer>
#include <QIODevice>
#include <QStringList>

#include "Core/CharmDataModel.h"

#include "ViewHelpers.h"

#include "CharmCommandProtocol.h"
#include "CharmCMake.h"

#ifndef CHARM_CI_SUPPORT
#error Build system error: CHARM_CI_SUPPORT should be defined
#endif

CharmCommandSession::CharmCommandSession(QObject* parent)
    : QObject(parent)
    , m_device(nullptr)
    , m_state(InvalidState)
{
    qDebug("Command interface created.");

    DATAMODEL->registerAdapter(this);
}

CharmCommandSession::~CharmCommandSession()
{
    DATAMODEL->unregisterAdapter(this);

    qDebug("Command interface destroyed.");
}

QIODevice* CharmCommandSession::device() const
{
    return m_device;
}

void CharmCommandSession::setDevice(QIODevice* device)
{
    if (m_device)
        disconnect(m_device, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

    m_device = device;

    if (m_device)
        connect(m_device, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

    reset();
}

void CharmCommandSession::resetTasks()
{
    if (!m_device)
        return;

    m_device->write(CHARM_CI_EVENT_TASK_RESET);
    m_device->write("\n");
}

void CharmCommandSession::taskAdded( TaskId id )
{
    if (!m_device)
        return;

    m_device->write(QString("%1 %2\n")
        .arg(CHARM_CI_EVENT_TASK_ADDED)
        .arg(DATAMODEL->taskIdAndSmartNameString(id))
        .toLatin1());
}

void CharmCommandSession::taskModified( TaskId id )
{
    if (!m_device)
        return;

    m_device->write(QString("%1 %2\n")
        .arg(CHARM_CI_EVENT_TASK_MODIFIED)
        .arg(DATAMODEL->taskIdAndSmartNameString(id))
        .toLatin1());
}

void CharmCommandSession::eventActivated( EventId id )
{
    if (!m_device)
        return;

    const Event &event = DATAMODEL->eventForId(id);
    m_device->write(QString("%1 %2\n")
        .arg(CHARM_CI_EVENT_TASK_ACTIVATED)
        .arg(DATAMODEL->taskIdAndSmartNameString(event.taskId()))
        .toLatin1());
}

void CharmCommandSession::eventDeactivated( EventId id )
{
    if (!m_device)
        return;

    const Event &event = DATAMODEL->eventForId(id);
    m_device->write(QString("%1 %2\n")
        .arg(CHARM_CI_EVENT_TASK_DEACTIVATED)
        .arg(DATAMODEL->taskIdAndSmartNameString(event.taskId()))
        .toLatin1());
}

void CharmCommandSession::reset()
{
    m_state = InvalidState;
    startHandshake();
}

void CharmCommandSession::onReadyRead()
{
    QByteArray payload = m_device->readAll();

    switch (m_state) {
    case HandshakeState:
        handleHandshare(payload);
        break;
    case CommandState:
        handleCommand(payload);
        break;
    case InvalidState:
        qDebug("Received data while in invalid state. Discarding.");
        break;
    }
}

void CharmCommandSession::sendAck(const QString &comment)
{
    m_device->write(QString("%1 %2\n")
        .arg(CHARM_CI_SERVER_ACK)
        .arg(comment)
        .toLatin1());
}

void CharmCommandSession::sendNak(const QString &comment)
{
    m_device->write(QString("%1 %2\n")
        .arg(CHARM_CI_SERVER_NAK)
        .arg(comment)
        .toLatin1());
}

void CharmCommandSession::sendComment(const QString &comment)
{
    m_device->write(QString("%1 %2\n")
        .arg(CHARM_CI_SERVER_COMMENT)
        .arg(comment)
        .toLatin1());
}

void CharmCommandSession::startHandshake()
{
    sendComment("Charm Command Line Interface");

    m_device->write(QString("%1 %2\n")
        .arg(CHARM_CI_HANDSHAKE_SEND)
        .arg(QString::number(CHARM_CI_VERSION))
        .toLatin1());

    m_state = HandshakeState;
}

void CharmCommandSession::startCommand()
{
    /*
     * simulate event activated events
     */
    EventIdList activeEvents = DATAMODEL->activeEvents();
    foreach (EventId id, activeEvents)
        eventActivated(id);

    m_state = CommandState;
}

void CharmCommandSession::handleHandshare(QByteArray payload)
{
    QBuffer buffer(&payload);
    buffer.open(QIODevice::ReadOnly);

    if (!buffer.canReadLine()) {
        buffer.close();
        return;
    }

    QString reply = buffer.readLine();

    if (reply.startsWith(CHARM_CI_HANDSHAKE_RECV, Qt::CaseInsensitive)) {
        sendAck("Entering Command Mode");
        startCommand();
    }
    else if (reply.startsWith(CHARM_CI_COMMAND_DISCONNECT, Qt::CaseInsensitive)) {
        qDebug("BYE command received. Closing connection.");
        m_device->close();
    }
}

void CharmCommandSession::handleCommand(QByteArray payload)
{
    QBuffer buffer(&payload);
    buffer.open(QIODevice::ReadOnly);

    if (!buffer.canReadLine()) {
        buffer.close();
        return;
    }

    const QString command = buffer.readLine().trimmed();

    const QStringList segment =
        command.split(' ', QString::SkipEmptyParts);

    if (segment.isEmpty()) {
        qDebug("Received empty command...");
        return;
    }

    if (segment[0].compare(CHARM_CI_COMMAND_START, Qt::CaseInsensitive) == 0) {
        bool tid_ok;
        TaskId tid;

        if (segment.count() == 2)
            tid = segment[1].toInt(&tid_ok);
        else {
            EventMap::const_reverse_iterator i = DATAMODEL->eventMap().rbegin();
            tid_ok = (i != DATAMODEL->eventMap().rend());
            tid = i->second.taskId();
        }

        if (tid_ok && DATAMODEL->taskExists(tid)) {
            if (!DATAMODEL->isTaskActive(tid)) {
                qDebug("START command received. Starting task %d", tid);
                DATAMODEL->startEventRequested(DATAMODEL->getTask(tid));
            }
        }
        else sendNak("UNKNOWN TASK");
    }

    else if (segment[0].compare(CHARM_CI_COMMAND_STOP, Qt::CaseInsensitive) == 0) {
        bool tid_ok;
        TaskId tid;

        if (segment.count() == 2)
            tid = segment[1].toInt(&tid_ok);
        else {
            tid = DATAMODEL->activeEventCount() > 0 ?
                DATAMODEL->eventForId(DATAMODEL->activeEvents().last()).taskId() : 0;
            tid_ok = (tid > 0);
        }

        if (tid_ok && DATAMODEL->taskExists(tid) && DATAMODEL->isTaskActive(tid)) {
            qDebug("STOP command received. Stopping task %d", tid);
            DATAMODEL->endEventRequested(DATAMODEL->getTask(tid));
        }
        else sendNak("UNKNOWN TASK");
    }

    else if (segment[0].compare(CHARM_CI_COMMAND_TASK, Qt::CaseInsensitive) == 0) {
        bool tid_ok;
        TaskId tid;

        if (segment.count() == 2)
            tid = segment[1].toInt(&tid_ok);
        else tid_ok = false;

        if (tid_ok && DATAMODEL->taskExists(tid)) {
            qDebug("TASK command received. Task %d requested", tid);
            m_device->write(DATAMODEL->taskIdAndSmartNameString(tid).toLatin1());
            m_device->write("\n");
        }
        else sendNak("UNKNOWN TASK");
    }

    else if (segment[0].compare(CHARM_CI_COMMAND_STATUS, Qt::CaseInsensitive) == 0) {
        qDebug("STATUS command received.");

        const EventIdList activeEvents = DATAMODEL->activeEvents();
        if (!activeEvents.isEmpty()) {
            foreach (EventId id, activeEvents) {
                const Event &event = DATAMODEL->eventForId(id);
                m_device->write(QString("%0 %1\n")
                    .arg(event.taskId(), 4, 10, QChar('0'))
                    .arg(event.duration()).toLatin1());
            }
        }
        else sendNak("WORK HARDER");
    }

    else if (segment[0].compare(CHARM_CI_COMMAND_RECENT, Qt::CaseInsensitive) == 0) {
        bool offset_ok;
        bool count_ok;
        int offset;
        int count;
        const EventIdList recent = DATAMODEL->mostRecentlyUsedTasks();

        /* default params */

        offset_ok = true;
        offset = 0;

        count_ok = true;
        count = 5;

        if (segment.count() > 1) {
            offset = segment[1].toInt(&offset_ok);
            if (segment.count() > 2)
                count = segment[2].toInt(&count_ok);
        }

        if (offset_ok && count_ok && offset >= 0 && count >= 1 && recent.size() > offset) {
            qDebug("RECENT command received. Sending %d entries starting from offset %d", count, offset);

            if ((offset + count) > recent.size())
                count = recent.size() - offset;

            for (int i = 0; i < count; ++i) {
                m_device->write(DATAMODEL->taskIdAndSmartNameString(recent[offset + i]).toLatin1());
                m_device->write("\n");
            }
        }
        else sendNak("INVALID REQUEST");
    }

    else if (segment[0].compare(CHARM_CI_COMMAND_DISCONNECT, Qt::CaseInsensitive) == 0) {
        qDebug("BYE command received. Closing connection.");
        m_device->close();
    }

    /* unknown command sent */
    else sendNak("UNKNOWN COMMAND");
}

