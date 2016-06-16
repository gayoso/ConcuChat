#ifndef RUNNABLE_H
#define RUNNABLE_H

#include <unistd.h>

class Runnable
{
    public:
        Runnable();
        virtual ~Runnable();

        pid_t run();
    protected:
        bool child;

        virtual void _run() = 0;
        virtual void init() = 0;
    private:
};

#endif // RUNNABLE_H
