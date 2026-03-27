////////////////////////////////
// Criado por: Felipe Granado //
////////////////////////////////

#include "RaceDirector.hpp"
#include "InternalsPlugin.hpp"


RaceDirectorPlugin::RaceDirectorPlugin()
{
    double mTempoLimite = -1;
    double mContagemFCY = -1;
    double mContagemGreen = -1;
    double mVelocidadeKPH = -1;
    double mPenalidade = -1;

    bool mLog = false;
    int mIdioma = -1;


	// Logging variables
	mLogHabilitar = true; // Habilita o log por padrão

    mNumPilotos = 0;
    mMaxPilotos = 0;
    mFase = 0;

	// Full Course Yellow (FCY) variables
    mFCYAtivo = false;
    mContagemFCY = false;
    mTempoFCY = 0.0;
    mContagemParaFCY = 10.0; // Exemplo de tempo para iniciar FCY
    mContagemParaVerde = 10.0; // Exemplo de tempo para bandeira verde
    mVelocidadeFCY = 80.0; // Exemplo de velocidade máxima durante FCY
    mPilotoCausador = -1;
}


void RaceDirectorPlugin::SetEnvironment(const EnvironmentInfoV01& info)
{
    string caminhoBase = info.mPath[0];
    mLogPasta = caminhoBase + "Log\\RaceDirector\\";

    // Cria a pasta se não existir
    CreateDirectoryA( mLogPasta.c_str(), NULL);
}


void RaceDirectorPlugin::StartSession()
{
    CriarLog();

    EscreverLog("Log criado: " + mLogPasta);

    // EscreverLog(
    //     "Controle de largada: " + to_string(mRscEnabled) +
    //     "\t| Marcha: " + to_string(mRscGear) +
    //     "\t| Limitador: " + to_string(mRscLimiter) +
    //     "\t| Vel Maxima : " + to_string(mRscMaxVelKPH) +
    //     "\t| Penalidade : " + to_string(mRscPenalty) +
    //     "\n"
    // );

    EscreverLog("---------- SESSAO INICIADA ----------");
}


void RaceDirectorPlugin::UpdateScoring(const ScoringInfoV01& info)
{
    

    // mFCY.AtualizarEstado(info);

    if (info.mSession >= 10)
    {
        mNumPilotos = info.mNumVehicles;
        mMaxPilotos = info.mMaxPlayers;

        if (mFase != info.mGamePhase)
        {
            mFase = info.mGamePhase;

            if (mDrivers.empty()) {
                InicializarPilotos(info);
            }

            if (mFase == 5) // Fase de corrida
            {
                mTelemEnabled = true; // Habilita atualizacoes de telemetria apenas na fase de corrida

                ostringstream oss;
                oss << "Pista: " << info.mTrackName
                    << "\t| Pilotos: " << info.mNumVehicles
                    << "\t| Sessao: " << info.mSession
                    << "\t| Fase: " << to_string(info.mGamePhase)
                    << "\t| Tempo: " << info.mCurrentET;
                WriteLog(oss.str());

                EscreverLog(info.mCurrentET, "--- CORRIDA INICIADA ---");


            }
            else if (mFase == 6) // Fase de fim de corrida
            {
                EscreverLog(info.mCurrentET, "--- CORRIDA FINALIZADA ---");
            }
        }
    }
}


/*
long RaceDirectorPlugin::WantsTelemetryUpdates()
{
    return mTelemEnabled ? 2 : 0;
}


void RaceDirectorPlugin::UpdateTelemetry(const TelemInfoV01& info)
{
    if (mTelemEnabled)
    {
        CheckStartControl(info); // penalização se necessário
        // CheckDownshift(piloto); // verifica reducao de marcha

        mTelemEnabled = false; // Desabilita atualizacoes de telemetria apos o processamento  
    }
}
*/


bool RaceDirectorPlugin::WantsToDisplayMessage(MessageInfoV01 &msg)
{
    if (mFilaMensagem.empty())
        return false;           // Retorna false se não houver mensagem para enviar

		string cmd = mFilaMensagem.front();
        mFilaMensagem.pop(); // Remove a mensagem da fila após obtê-la

        msg.mDestination = 1;
        msg.mTranslate = 0;
        strncpy(msg.mText, cmd.c_str(), sizeof(msg.mText) -1);

        EscreverLog("Mensagem enviada: " + cmd);

        return true;
}


bool RaceDirectorPlugin::AccessTrackRules(TrackRulesV01& info)
{
    // Aqui você pode chamar sua lógica de FCY, por exemplo:

    FullCourseYellow(info);

    return true; // Retorne true apenas se quiser modificar TrackRulesV01
}


bool RaceDirectorPlugin::GetCustomVariable(long i, CustomVariableV01 &var)
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
    case 1:
    {
        strcpy_s(var.mCaption, "Log.Register");
        var.mNumSettings = 2;
        var.mCurrentSetting = 1;
        return(true);
    }
    case 2:
    {
        strcpy_s(var.mCaption, "FullCourseYellow.Enabled");
        var.mNumSettings = 2;
        var.mCurrentSetting = 1;
        return(true);
    }
    case 3:
    {
        strcpy_s(var.mCaption, "FullCourseYellow.VerifyTimeFCY");
        var.mNumSettings = 10;
        var.mCurrentSetting = 6;
        return(true);
    }
    case 4:
    {
        strcpy_s(var.mCaption, "FullCourseYellow.CountdownFCY");
        var.mNumSettings = 10;
        var.mCurrentSetting = 6;
        return(true);
    }
    case 5:
    {
        strcpy_s(var.mCaption, "FullCourseYellow.VerifyTimeGreen");
        var.mNumSettings = 10;
        var.mCurrentSetting = 6;
        return(true);
    }
    case 6:
    {
        strcpy_s(var.mCaption, "FullCourseYellow.CountdownGreen");
        var.mNumSettings = 10;
        var.mCurrentSetting = 6;
        return(true);
    }
    case 7:
    {
        strcpy_s(var.mCaption, "StartControl.Enabled");
        var.mNumSettings = 2;
        var.mCurrentSetting = 1;
        return(true);
    }
    case 8:
    {
        strcpy_s(var.mCaption, "StartControl.Gear");
        var.mNumSettings = 10;
        var.mCurrentSetting = 2;
        return(true);
    }
    case 9:
    {
        strcpy_s(var.mCaption, "StartControl.Limiter");
        var.mNumSettings = 2;
        var.mCurrentSetting = 1;
        return(true);
    }
    case 10:
    {
        strcpy_s(var.mCaption, "StartControl.MaxVelKPH");
        var.mNumSettings = 301;
        var.mCurrentSetting = 80;
        return(true);
    }
    case 11:
    {
        strcpy_s(var.mCaption, "StartControl.Penalty");
        var.mNumSettings = 64;
        var.mCurrentSetting = -1;
        return(true);
    }
    }
    return(false);
}


void RaceDirectorPlugin::GetCustomVariableSetting(CustomVariableV01 &var, long i, CustomSettingV01 &setting)
{
    if (0 == _stricmp(var.mCaption, " Enabled"))
    {
        if (i == 0)
            sprintf_s(setting.mName, "False");
        else if (i == 1)
            sprintf_s(setting.mName, "True");
    }
    else if (0 == _stricmp(var.mCaption, "Log.Register"))
    {
        if (i == 0)
            sprintf_s(setting.mName, "False");
        else if (i == 1)
            sprintf_s(setting.mName, "True");
    }
    else if (0 == _stricmp(var.mCaption, "FullCourseYellow.Enabled"))
    {
        if (i == 0)
            sprintf_s(setting.mName, "False");
        else if (i == 1)
            sprintf_s(setting.mName, "True");
    }
    else if (0 == _stricmp(var.mCaption, "FullCourseYellow.VerifyTimeFCY"))
    {
        if (i == 0)
            sprintf_s(setting.mName, "Disabled");
        else if (i > 0)
            sprintf_s(setting.mName, "%d", mFCY.mVerifyTimeFCY);
    }
    else if (0 == _stricmp(var.mCaption, "FullCourseYellow.CountdownFCY"))
    {
        if (i == 0)
            sprintf_s(setting.mName, "Disabled");
        else if (i > 0)
            sprintf_s(setting.mName, "%d", mFCY.mCountdownFCY);
    }
    else if (0 == _stricmp(var.mCaption, "FullCourseYellow.VerifyTimeGreen"))
    {
        if (i == 0)
            sprintf_s(setting.mName, "Disabled");
        else if (i > 0)
            sprintf_s(setting.mName, "%d", mFCY.mVerifyTimeGreen);
    }
    else if (0 == _stricmp(var.mCaption, "FullCourseYellow.CountdownGreen "))
    {
        if (i == 0)
            sprintf_s(setting.mName, "Disabled");
        else if (i > 0)
            sprintf_s(setting.mName, "%d", mFCY.mCountdownGreen);
    }
    else if (0 == _stricmp(var.mCaption, "StartControl.Enabled"))
    {
        if (i == 0)
            sprintf_s(setting.mName, "False");
        else if (i == 1)
            sprintf_s(setting.mName, "True");
    }
    else if (0 == _stricmp(var.mCaption, "StartControl.Gear"))
    {
        if (i == 0)
            sprintf_s(setting.mName, "Disabled");
        else if (i > 0)
            sprintf_s(setting.mName, "%d", mRscGear);
    }
    else if (0 == _stricmp(var.mCaption, "StartControl.Limiter"))
    {
        if (i == 0)
            sprintf_s(setting.mName, "Disabled");
        else if (i == 1)
            sprintf_s(setting.mName, "Enabled");
    }
    else if (0 == _stricmp(var.mCaption, "StartControl.MaxVelKPH"))
    {
        if (i == 0)
            sprintf_s(setting.mName, "Disabled");
        else if (i > 0)
            sprintf_s(setting.mName, "%.3f", mRscMaxVelKPH);
    }
    else if (0 == _stricmp(var.mCaption, "StartControl.Penalty"))
    {
        if (i == -3)
            sprintf_s(setting.mName, "Disabled");
        else if (i == -2)
            sprintf_s(setting.mName, "Longest Line");
        else if (i == -1)
            sprintf_s(setting.mName, "Drive-Thru");
        else if (i >= 0 && i <= 60)
            sprintf_s(setting.mName, "Stop&Go %ds", mRscPenalty);
    }
}


void RaceDirectorPlugin::AccessCustomVariable(CustomVariableV01& var)
{
    if (0 == _stricmp(var.mCaption, " Enabled"))
    {
        // Nao faca nada; esta variavel e apenas para o rF2 saber se deve manter o plugin carregado.
    }
    else if (0 == _stricmp(var.mCaption, "Log.Register"))
    {
        mLogEnabled = (var.mCurrentSetting != 0);
    }
    else if (0 == _stricmp(var.mCaption, "FullCourseYellow.Enabled"))
    {
        fcyHabilitar = (var.mCurrentSetting != 0);
    }
    else if (0 == _stricmp(var.mCaption, "FullCourseYellow.VerifyTimeFCY"))
    {
        mFCY.mVerifyTimeFCY = var.mCurrentSetting;
    }
    else if (0 == _stricmp(var.mCaption, "FullCourseYellow.CountdownFCY"))
    {
        fcyContagemFCY = var.mCurrentSetting;
    }
    else if (0 == _stricmp(var.mCaption, "FullCourseYellow.VerifyTimeGreen"))
    {
        mFCY.mVerifyTimeGreen = var.mCurrentSetting;
    }
    else if (0 == _stricmp(var.mCaption, "FullCourseYellow.CountdownGreen"))
    {
        fcyContagemGreen = var.mCurrentSetting;
    }
    else if (0 == _stricmp(var.mCaption, "StartControl.Enabled"))
    {
        mRscEnabled = (var.mCurrentSetting != 0);
    }
    else if (0 == _stricmp(var.mCaption, "StartControl.Gear"))
    {
        mRscGear = var.mCurrentSetting;
    }
    else if (0 == _stricmp(var.mCaption, "StartControl.Limiter"))
    {
        mRscLimiter = var.mCurrentSetting;
    }
    else if (0 == _stricmp(var.mCaption, "StartControl.MaxVelKPH"))
    {
        mRscMaxVelKPH = var.mCurrentSetting;
    }
    else if (0 == _stricmp(var.mCaption, "StartControl.Penalty"))
    {
        mRscPenalty = var.mCurrentSetting;
    }
}


void RaceDirectorPlugin::CheckStartControl(const TelemInfoV01& info)
{
    bool mAddPenalty = false;

    double velMPH = sqrt(info.mLocalVel.x * info.mLocalVel.x + info.mLocalVel.y * info.mLocalVel.y + info.mLocalVel.z * info.mLocalVel.z) * 2.23694;
    double velKPH = sqrt(info.mLocalVel.x * info.mLocalVel.x + info.mLocalVel.y * info.mLocalVel.y + info.mLocalVel.z * info.mLocalVel.z) * 3.6;


    if (mRscEnabled)
    {
        if (velKPH > mRscMaxVelKPH || info.mGear != mRscGear || info.mSpeedLimiter != mRscLimiter)
        {
            mAddPenalty = true;
        }
        else
        {
            mAddPenalty = false;
        }
    }

    if (mAddPenalty)
    {
        ostringstream oss;
        oss << "/addpenalty " << mRscPenalty << " V:" << info.mVehicleName;
        mQueueMessage.push(oss.str());
        WriteLog("Comando adicionado à fila: " + oss.str());
    }

    //driver.mVerificouLargada = true; // Marca que a verificação foi feita
}

/*
void RaceDirectorPlugin::CheckStartControl(DriverInfo& driver)
{
    bool mAddPenalty = false;

    if (driver.mVerificouLargada)
	    return; // Já verificou a largada, não faz nada

    if (!driver.mPit && mRscEnabled)
    {
        if (driver.mVelKPH > mRscMaxVelKPH)
        {
            mAddPenalty = true;
        }
        else if (driver.mMarcha != mRscGear)
        {
            mAddPenalty = true;
        }
        else if (driver.mLimitador != mRscLimiter)
        {
            mAddPenalty = true;
        }
        else
        {
            mAddPenalty = false;
        }
    }

    if (mAddPenalty)
    {
        ostringstream oss;
        oss << "/addpenalty " << mRscPenalty << " " << driver.mNome;
        mQueueMessage.push(oss.str());
        WriteLog("Comando adicionado à fila: " + oss.str());
    }

	driver.mVerificouLargada = true; // Marca que a verificação foi feita
}
*/

/* void RaceDirectorPlugin::CheckDownshift(DriverInfo& piloto)
{
    if (piloto.mDetectouMudanca)
        return;

    if (!piloto.mMarchaLargada && piloto.mMarcha == mStartControlGear)
    {
        piloto.mMarchaLargada = true;
    }

    if (piloto.mMarchaLargada && piloto.mMarcha != piloto.mMarchaAnterior)
    {
        if (piloto.mMarcha < piloto.mMarchaAnterior)
        {
            EscreverLog("REDUÇÃO DETECTADA: " + piloto.mNome +
                " - De " + std::to_string(piloto.mMarchaAnterior) +
                " para " + std::to_string(piloto.mMarcha));
        }

        piloto.mDetectouMudanca = true;
    }

    piloto.mMarchaAnterior = piloto.mMarcha;
}
*/


void RaceDirectorPlugin::FullCourseYellow(TrackRulesV01& info)
{
    switch (mFase)
    {

    case FASE_VERDE:

        info.mStage = TRSTAGE_NORMAL;

        if (info.mYellowFlagDetected)
        {
            EscreverLog(info.mCurrentET, "Yellow detectado. Iniciando pre contagem");

            mFase = FASE_LIMITE;

            mInicioFase = info.mCurrentET;
        }

        break;


    case FASE_LIMITE:

        if (!info.mYellowFlagDetected)
        {
            EscreverLog(info.mCurrentET, "Pre contagem cancelada");

            break;
        }

        if (info.mCurrentET - mInicioFase >= mTempoLimite)
        {
            EscreverLog(info.mCurrentET, "Pre contagem terminada. Entrando em FCY...");

            mFase = FASE_CONTAGEM_FCY;

            mInicioFase = info.mCurrentET;
        }

        break;


    case FASE_CONTAGEM_FCY:

        info.mStage = TRSTAGE_CAUTION_INIT;

        {
            int tempo = (int)(mContagemFCY - (info.mCurrentET - mInicioFase));

            char msg[32];

            sprintf(msg, "FCY em %d", tempo);

            strcpy(info.mMessage, msg);
        }

        if (info.mCurrentET - mInicioFase >= mContagemFCY)
        {
            EscreverLog(info.mCurrentET, "FCY iniciado");

            mFase = FASE_FCY;
        }

        break;


    case FASE_FCY:

        info.mStage = TRSTAGE_CAUTION_UPDATE;

        strcpy(info.mMessage, "FULL COURSE YELLOW");

        info.mMaximumSpeed = mVelocidadeKPH / 3.6f;

        VerificarVelocidade(info);

        if (!info.mYellowFlagDetected)
        {
            EscreverLog(info.mCurrentET, "Saindo do FCY...");

            mFase = FASE_CONTAGEM_GREEN;

            mInicioFase = info.mCurrentET;
        }

        break;


    case FASE_CONTAGEM_GREEN:

        info.mStage = TRSTAGE_CAUTION_UPDATE;

        info.mMaximumSpeed = mVelocidadeKPH / 3.6f;

        {
            int tempo = (int)(mContagemGreen - (info.mCurrentET - mInicioFase));

            char msg[32];

            sprintf(msg, "Green em %d", tempo);

            strcpy(info.mMessage, msg);
        }

        if (info.mCurrentET - mInicioFase >= mContagemGreen)
        {
            EscreverLog(info.mCurrentET, "BANDEIRA VERDE");

            mFase = FASE_VERDE;

            strcpy(info.mMessage, ""); // volta à normalidade
        }

        break;

    } // switch mFase

    return true;
}


void RaceDirectorPlugin::InicializarPilotos(const ScoringInfoV01& info)
{
    mDrivers.clear();
    mDrivers.reserve(info.mMaxPlayers);

    for (int i = 0; i < info.mNumVehicles; ++i)
    {
        const VehicleScoringInfoV01& vsi = info.mVehicle[i];

        DriverInfo d;
        d.mIndex = i;
        d.mID = vsi.mID;
        d.mNome = vsi.mDriverName;
        d.mVeiculo = vsi.mVehicleName;
        d.mClasse = vsi.mVehicleClass;
        d.mPit = vsi.mInPits;
        d.mVelKPH = 0.0;
        d.mVelMPH = 0.0;
        d.mMarcha = 0;
        d.mLimitador = 0;
        d.mVerificouLargada = false;
        mDrivers.push_back(d);
    }

    mLog.WriteLog("Pilotos inicializados: " + to_string(mDrivers.size()));
}


void RaceDirectorPlugin::CriarLog()
{
    time_t rawTime;
    time(&rawTime);             // obtém o tempo atual em segundos
    tm now;
    localtime_s(&now, &rawTime); // converte para struct tm local

    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y_%m_%d-%H_%M_%S", &now);

    ostringstream oss;
    oss << "RD-" << buffer << ".txt";

    mLogPasta += oss.str();
}


void RaceDirectorPlugin::EscreverLog(double tempo, const char* msg) const
{
    ofstream mLogArquivo(mLogPasta, ios::app);
    if (mLogArquivo.is_open())
    {
        mLogArquivo << tempo << ": " << msg << endl;
    }
}


// plugin information

extern "C" __declspec(dllexport)
const char* __cdecl GetPluginName() { return("Race Director - 2025.06.14"); }

extern "C" __declspec(dllexport)
PluginObjectType __cdecl GetPluginType() { return(PO_INTERNALS); }

extern "C" __declspec(dllexport)
int __cdecl GetPluginVersion() { return(7); }  // Funcionalidade do InternalsPluginV07 (se voce mudar esse valor de retorno, deve derivar da classe apropriada!)

extern "C" __declspec(dllexport)
PluginObject* __cdecl CreatePluginObject() { return(static_cast<PluginObject*>(new RaceDirectorPlugin)); }

extern "C" __declspec(dllexport)
void __cdecl DestroyPluginObject(PluginObject* obj) { delete(static_cast<RaceDirectorPlugin*>(obj)); }