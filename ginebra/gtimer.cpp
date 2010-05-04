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


#include "gtimer.h"
#include <QTime>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <assert.h>

GTimer::GTimer()
  : m_time(0)
{
}


void GTimer::start(QString op){
  if(!m_time){
    m_time = new QTime();
  }
  m_op = op;
  m_start = QTime::currentTime().toString("hh:mm:ss:zzz");
  m_time->start();
}


void GTimer::stop(){
  int elapsed = m_time->elapsed();
  //qDebug() << m_op << ": " << elapsed << " ms";
  m_log += m_op + ": " + m_start + QString(": %1ms\n").arg(elapsed);
}

void GTimer::save(){
  /*  FILE* fp = fopen("c:\\data\\ginebra_log.txt", "a");
    if (fp) {
      char* str = m_log.toLatin1().data();
      fwrite(str, strlen(str), 1, fp);
      fclose(fp);

      }*/

  QFile l("c:\\data\\ginebra_log.txt");
  if(l.open(QFile::ReadWrite | QFile::Append )){
    QTextStream out(&l);
    out << m_log;
    l.close();
  }else{
    assert(0);
  }

}


QString GTimer::log(){
  return m_log;
}
