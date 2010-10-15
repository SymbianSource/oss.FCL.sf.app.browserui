/*
 * =============================================================================
 *  Name        : serviceipcsymbiansession.h
 *  Part of     : Service Framework / Service IPC
 *  Interface   : 
 *  Description : Client session class to  server
 *  Version     : %version: 1 %
 *
 *  Copyright © 2008 Nokia. All rights reserved.
 *  This material, including documentation and any related 
 *  computer programs, is protected by copyright controlled by 
 *  Nokia. All rights are reserved. Copying, including 
 *  reproducing, storing, adapting or translating, any 
 *  or all of this material requires the prior written consent of 
 *  Nokia. This material also contains confidential 
 *  information which may not be disclosed to others without the 
 *  prior written consent of Nokia.
 * =============================================================================
 */

#ifndef C_SERVICEIPC_SYMBIANSESSION_H
#define C_SERVICEIPC_SYMBIANSESSION_H

#include <e32std.h>
namespace WRT {
    
    class RServiceIPCSession : public RSessionBase
    {
    public:
        TInt Connect(const TDesC& aServer, const TVersion& aVersion);
    
        TVersion Version() const;
    
        TInt SendReceiveL(TInt aFunction) const;
    
        TInt SendReceiveL(TInt aFunction, const TIpcArgs& aArgs) const;
    
        void SendReceive(TInt aFunction, TRequestStatus& aStatus) const;
    
        void SendReceive(TInt aFunction,
                         const TIpcArgs& aArgs,
                         TRequestStatus& aStatus) const;
    
        TInt StartServer(const TDesC& aImage);
    
    private:
        TVersion iVersion;
    };
}
#endif // C_SERVICEIPC_SYMBIANSESSION_H
// End of file
