#include "Logger.h"
#include <fstream>
#include <ctime>
#include <sys/time.h>
#include "errno.h"
#include "string.h"
#include "LockFile.h"

Logger* Logger::instancia = 0;

Logger::Logger()
{
    log_path = "log.txt";
    mode = DEBUG; //cambiar para cambiar el default. en la version final el default deberia ser ERROR
    mode_symbols[0] = "D.\t";
    mode_symbols[1] = "W.\t";
    mode_symbols[2] = "E.\t";
    mode_symbols[3] = "I.\t";
    mode_symbols[4] = "T.\t";

    log_timestamp();
}

Logger::~Logger()
{

}

void Logger::destruir()
{
    if(instancia != NULL)
        delete instancia;
    instancia = NULL;
}

void Logger::log_timestamp(){
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,80,"%d-%m-%Y %H:%M:%S",timeinfo);
    std::string timestamp(buffer);

    std::string out_final = "/---------------------- " + timestamp + " ----------------------/" + "\n";

    LockFile log("log.txt");
    log.tomarLock();
    log.escribir(static_cast<const void*>(out_final.c_str()),out_final.length());
    log.liberarLock();
}

std::string Logger::timeHMSmu(){
    struct timeval tv;
    time_t nowtime;
    struct tm *nowtm;
    char tmbuf[64], buf[64];

    gettimeofday(&tv, NULL);
    nowtime = tv.tv_sec;
    nowtm = localtime(&nowtime);
    strftime(tmbuf, sizeof tmbuf, "%H:%M:%S", nowtm);
    snprintf(buf, sizeof(buf), "%s.%06d", tmbuf, tv.tv_usec);
    std::string str(buf);
    return str;
}

void Logger::_log(std::string name, std::string comment, LOG_MODE comment_mode){
    // tal vez deberia hacer esto en un fork
    if(comment_mode >= mode){
        /*std::ofstream log_file(log_path.c_str(), std::ofstream::out | std::ofstream::app);
        log_file << mode_symbols[comment_mode] << comment << std::endl;
        log_file.close();*/
        std::string out_final = this->timeHMSmu() + "\t" +
                                name + "\t" +
                                std::to_string(getpid()) + "\t" +
                                mode_symbols[comment_mode] + comment + "\n";

        LockFile log("log.txt");
        log.tomarLock();
        log.escribir(static_cast<const void*>(out_final.c_str()),out_final.length());
        log.liberarLock();
    }
}

Logger* Logger::getInstance(){
    if(!instancia){
        instancia = new Logger;
    }
    return instancia;
}

void Logger::log(std::string name, std::string comment, LOG_MODE mode){
    Logger* l = Logger::getInstance();
    l->_log(name, comment, mode);
}

void Logger::logErrno(std::string name){
    char buf[256];
    char const* str = strerror_r(errno, buf, 256);
    std::string msg(str);

    Logger::log(name, msg, ERROR);
}
