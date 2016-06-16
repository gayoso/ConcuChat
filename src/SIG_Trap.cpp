#include "SIG_Trap.h"

SIG_Trap::SIG_Trap(int mysignal) : signal_received(0), my_signal(mysignal)
{
    //ctor
}

SIG_Trap::~SIG_Trap()
{
    //dtor
}

int SIG_Trap::handleSignal(int signum) {
    assert ( signum == my_signal );
    this->signal_received = 1;
    return 0;
}
