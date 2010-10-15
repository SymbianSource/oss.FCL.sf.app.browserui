#ifndef UITIMER_H_
#define UITIMER_H_

#include <QDateTime>
#include <QDebug>
#include "qstmfilelogger.h"

#define TIMESTAMP() QDateTime::currentDateTime().toString("hh:mm:ss:zzz")

#ifdef Q_OS_SYMBIAN
#include <e32base.h>
#include <e32const.h>

class CUiTimer : public CTimer
{
public:
    enum CUiTimerType
    {
        AfterType,
        HighResType,
        LockType
    };
    
    CUiTimer(int priority) : CTimer(priority), m_timerType(AfterType)
    {
    }
    
    
    
    static CUiTimer* New(int priority)
    {
        CUiTimer* uitao = new CUiTimer(priority);
        if (uitao) {            
            TRAPD(r, uitao->ConstructL());
            if (r == KErrNone) {
                CActiveScheduler::Add(uitao);
            }
            else {
                delete uitao;
                uitao = NULL;
            }
        }
        return uitao;
    }
    
    ~CUiTimer() {}
    
    void Start(TTimeIntervalMicroSeconds32 delay,TTimeIntervalMicroSeconds32 interval,TCallBack callBack)
    {
        __ASSERT_ALWAYS(interval.Int()>=0,User::Panic(_L("Interval < 0"), -1));
        __ASSERT_ALWAYS(delay.Int()>=0,User::Panic(_L("Delay < 0"), -2));
        m_interval = interval.Int();
        m_callback = callBack;
        m_timerType = AfterType;
        After(delay);
    }
    
    void StartHighRes(TTimeIntervalMicroSeconds32 delay,TTimeIntervalMicroSeconds32 interval,TCallBack callBack)
    {
        __ASSERT_ALWAYS(interval.Int()>=0,User::Panic(_L("Interval < 0"), -1));
        __ASSERT_ALWAYS(delay.Int()>=0,User::Panic(_L("Delay < 0"), -2));
        m_interval = interval.Int();
        m_callback = callBack;
        m_timerType = HighResType;
        HighRes(delay);
    }
        
    void StartLock(TTimerLockSpec lockSpec,TCallBack callBack)
    {
        m_lockSpec = lockSpec;
        m_callback = callBack;
        m_timerType = LockType;
        Lock(m_lockSpec);
    }

    
    void RunL()
    {
        switch (m_timerType) {
        case HighResType:
            HighRes(m_interval);
            m_callback.CallBack();
            break;
        case LockType:
            Lock(m_lockSpec);
            m_callback.CallBack();
            break;
        case AfterType:
            After(m_interval);
            m_callback.CallBack();
            break;
        }
    }
    
private:    
    uint            m_interval; 
    TCallBack       m_callback;
    CUiTimerType    m_timerType;
    TTimerLockSpec  m_lockSpec;
};

class UiTimer : QObject
{
    enum UiTimerType
    {
        AfterType,
        HighResType,
        LockType
    };

public:
    static UiTimer*  New() 
    { 
        return new UiTimer(); 
    }
  
    inline UiTimer(): QObject(), m_id(0), m_requestedTicks(-1), 
                      m_ticksCount(0), m_cbName("uiTimerCallback") 
    {    
        m_timer = CUiTimer::New(20);
    }
  
    inline void setPriority(int priority) { m_timer->SetPriority(priority); }
    inline ~UiTimer() { m_timer->Cancel(); delete m_timer;}
    
    inline int timerId() const { return m_id; }

    static int timercallback(TAny* ptr)
    {
        UiTimer* uitimer = static_cast<UiTimer*>(ptr);
        uitimer->doTimerCallback();
        return 0;
    }
    
    void doTimerCallback()
    {
       if (isActive() && (m_requestedTicks < 0 || m_ticksCount++ < m_requestedTicks)) {  
           
           QMetaObject::invokeMethod(m_obj, m_cbName, Qt::DirectConnection);
           if (m_ticksCount == m_requestedTicks) {
               stop();
           }
       }
       return;
    }
    
    void start(int msec, QObject *obj, UiTimerType timerType = HighResType) 
    {
        m_delay = msec;
        m_obj = obj;
        m_ticksCount = 0;
        switch (timerType)
        {
        case HighResType:
            m_timer->StartHighRes((msec + 1) * 1000, (msec + 1) * 1000, TCallBack(timercallback, this));
            break;
        case AfterType:
            m_timer->Start(msec * 1000, msec * 1000, TCallBack(timercallback, this));
            break;
        case LockType:
            TTimerLockSpec lockSpec = static_cast<TTimerLockSpec>(msec);
            m_timer->StartLock(lockSpec, TCallBack(timercallback, this));
            break;
        }
    }
    
    void stop() 
    { 
        m_timer->Cancel(); 
    }
    
    bool isActive() { return m_timer->IsActive(); }
    
    void setRequestedTicks(int ticksCount) { m_requestedTicks = ticksCount; }
    int ticksCount() { return m_ticksCount; }
    
    void setSingleShot(bool isOneShot) { isOneShot ?  m_requestedTicks = 1 : m_requestedTicks = -1; }
    bool isSingleShot() { return m_requestedTicks == 1; }
    
    void setTimerCallback(char* cbName) { m_cbName = cbName; }
    
private:
    int         m_id;
    int         m_delay;
    QObject*    m_obj;
    CUiTimer*   m_timer;
    int         m_requestedTicks;
    int         m_ticksCount;
    char*       m_cbName;
    
};
#else
class UiTimer : public QObject
{
public:
    static UiTimer* New() { return new UiTimer(); }
    UiTimer() : QObject(), m_requestedTicks(-1), m_cbName(0)
    {
    }
    inline void setPriority(int priority) {}
    inline void start(int msec, QObject *obj) 
    {
        m_delay = msec;
        m_obj = obj;
        m_ticksCount = 0;
        m_timer.start(msec, this);
    }
    inline ~UiTimer() {}
        
    inline int timerId() const { return m_timer.timerId(); }

    inline void timerEvent(QTimerEvent *event)
    {
        if (m_requestedTicks < 0 || m_ticksCount++ < m_requestedTicks) {
            QMetaObject::invokeMethod(m_obj, m_cbName ? m_cbName : "uiTimerCallback", Qt::DirectConnection);
        }
        else {
            stop();
        }
        return;
    }

    inline void stop() { m_timer.stop(); }
        
    inline bool isActive() { return m_timer.isActive(); }

    void setRequestedTicks(int ticksCount) { m_requestedTicks = ticksCount; }
    int ticksCount() { return m_ticksCount; }
    
    void setSingleShot(bool isOneShot) { isOneShot ?  m_requestedTicks = 1 : m_requestedTicks = -1; }
    bool isSingleShot() { return m_requestedTicks == 1; }
    void setTimerCallback(char* cbName) { m_cbName = cbName; }

private:
    QBasicTimer  m_timer; 
    int          m_delay;
    QObject*     m_obj;
    int         m_requestedTicks;
    int         m_ticksCount;
    char*       m_cbName;
};

#endif




#endif /* UITIMER_H_ */
