/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/
#include <QDir>
#include <QApplication>
#include <QDateTime>
#include "qstmfilelogger.h"

#define BUF_SIZE  512*1004
QStm_FileLogger* QStm_FileLogger::m_instance = 0;

QStm_FileLogger::QStm_FileLogger() : QObject(), m_file(0), m_stream(0), m_enabled(0)
{
#ifdef GESTURES_LOGTOFILE
	QString fname = getLogFileName();
	m_file = new QFile(fname);
	m_file->open(QIODevice::ReadWrite | QIODevice::Text);
	m_stream = new QTextStream(m_file);
#else
	m_buffer = new QString();
	m_buffer->reserve(BUF_SIZE);
	m_debug = new QDebug(m_buffer);
	m_noDebug = new QNoDebug();
	connect(&m_flushTimer, SIGNAL(timeout()), this, SLOT(doFlush()));
	m_flushTimer.start(5000);
#endif
}


QStm_FileLogger::~QStm_FileLogger()
{
#ifdef GESTURES_LOGTOFILE
	delete m_stream;
	m_file->close();
	delete m_file;
#else
	m_flushTimer.stop();
	delete m_buffer;
	delete m_debug;
#endif
}


void QStm_FileLogger::enable(bool on) 
{ 
    m_enabled = on;
    if (m_enabled) {
        if (!m_buffer) {
            m_buffer = new QString();
            m_buffer->reserve(BUF_SIZE);   
        }
        m_flushTimer.start(5000);
    }
    else {
        delete m_buffer;
        m_buffer = NULL;
        m_flushTimer.stop();
    }
}

QString QStm_FileLogger::getLogFileName()
{
#ifdef Q_OS_SYMBIAN	
    QString path = "c:/data/Others";
    QDir logdir(path);
    logdir.mkpath(path);
#else 
    QString path = QDir::tempPath();
#endif    
    
    QString appName = qApp->applicationFilePath();
    QFileInfo fi(appName);
    QString baseName = fi.baseName();
    QString fileName = path % "/" % baseName % 
    		           QDateTime::currentDateTime().toString("ddMMyy_hhmmss") % 
    		           ".log";
    return fileName;
}


void QStm_FileLogger::log(const char* fmt,...)
{
    if (logger()->isEnabled()) {
	va_list list;
	QString logStr;
	va_start(list, fmt);
	logStr.vsprintf(fmt, list);
	logger()->doLog(logStr);
	va_end(list);
    }
}


void QStm_FileLogger::log(const QString& text)
{
    if (logger()->isEnabled()) {
	logger()->doLog(text);
    }
}

void QStm_FileLogger::doLog(const QString& text) 
{
    if (m_enabled) {
#ifdef GESTURES_LOGTOFILE
	    *m_stream << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") << " " << 
			text << "\n"; 
#else
        if (m_buffer->size() >= BUF_SIZE) {
	    //doFlush();
	    }
        *m_debug << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") << " " << 
                text << "\n"; 
#endif
    }
}

void QStm_FileLogger::doLog(const char* text) 
{ 
    if (m_enabled) {
#ifdef GESTURES_LOGTOFILE
	    *m_stream << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") << " " << 
			text << "\n"; 
#else
	    if (m_buffer->size() >= BUF_SIZE) {
	        //doFlush();
	    }
        *m_debug << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") << " " << 
                text << "\n"; 
#endif
    }
}


void QStm_FileLogger::doFlush() 
{ 
    if (m_enabled) {
#ifdef GESTURES_LOGTOFILE
        if (m_stream) m_stream->flush();
#else
        if (!m_buffer->isEmpty()) {
            qDebug() << *m_buffer;
            m_buffer->clear();
        }
#endif
    }
}

QStm_FileLogger* QStm_FileLogger::logger()
{
	if (!m_instance) {
		m_instance = new QStm_FileLogger();
	}
	return m_instance;
}

QDebug QStm_FileLogger::debug() 
{ 
    if (logger()->isEnabled()) {
        *logger()->m_debug << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") << " "; 
    }
    return *logger()->m_debug;
}
