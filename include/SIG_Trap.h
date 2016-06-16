#ifndef SIG_TRAP_H
#define SIG_TRAP_H

#include <signal.h>
#include <assert.h>

#include "EventHandler.h"

class SIG_Trap : public EventHandler
{
    public:
        SIG_Trap(int mysignal);
        virtual ~SIG_Trap();

        virtual int handleSignal ( int signum );

		sig_atomic_t signalWasReceived () const { return signal_received; }

		void reset() { signal_received = 0; }

    protected:
    private:
        sig_atomic_t signal_received;
        int my_signal;
};

#endif // SIG_TRAP_H
