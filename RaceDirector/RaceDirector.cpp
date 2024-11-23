////////////////////////////////
// Criado por: Felipe Granado //
////////////////////////////////

#define _CRT_SECURE_NO_WARNINGS

#include "RaceDirector.hpp"


using namespace std;
namespace fsm = filesystem;

// plugin information

extern "C" __declspec( dllexport )
const char * __cdecl GetPluginName()                   { return( "Race Director - 2024.07.25" ); }

extern "C" __declspec( dllexport )
PluginObjectType __cdecl GetPluginType()               { return( PO_INTERNALS ); }

extern "C" __declspec( dllexport )
int __cdecl GetPluginVersion()                         { return( 7 ); }  // Funcionalidade do InternalsPluginV07 (se voce mudar esse valor de retorno, deve derivar da classe apropriada!)

extern "C" __declspec( dllexport )
PluginObject * __cdecl CreatePluginObject()            { return( static_cast< PluginObject * >( new RaceDirectorPlugin ) ); }

extern "C" __declspec( dllexport )
void __cdecl DestroyPluginObject( PluginObject *obj )  { delete( static_cast< RaceDirectorPlugin * >( obj ) ); }


// StartControl sc;

RaceDirectorPlugin::RaceDirectorPlugin()
{
    mNumPilotos = 0;
    mMaxPilotos = 0;
    // mPiloto = NULL;

    faseSessao = 0;

    atualizaTelemetria = false;
}


void RaceDirectorPlugin::SetEnvironment(const EnvironmentInfoV01& info)
{
    string caminhoBase = info.mPath[0];
   mLocal = caminhoBase + "Log\\RaceDirector\\";

    if (!fsm::exists(mLocal))
    {
        fsm::create_directories(mLocal);
    }
}


void RaceDirectorPlugin::StartSession()
{
    NomeArquivo();

    EscreverLog( "Local do arquivo: " + mLocal );

    EscreverLog( 
        "Controle de largada: " + to_string(mLigado) +
        "\t| Marcha: " + to_string(mMarcha) +
        "\t| Limitador: " + to_string(mLimitador) +
        "\t| Vel Maxima : " + to_string(mMaxVelKPH) +
        "\n"
    );

    EscreverLog( "---------- SESSAO INICIADA ----------" );
}


/*
// Função chamada quando a sessão é encerrada
void RaceDirectorPlugin::EndSession()
{
    EscreverLog("a", "--- SESSAO ENCERRADA ---");


}
*/


void RaceDirectorPlugin::UpdateScoring(const ScoringInfoV01& info)
{
    if (info.mSession >= 10)
    {
        mNumPilotos = info.mNumVehicles;
        mMaxPilotos = info.mMaxPlayers;

        if (faseSessao != info.mGamePhase) //  && info.mGamePhase == 5
        {
            faseSessao = info.mGamePhase;

            mPilotos.reserve(info.mMaxPlayers);

            ostringstream oss;
            oss << "Pista: " << info.mTrackName
                << "\t| Pilotos: " << info.mNumVehicles
                << "\t| Sessao: " << info.mSession
                << "\t| Fase: " << to_string(info.mGamePhase)
                << "\t| Tempo: " << info.mCurrentET;
            EscreverLog(oss.str());

            
            for (int i = 0; i < info.mNumVehicles; ++i)
            {
                const VehicleScoringInfoV01& vsi = info.mVehicle[i];

                PilotoInfo piloto;
                piloto.mIndex = i;
                piloto.mID = vsi.mID;
                piloto.mPiloto = vsi.mDriverName;
                piloto.mVeiculo = vsi.mVehicleName;
                piloto.mClasse = vsi.mVehicleClass;
                piloto.mEmPit = vsi.mInPits;

                mPilotos.push_back(piloto);
            }
            atualizaTelemetria = true;
        }
    }
}


void RaceDirectorPlugin::UpdateTelemetry(const TelemInfoV01& info)
{
    double velMPH = sqrt(info.mLocalVel.x * info.mLocalVel.x + info.mLocalVel.y * info.mLocalVel.y + info.mLocalVel.z * info.mLocalVel.z) * 2.23694;
    double velKPH = sqrt(info.mLocalVel.x * info.mLocalVel.x + info.mLocalVel.y * info.mLocalVel.y + info.mLocalVel.z * info.mLocalVel.z) * 3.6;
    
    for (long i = 0; i < mNumPilotos; ++i) {
        PilotoInfo piloto;

        if (piloto.mIndex == i)
        {
            piloto.mVelMPH = velMPH;
            piloto.mVelKPH = velKPH;
            piloto.mMarcha = info.mGear;
            piloto.mLimitador = info.mSpeedLimiter;

            ostringstream oss;
            oss << piloto.mIndex << "\t| "
                << piloto.mID << "\t| "
                << piloto.mPiloto << "\t| "
                << piloto.mVeiculo << "\t| "
                << piloto.mClasse << "\t| "
                << piloto.mEmPit << "\t| "
                << piloto.mVelKPH << "\t| "
                << piloto.mMarcha << "\t| "
                << piloto.mLimitador;

            EscreverLog(oss.str());
        }

            StartControl(piloto);
    }
}


long RaceDirectorPlugin::WantsTelemetryUpdates()
{
    if (atualizaTelemetria)
    {
        atualizaTelemetria = false;
        return(2);
    }
    else
    {
        return (0);
    }
}


bool RaceDirectorPlugin::WantsToDisplayMessage(MessageInfoV01& msgInfo)
{
    if (mMensagem == "")
    {
        return(false);
    }
    else
    {
        msgInfo.mDestination = 1;
        msgInfo.mTranslate = 0;
        strcpy_s(msgInfo.mText, mMensagem.c_str());

        mMensagem = "";
        return(true);
    }
}


bool RaceDirectorPlugin::GetCustomVariable(long i, CustomVariableV01& var)
{
    switch (i)
    {
    case 0:
    {
        // rF2 criará automaticamente esta variável e a padronizará como 1 (true), a menos que a criemos primeiro, caso em que podemos escolher o padrão.
        strcpy_s(var.mCaption, " Enabled");
        var.mNumSettings = 2;
        var.mCurrentSetting = 1;
        return(true);
    }
    // retorna antes do break;

    case 1:
    {
        strcpy_s(var.mCaption, "LogRegister");
        var.mNumSettings = 2;
        var.mCurrentSetting = 1;
        return(true);
    }
    // retorna antes do break;

    case 2:
    {
        strcpy_s(var.mCaption, "Language");
        var.mNumSettings = 3;
        var.mCurrentSetting = 0;
        return(true);
    }
    // retorna antes do break;

    case 3:
    {
        strcpy_s(var.mCaption, "Penalty");
        var.mNumSettings = 64;
        var.mCurrentSetting = -1;
        return(true);
    }
    // retorna antes do break;

    case 4:
    {
        strcpy_s(var.mCaption, "StartControl");
        var.mNumSettings = 2;
        var.mCurrentSetting = 0;
        return(true);
    }
    // retorna antes do break;

    case 5:
    {
        strcpy_s(var.mCaption, "StartControlGear");
        var.mNumSettings = 10;
        var.mCurrentSetting = 0;
        return(true);
    }
    // retorna antes do break;
    case 6:
    {
        strcpy_s(var.mCaption, "StartControlLimiter");
        var.mNumSettings = 2;
        var.mCurrentSetting = 0;
        return(true);
    }
    // retorna antes do break;
    case 7:
    {
        strcpy_s(var.mCaption, "StartControlMaxSpeedKPH");
        var.mNumSettings = 301;
        var.mCurrentSetting = 80;
        return(true);
    }
    // retorna antes do break;
    case 8:
    {
        strcpy_s(var.mCaption, "StartControlPenalty");
        var.mNumSettings = 64;
        var.mCurrentSetting = -3;
        return(true);
    }
    // retorna antes do break;
    }
    return(false);
}


void RaceDirectorPlugin::GetCustomVariableSetting(CustomVariableV01& var, long i, CustomSettingV01& setting)
{
    if (0 == _stricmp(var.mCaption, " Enabled"))
    {
        if (0 == i)
            strcpy_s(setting.mName, "False");
        else
            strcpy_s(setting.mName, "True");
    }
    else if (0 == _stricmp(var.mCaption, "LogRegister"))
    {
        if (0 == i)
            strcpy_s(setting.mName, "False");
        else
            strcpy_s(setting.mName, "True");
    }
    else if (0 == _stricmp(var.mCaption, "Language"))
    {
        if (0 == i)
            strcpy_s(setting.mName, "en");
        else if (1 == i)
            strcpy_s(setting.mName, "pt");
    }
    else if (0 == _stricmp(var.mCaption, "Penalty"))
    {
        if (-3 == i)
            sprintf_s(setting.mName, "Do Nothing");
        else if (-2 == i)
            sprintf_s(setting.mName, "Longest Line");
        else if (-1 == i)
            sprintf_s(setting.mName, "Drive-Thru");
        else if (0 <= i)
            sprintf_s(setting.mName, "Stop&Go");
    }
    else if (0 == _stricmp(var.mCaption, "StartControl"))
    {
        if (i == 0)
            sprintf_s(setting.mName, "False");
        else if (i == 1)
            sprintf_s(setting.mName, "True");
    }
    else if (0 == _stricmp(var.mCaption, "StartControlGear"))
    {
        if (i == 0)
            sprintf_s(setting.mName, "Disabled");
        else if (i > 0)
            sprintf_s(setting.mName, "%d", mMarcha);
    }
    else if (0 == _stricmp(var.mCaption, "StartControlLimiter"))
    {
        if (i == 0)
            sprintf_s(setting.mName, "Disabled");
        else if (i == 1)
            sprintf_s(setting.mName, "Enabled");
    }
    else if (0 == _stricmp(var.mCaption, "StartControlMaxSpeedKPH"))
    {
        if (i == 0)
            sprintf_s(setting.mName, "Disabled");
        else if (i > 0)
            sprintf_s(setting.mName, "%.3f", mMaxVelKPH);
    }
    else if (0 == _stricmp(var.mCaption, "StartControlPenalty"))
    {
        if (i == -3)
            sprintf_s(setting.mName, "Disabled");
        else if (i == -2)
            sprintf_s(setting.mName, "Longest Line");
        else if (i == -1)
            sprintf_s(setting.mName, "Drive-Thru");
        else if (i >= 0 && i <= 60)
            sprintf_s(setting.mName, "Stop&Go %ds", mPenalidade);
    }
}


void RaceDirectorPlugin::AccessCustomVariable(CustomVariableV01& var)
{
    if (0 == _stricmp(var.mCaption, " Enabled"))
    {
        // Nao faca nada; esta variavel e apenas para o rF2 saber se deve manter o plugin carregado.
    }
    else if (0 == _stricmp(var.mCaption, "LogRegister"))
    {
        mLog = var.mCurrentSetting;
    }
    else if (0 == _stricmp(var.mCaption, "Language"))
    {
        rdIdioma = var.mCurrentSetting;
    }
    else if (0 == _stricmp(var.mCaption, "Penalty"))
    {
        rdPenalidade = var.mCurrentSetting;
    }
    else if (0 == _stricmp(var.mCaption, "StartControl"))
    {
        /// scControleLargada = var.mCurrentSetting;
        mLigado = var.mCurrentSetting;
    }
    else if (0 == _stricmp(var.mCaption, "StartControlGear"))
    {
        // scMarcha = var.mCurrentSetting;
        mMarcha = var.mCurrentSetting;
    }
    else if (0 == _stricmp(var.mCaption, "StartControlLimiter"))
    {
        // scLimitador = var.mCurrentSetting;
        mLimitador = var.mCurrentSetting;
    }
    else if (0 == _stricmp(var.mCaption, "StartControlMaxSpeedKPH"))
    {
        // scMaxVelKPH = var.mCurrentSetting;
        mMaxVelKPH = var.mCurrentSetting;
    }
    else if (0 == _stricmp(var.mCaption, "StartControlPenalty"))
    {
        // scPenalidade = var.mCurrentSetting;
        mPenalidade = var.mCurrentSetting;
    }
}


void RaceDirectorPlugin::NomeArquivo()
{
    auto agora = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(agora);
    tm now;
    localtime_s(&now, &t);

    ostringstream oss;

    oss << "RD-" << put_time(&now, "%Y_%m_%d-%H_%M_%S") << ".txt";

    mLocal += oss.str();

    /*
        // Obter o tempo atual
        time_t t = time(nullptr);
        tm now;
        localtime_s(&now, &t);

        // Formatar o nome do arquivo com a data e hora
        char filename[32];
        strftime(filename, sizeof(filename), "RD-%Y_%m_%d-%H_%M_%S.txt", &now);
    */
}


// Escrever o Log no arquivo criado
void RaceDirectorPlugin::EscreverLog(const string msg) const
{
    ofstream arquivo(mLocal, std::ios::app);

    if (arquivo.is_open())
    {
        arquivo << msg << endl;
        // clog << "TESTE DE MENSAGEM DE LOG" << endl;
    }
    else
    {
        // cerr << "TESTE DE LOG DE ERRO" << endl;
    }

    /*
    errno_t err = fopen_s(&logFile, mLocal.c_str(), "a");

    if (err == 0 && logFile != nullptr)
    {

        fprintf_s( logFile, "%s\n", msg.c_str());  // Escreve a mensagem formatada no arquivo

        fclose(logFile);
    }
    */
}


void RaceDirectorPlugin::StartControl(PilotoInfo& piloto) {
    aplicaPenalidade = false;

    if (piloto.mEmPit == 0) {

        if (piloto.mVelKPH > mMaxVelKPH) {
            aplicaPenalidade = true;
        }
        else if (piloto.mMarcha != mMarcha) {
            aplicaPenalidade = true;
        }
        else if (piloto.mLimitador != mLimitador) {
            aplicaPenalidade = true;
        }
        else {
            aplicaPenalidade = false;
        }

    }

    if (aplicaPenalidade) {
        ostringstream oss;
        oss << "/ addpenalty " << mPenalidade << " " << piloto.mPiloto;
        mMensagem = oss.str();
    }
}
