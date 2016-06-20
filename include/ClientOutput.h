#ifndef CLIENTOUTPUT_H
#define CLIENTOUTPUT_H


class ClientOutput : public Runnable
{
    public:
        ClientOutput();
        virtual ~ClientOutput();
    protected:

        void init();
        void _run();
        FifoLectura forOutput;

    private:
};

#endif // CLIENTOUTPUT_H
