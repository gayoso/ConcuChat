#include "ClientOutput.h"

ClientOutput::ClientOutput() : forOutput(NOM_CLIENT_OUT)
{
    //ctor
}

ClientOutput::~ClientOutput()
{
    //dtor
}

void ClientOutput::init() {

}

void ClientOutput::_run() {

    SIG_Trap sigint_handler(SIGINT);
    SignalHandler::getInstance()->registrarHandler(SIGINT, &sigint_handler);

    forOutput.abrir();

    std::string input = "";
    char buffer[BUFFSIZE];
    while(!sigint_handler.signalWasReceived()) {
        forOutput.leer(bufer, BUFFSIZE);

        if(!sigint_handler.signalWasReceived()) {

        }
    }

}
