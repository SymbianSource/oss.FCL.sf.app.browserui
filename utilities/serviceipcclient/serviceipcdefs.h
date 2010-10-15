#ifndef serviceipcdefs_h
#define serviceipcdefs_h

#include <Qt/qglobal.h>

namespace WRT {

    #ifdef QT_MAKE_IPC_DLL
        # define SFWIPC_EXPORT Q_DECL_EXPORT
    #else
        # define SFWIPC_EXPORT Q_DECL_IMPORT
    #endif

    #ifdef QT_MAKE_IPC_SERVER_DLL
        # define SFWIPCSRV_EXPORT Q_DECL_EXPORT
    #else
        # define SFWIPCSRV_EXPORT Q_DECL_IMPORT
    #endif

    enum TServiceIPCBackends
        {
        EDefaultIPC, 
        ELocalSocket, 
        ESymbianServer, 
        ESharedMemory
        };

    static const char FUNCTIONSEM[] = "_funclock";
    static const char SERVERSEM[] = "_serverlock";
    static const char SERVERNOTSTARTED[] = "0";
    static const char SERVERSTARTED[] = "1";

    #define GETSESSIONID   "GetSessionId"
    #define SETSESSIONINFO   "SetSessionInfo"
    #define SUBSCRIBEBROADCASTMSG "SubscribeBroadcastMsg"
}
#endif // serviceipcdefs_h
