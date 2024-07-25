#ifndef _RACE_DIRECTOR_HPP_
#define _RACE_DIRECTOR_HPP_

#include "InternalsPlugin.hpp"     // classe base da qual os objetos de plugin devem derivar
#include <cstdio>                  // para logging

class RaceDirectorPlugin : public InternalsPluginV07 // LEMBRETE: a função exportada GetPluginVersion() deve retornar 1 se você estiver derivando de InternalsPluginV01, 2 para InternalsPluginV02, etc.
{
    protected:

    // Construtor/destrutor
    RaceDirectorPlugin() {}
    ~RaceDirectorPlugin() {}

    // log
    bool mLogging;                        // se queremos habilitar o log
    char *mLogPath;                       // caminho para gravar os logs
    FILE *mLogFile;                       // o ponteiro para o arquivo

    public:



};


#endif // _RACE_DIRECTOR_HPP_