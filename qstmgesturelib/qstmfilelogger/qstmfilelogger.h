/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
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

#define LOGFLUSH            QStm_FileLogger::flush()
#define LOGCLOSE            QStm_FileLogger::close()
#ifdef Q_OS_WIN
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

class QSTMGESTURELIB_EXPORT QStm_FileLogger
{
public:
	static void log(const QString& text);
	static void log(const char* fmt,...);
	static void close() { logger()->doClose(); }
	static void flush() { logger()->doFlush(); }
	static QStm_FileLogger* logger();
	static QIODevice* ioDevice() { return logger()->m_file; }
private:
    QStm_FileLogger();
    ~QStm_FileLogger();
    void doLog(const QString& text);
    void doLog(const char* text);
    void doClose() { m_file->close(); }
    void doFlush() { m_stream->flush(); }   
    QString getLogFileName();

private:
    QFile*                     m_file;    
    QTextStream*               m_stream;
    static QStm_FileLogger*    m_instance;
};



#endif /* QSTMFILELOGGER_H_ */
