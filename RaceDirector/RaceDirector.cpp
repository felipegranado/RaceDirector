////////////////////////////////
// Criado por: Felipe Granado //
////////////////////////////////

#define _CRT_SECURE_NO_WARNINGS

#include "RaceDirector.hpp"

// plugin information

extern "C" __declspec( dllexport )
const char * __cdecl GetPluginName()                   { return( "Race Director - 2025.06.14" ); }

extern "C" __declspec( dllexport )
PluginObjectType __cdecl GetPluginType()               { return( PO_INTERNALS ); }

extern "C" __declspec( dllexport )
int __cdecl GetPluginVersion()                         { return( 7 ); }  // Funcionalidade do InternalsPluginV07 (se voce mudar esse valor de retorno, deve derivar da classe apropriada!)

extern "C" __declspec( dllexport )
PluginObject * __cdecl CreatePluginObject()            { return( static_cast< PluginObject * >( new RaceDirectorPlugin ) ); }

extern "C" __declspec( dllexport )
void __cdecl DestroyPluginObject( PluginObject *obj )  { delete( static_cast< RaceDirectorPlugin * >( obj ) ); }


RaceDirectorPlugin::RaceDirectorPlugin()
{

	// Logging variables
	mLogEnabled = true; // Habilita o log por padrão
	mIdioma = 0; // 0 = Inglês, 1 = Português

    mNumPilotos = 0;
    mMaxPilotos = 0;
    mFase = 0;

    mAtualizaTelemetria = false;
    mAplicaPenalidade = false;


	// Start Control variables
    mStartControlEnabled = -1;
    mStartControlGear = -2;
	mStartControlLimiter = -1;
	mStartControlMaxVelKPH = -1.0;
	mStartControlPenalty = -3; // -3 = Disabled, -2 = Longest Line, -1 = Drive-Thru, 0-60 = Stop&Go


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
    mLogLocal = caminhoBase + "Log\\RaceDirector\\";

    // Cria a pasta se não existir
    CreateDirectoryA(mLogLocal.c_str(), NULL);
}


void RaceDirectorPlugin::StartSession()
{
    GerarLog();

    EscreverLog("Log criado: " + mLogLocal);

    EscreverLog(
        "Controle de largada: " + to_string(mStartControlEnabled) +
        "\t| Marcha: " + to_string(mStartControlGear) +
        "\t| Limitador: " + to_string(mStartControlLimiter) +
        "\t| Vel Maxima : " + to_string(mStartControlMaxVelKPH) +
        "\t| Penalidade : " + to_string(mStartControlPenalty) +
        "\n"
    );

    EscreverLog("---------- SESSAO INICIADA ----------");
}


void RaceDirectorPlugin::UpdateScoring(const ScoringInfoV01& info)
{
    if (info.mSession >= 10)
    {
        mNumPilotos = info.mNumVehicles;
        mMaxPilotos = info.mMaxPlayers;

        if (mFase != info.mGamePhase)
        {
            mFase = info.mGamePhase;

            mPilotos.clear(); // <--- IMPORTANTE
            mPilotos.reserve(mMaxPilotos);

            for (int i = 0; i < mNumPilotos; ++i)
            {
                const VehicleScoringInfoV01& vsi = info.mVehicle[i];

                PilotoInfo piloto;
                piloto.mIndex = i;
                piloto.mID = vsi.mID;
                piloto.mNome = vsi.mDriverName;
                piloto.mVeiculo = vsi.mVehicleName;
                piloto.mClasse = vsi.mVehicleClass;
                piloto.mPit = vsi.mInPits;
                piloto.mVelKPH = 0.0;
                piloto.mVelMPH = 0.0;
                piloto.mMarcha = 0;
                piloto.mLimitador = 0;

                mPilotos.push_back(piloto);
            }

            if (mFase == 5) // Fase de corrida
            {
                mAtualizaTelemetria = true; // Habilita atualizacoes de telemetria apenas na fase de corrida

                ostringstream oss;
                oss << "Pista: " << info.mTrackName
                    << "\t| Pilotos: " << info.mNumVehicles
                    << "\t| Sessao: " << info.mSession
                    << "\t| Fase: " << to_string(info.mGamePhase)
                    << "\t| Tempo: " << info.mCurrentET;
                EscreverLog(oss.str());

                EscreverLog("---------- CORRIDA INICIADA ----------");


            }
            else if (mFase == 6) // Fase de fim de corrida
            {
                EscreverLog("---------- CORRIDA FINALIZADA ----------");
            }
        }
    }
}

bool RaceDirectorPlugin::AccessTrackRules(TrackRulesV01& info)
{
    // info.mNumParticipants: quantidade de participantes
    // info.mParticipant: ponteiro para o array de TrackRulesParticipantV01

    // Exemplo: Atualizar PilotoInfo com dados do TrackRules
    for (int i = 0; i < info.mNumParticipants; ++i) {
        const TrackRulesParticipantV01& trp = info.mParticipant[i];
        // Procure o piloto correspondente pelo mID
        for (auto& piloto : mPilotos) {
            if (piloto.mID == trp.mID) {
                piloto.mFrozenOrder = trp.mFrozenOrder;
                piloto.mPlace = trp.mPlace;
                piloto.mYellowSeverity = trp.mYellowSeverity;
                piloto.mCurrentRelativeDistance = trp.mCurrentRelativeDistance;
                piloto.mRelativeLaps = trp.mRelativeLaps;
                piloto.mColumnAssignment = trp.mColumnAssignment;
                piloto.mPositionAssignment = trp.mPositionAssignment;
                piloto.mPitsOpen = trp.mPitsOpen;
                piloto.mUpToSpeed = trp.mUpToSpeed;
                piloto.mGoalRelativeDistance = trp.mGoalRelativeDistance;
                piloto.mMessage = trp.mMessage;
            }
        }
    }

    // Aqui você pode chamar sua lógica de FCY, por exemplo:
    CheckFullCourseYellow(info.mParticipant, info.mNumParticipants, info.mCurrentET);

    return false; // Retorne true apenas se quiser modificar TrackRulesV01
}


long RaceDirectorPlugin::WantsTelemetryUpdates()
{
    return mAtualizaTelemetria ? 2 : 0;
}


void RaceDirectorPlugin::UpdateTelemetry(const TelemInfoV01& info)
{
    double velMPH = sqrt(info.mLocalVel.x * info.mLocalVel.x + info.mLocalVel.y * info.mLocalVel.y + info.mLocalVel.z * info.mLocalVel.z) * 2.23694;
    double velKPH = sqrt(info.mLocalVel.x * info.mLocalVel.x + info.mLocalVel.y * info.mLocalVel.y + info.mLocalVel.z * info.mLocalVel.z) * 3.6;
    
    for(auto& piloto : mPilotos)
    {
        if (piloto.mID == info.mID)
        {
            piloto.mVelMPH = velMPH;
            piloto.mVelKPH = velKPH;
            piloto.mMarcha = info.mGear;
            piloto.mLimitador = info.mSpeedLimiter;

            if (!piloto.mJaLogado)
            {
                EscreverLog("ID: " + to_string(piloto.mID) + "\t | Piloto: " + piloto.mNome + "\t| Vel KPH: " + to_string(piloto.mVelKPH) + "\t| Marcha: " + to_string(piloto.mMarcha) + "\t| Limitador: " + to_string(piloto.mLimitador));

                piloto.mJaLogado = true;
            }

            CheckStartControl(piloto); // penalização se necessário
			CheckDownshift(piloto); // verifica reducao de marcha
        }
	}
	mAtualizaTelemetria = false; // Desabilita atualizacoes de telemetria apos o processamento  
}


bool RaceDirectorPlugin::WantsToDisplayMessage(MessageInfoV01 &msgInfo)
{
    if (!mFilaMensagens.empty())
    {
		mMensagem = mFilaMensagens.front();
		mFilaMensagens.pop(); // Remove a mensagem da fila após obtê-la

        msgInfo.mDestination = 1;
        msgInfo.mTranslate = 0;
        strncpy_s(msgInfo.mText, mMensagem.c_str(), sizeof(msgInfo.mText) -1);

        EscreverLog("Mensagem enviada: " + mMensagem);

        return true;
    }
    else
    {
        return false; // Retorna false se não houver mensagem para enviar
	}
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
        strcpy_s(var.mCaption, "Language");
        var.mNumSettings = 2;
        var.mCurrentSetting = 0;
        return(true);
    }
    case 3:
    {
        strcpy_s(var.mCaption, "StartControl.Enabled");
        var.mNumSettings = 2;
        var.mCurrentSetting = 1;
        return(true);
    }
    case 4:
    {
        strcpy_s(var.mCaption, "StartControl.Gear");
        var.mNumSettings = 10;
        var.mCurrentSetting = 2;
        return(true);
    }
    case 5:
    {
        strcpy_s(var.mCaption, "StartControl.Gear#");
        var.mNumSettings = 1;
        var.mCurrentSetting = 0;
        return(true);
    }
    case 6:
    {
        strcpy_s(var.mCaption, "StartControl.Limiter");
        var.mNumSettings = 2;
        var.mCurrentSetting = 1;
        return(true);
    }
    case 7:
    {
        strcpy_s(var.mCaption, "StartControl.Limiter#");
        var.mNumSettings = 1;
        var.mCurrentSetting = 0;
        return(true);
    }
    case 8:
    {
        strcpy_s(var.mCaption, "StartControl.MaxVelKPH");
        var.mNumSettings = 301;
        var.mCurrentSetting = 80;
        return(true);
    }
    case 9:
    {
        strcpy_s(var.mCaption, "StartControl.MaxVelKPH#");
        var.mNumSettings = 1;
        var.mCurrentSetting = 0;
        return(true);
    }
    case 10:
    {
        strcpy_s(var.mCaption, "StartControl.Penalty");
        var.mNumSettings = 64;
        var.mCurrentSetting = -1;
        return(true);
    }
    case 11:
    {
        strcpy_s(var.mCaption, "StartControl.Penalty#");
        var.mNumSettings = 1;
        var.mCurrentSetting = 0;
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
    else if (0 == _stricmp(var.mCaption, "Language"))
    {
        if (i == 0)
            sprintf_s(setting.mName, "EN");
        else if (i == 1)
            sprintf_s(setting.mName, "PT");
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
            sprintf_s(setting.mName, "%d", mStartControlGear);
    }
    else if (0 == _stricmp(var.mCaption, "StartControl.Gear#"))
    {
        if (i == 0)
            sprintf_s(setting.mName, "Gear must be in. 0 = Disable.");
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
            sprintf_s(setting.mName, "%.3f", mStartControlMaxVelKPH);
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
            sprintf_s(setting.mName, "Stop&Go %ds", mStartControlPenalty);
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
    else if (0 == _stricmp(var.mCaption, "Language"))
    {
        mIdioma = var.mCurrentSetting;
    }
    else if (0 == _stricmp(var.mCaption, "StartControl.Enabled"))
    {
        mStartControlEnabled = (var.mCurrentSetting != 0);
    }
    else if (0 == _stricmp(var.mCaption, "StartControl.Gear"))
    {
        mStartControlGear = var.mCurrentSetting;
    }
    else if (0 == _stricmp(var.mCaption, "StartControl.Limiter"))
    {
        mStartControlLimiter = var.mCurrentSetting;
    }
    else if (0 == _stricmp(var.mCaption, "StartControl.MaxVelKPH"))
    {
        mStartControlMaxVelKPH = var.mCurrentSetting;
    }
    else if (0 == _stricmp(var.mCaption, "StartControl.Penalty"))
    {
        mStartControlPenalty = var.mCurrentSetting;
    }
}


void RaceDirectorPlugin::GerarLog()
{
    time_t rawTime;
    time(&rawTime);             // obtém o tempo atual em segundos
    tm now;
    localtime_s(&now, &rawTime); // converte para struct tm local

    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y_%m_%d-%H_%M_%S", &now);

    std::ostringstream oss;
    oss << "RD-" << buffer << ".txt";

    mLogLocal += oss.str();
}


void RaceDirectorPlugin::EscreverLog(const string msg) const
{
    ofstream mLogArquivo(mLogLocal, std::ios::app);
    if (mLogArquivo.is_open())
    {
        mLogArquivo << msg << endl;
    }
}


void RaceDirectorPlugin::CheckStartControl(PilotoInfo& piloto)
{
    if (piloto.mVerificouLargada);
	    return; // Já verificou a largada, não faz nada

    if (!piloto.mPit && mStartControlEnabled)
    {
        if (piloto.mVelKPH > mStartControlMaxVelKPH)
        {
            mAplicaPenalidade = true;
        }
        else if (piloto.mMarcha != mStartControlGear)
        {
            mAplicaPenalidade = true;
        }
        else if (piloto.mLimitador != mStartControlLimiter)
        {
            mAplicaPenalidade = true;
        }
        else
        {
            mAplicaPenalidade = false;
        }
    }

    if (mAplicaPenalidade)
    {
        ostringstream oss;
        oss << "/addpenalty " << mStartControlPenalty << " " << piloto.mNome;
        mFilaMensagens .push(oss.str());
		EscreverLog("Comando adicionado à fila: " + oss.str());
    }

	piloto.mVerificouLargada = true; // Marca que a verificação foi feita
}


void RaceDirectorPlugin::CheckDownshift(PilotoInfo& piloto)
{
    if (piloto.mDetectouMudanca)
        return;

    if (!piloto.mMarchaLargada &&
        piloto.mMarcha == mStartControlGear)
    {
        piloto.mMarchaLargada = true;
    }

    if (piloto.mMarchaLargada &&
        piloto.mMarcha != piloto.mMarchaAnterior)
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


void RaceDirectorPlugin::CheckFullCourseYellow(const TrackRulesParticipantV01* participants, int numParticipants, double currentET)
{
    // 1. Detectar necessidade de FCY
    double somaYellow = 0.0;
    int causador = -1;
    float maiorYellow = 0.0f;

    for (int i = 0; i < numParticipants; ++i) {
        float yellow = participants[i].mYellowSeverity;
        somaYellow += yellow;
        if (yellow > maiorYellow) {
            maiorYellow = yellow;
            causador = participants[i].mID;
        }
    }

    // 2. Acionar FCY se necessário
    if (!mFCYAtivo && somaYellow > 1.0f) // 1.0f é um exemplo de threshold, ajuste conforme necessário
    {
        mFCYAtivo = true;
        mTempoFCY = currentET + mContagemParaFCY;
        mPilotoCausador = causador;
        EscreverLog("FCY acionado! Causador: " + to_string(mPilotoCausador));
        mFilaMensagens.push("ATENÇÃO: FULL COURSE YELLOW em instantes!");
    }

    // 3. Durante FCY, monitorar pilotos
    if (mFCYAtivo && currentET >= mTempoFCY)
    {
        for (auto& piloto : mPilotos)
        {
            if (!piloto.mPit)
            {
                if (piloto.mVelKPH > mVelocidadeFCY || piloto.mLimitador == 0)
                {
                    ostringstream oss;
                    oss << "/addpenalty " << mStartControlPenalty << " " << piloto.mNome;
                    mFilaMensagens.push(oss.str());
                    EscreverLog("Penalidade FCY: " + piloto.mNome);
                }
            }
        }
        // Se causador entrou no pit, iniciar contagem para bandeira verde
        for (auto& piloto : mPilotos)
        {
            if (piloto.mID == mPilotoCausador && piloto.mPit)
            {
                if (!mContagemFCY) {
                    mContagemFCY = true;
                    mTempoFCY = currentET + mContagemParaVerde;
                    EscreverLog("Causador entrou no pit. Contagem para bandeira verde iniciada.");
                    mFilaMensagens.push("Causador do FCY entrou no pit. Green em breve!");
                }
            }
        }
    }

    // 4. Voltar à bandeira verde
    if (mContagemFCY && currentET >= mTempoFCY)
    {
        mFCYAtivo = false;
        mContagemFCY = false;
        mPilotoCausador = -1;
        EscreverLog("Bandeira verde! FCY encerrado.");
        mFilaMensagens.push("Bandeira verde! FCY encerrado.");
        // Aqui você pode alterar mFase/mGamePhase se necessário
    }
}