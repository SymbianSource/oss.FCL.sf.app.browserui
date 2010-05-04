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

#include <QString>

class QTime;
class GTimer
{
 public:
  GTimer();
  void start(QString op);
  void stop();
  void save();
  QString log();
  void clearLog(){m_log.clear();}
 private:
  QTime * m_time;
  QString m_start;
  QString m_op;
  QString m_log;

};
