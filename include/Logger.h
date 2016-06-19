#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include "Runnable.h"
#define LOG_MODES 5

typedef enum Logmode{
    DEBUG,
    WARNING,
    ERROR,
    INFO,
    TRACE
} LOG_MODE;

class Logger
{
    public:
        virtual ~Logger();

        void setLogPath(std::string path) { log_path = path; }
        static Logger* getInstance();
        static void destruir();
        static void log(std::string name, std::string comment, LOG_MODE mode);
        static void logErrno(std::string name);

    protected:

    private:

        Logger();
        void _log(std::string name, std::string comment, LOG_MODE mode);
        static Logger* instancia;

        LOG_MODE mode;
        std::string log_path;
        std::string mode_symbols[LOG_MODES];
        void log_timestamp();
        std::string timeHMSmu();
};

#endif // LOGGER_H
