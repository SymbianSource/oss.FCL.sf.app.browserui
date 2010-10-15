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

#ifndef QSTMFILELOGGER_H_
#define QSTMFILELOGGER_H_

#include <qstmgesturedefs.h>
#include <QFile>
#include <QDebug>
#include <QString>
#include <QStringBuilder>
#include <QDateTime>
#include <QTimer>

#define LOGFLUSH            QStm_FileLogger::flush()
#define LOGCLOSE            QStm_FileLogger::close()
#if defined(Q_OS_WIN) && defined(_MSC_VER)
#define LOGARG()
#else
#define LOGARG(txt,parm...) {QStm_FileLogger::log(txt, parm);}
#endif
#define LOGTXT(txt)         {QStm_FileLogger::log("%s",txt);}
#define LOGERR(txt,err)     if (!err) {} else {LOGARG(txt, err);}
#define LOGCALL(exp)        {LOGARG("Calling \"%s\"", #exp); exp; LOGARG("Call to \"%s\" passed.", #exp);}
#define LOGENTER            LOGARG("%s start", __PRETTY_FUNCTION__)
#define LOGEXIT             LOGARG("%s end", __PRETTY_FUNCTION__)
#define LOGMEM(ptr)         LOGARG("%s [0x%x]", #ptr, (uint)ptr)
#define qstmDebug()         QStm_FileLogger::logger()->isEnabled() ? QStm_FileLogger::debug() : QStm_FileLogger::noDebug()
#define qstmIODevice()      QStm_FileLogger::ioDevice()
#define qstmEnableDebug(on) QStm_FileLogger::logger()->enable(on);


class QSTMGESTURELIB_EXPORT QStm_FileLogger : public QObject
{
    Q_OBJECT
public:
	static void log(const QString& text);
	static void log(const char* fmt,...);
	static void close() { logger()->doClose(); }
	static void flush() { logger()->doFlush(); }
	static QStm_FileLogger* logger();
	static QIODevice* ioDevice() { return logger()->m_file; }
	static QDebug debug();
	static QNoDebug noDebug() { return *logger()->m_noDebug; }
	void enable(bool on);
	bool isEnabled() { return m_enabled; }
private:
    QStm_FileLogger();
    ~QStm_FileLogger();
    void doLog(const QString& text);
    void doLog(const char* text);
    void doClose() { if (m_file) m_file->close(); }
    
    QString getLogFileName();
public slots:
    void doFlush();   
private:
    QFile*                     m_file;    
    QTextStream*               m_stream;
    static QStm_FileLogger*    m_instance;
    bool                       m_enabled;
    QDebug*                    m_debug;
    QNoDebug*                  m_noDebug;
    QString*                   m_buffer;
    QTimer                     m_flushTimer;
};



#endif /* QSTMFILELOGGER_H_ */
