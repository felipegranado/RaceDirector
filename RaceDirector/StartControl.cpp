#include "StartControl.hpp"

Log lg2;

StartControl::StartControl() {
    
    // Custom variables
    mLigado = -1;
    mLimitador = -1;
    mMarcha = -1;
    mMaxVelKPH = -1;
    mPenalidade = -4;

    mMensagem = "";
}

StartControl::~StartControl() {}


// Getters
int StartControl::GetLigado() const {
    return mLigado;
}

int StartControl::GetMarchaLimite() const {
    return mMarcha;
}

int StartControl::GetLimitador() const {
    return mLimitador;
}

double StartControl::GetMaxVelKPH() const {
    return mMaxVelKPH;
}

int StartControl::GetPenalidade() const {
    return mPenalidade;
}

const string& StartControl::GetMensagem() const {
    return mMensagem;
}


// Setters
void StartControl::SetLigado(int info) {
    mLigado = info;
}

void StartControl::SetMarchaLimite(int info) {
    mMarcha = info;
}

void StartControl::SetLimitador(int info) {
    mLimitador = info;
}

void StartControl::SetMaxVelKPH(double info) {
    mMaxVelKPH = info;
}

void StartControl::SetPenalidade(int info) {
    mPenalidade = info;
}

void StartControl::SetMensagem(const string& msg) {
    mMensagem = msg;
}


// Registro de Pilotos
/*
void StartControl::AtualizarPilotoScr(const ScoringInfoV01& scr)
{
    for (int i = 0; i < scr.mNumVehicles; ++i)
    {  
        const VehicleScoringInfoV01& vsi = scr.mVehicle[i];   // Atualiza somente dentro dos limites atuais de veículos

        PilotoInfo& piloto = scPiloto[i];

        piloto.vsiTempo = scr.mCurrentET;
        piloto.vsiID = vsi.mID;
        piloto.vsiPiloto2, vsi.mDriverName;
        piloto.vsiVeiculo2 = vsi.mVehicleName;
        piloto.vsiPits = vsi.mInPits;
    }
}

void StartControl::AtualizarPilotoTlm(const TelemInfoV01& tlm)
{
    double velKPH = sqrt(tlm.mLocalVel.x * tlm.mLocalVel.x +
        tlm.mLocalVel.y * tlm.mLocalVel.y +
        tlm.mLocalVel.z * tlm.mLocalVel.z) * 3.6;

    for (long i = 0; i < scNumPilotos; ++i)
    {
        PilotoInfo& piloto = scPiloto[i];

        if (piloto.vsiID == tlm.mID)
        {
            piloto.tlmTempo = tlm.mElapsedTime;
            piloto.tlmID = tlm.mID;
            piloto.tlmVelKPH = velKPH;
            piloto.tlmMarcha = tlm.mGear;
            piloto.tlmLimitador = tlm.mSpeedLimiter;
        }
    }
}
*/

void StartControl::AtualizarPilotoScr2(const VehicleScoringInfoV01& vsi)
{
    double velKPH = sqrt(vsi.mLocalVel.x * vsi.mLocalVel.x +
        vsi.mLocalVel.y * vsi.mLocalVel.y +
        vsi.mLocalVel.z * vsi.mLocalVel.z) * 3.6;

    PilotoInfo piloto;  
    
    piloto.vsiID = vsi.mID;
    piloto.vsiPiloto2 = vsi.mDriverName;
    piloto.vsiVeiculo2 = vsi.mVehicleName;
    piloto.vsiPits = vsi.mInPits;

    mPilotos.push_back(piloto);

    ostringstream lista;
    lista << "ID: " << piloto.vsiID
        << "\t| Piloto: " << piloto.vsiPiloto2
        << "\t| Carro: " << piloto.vsiVeiculo2
        << "\t| Pits: " << piloto.vsiPits;
    lg2.EscreverLog2(lista.str());
}

void StartControl::AtualizarPilotoTlm2(const TelemInfoV01& tlm)
{
    

    for (auto& piloto : mPilotos) {

        PilotoInfo piloto;

        if (piloto.vsiID == tlm.mID) {
            piloto.tlmMarcha = tlm.mGear;
            piloto.tlmLimitador = tlm.mSpeedLimiter;

            lg2.EscreverLog2(
                "ID: " + to_string(piloto.vsiID) +
                "\t| Piloto: " + piloto.vsiPiloto2 +
                "\t| Veiculo: " + piloto.vsiVeiculo2 +
                "\t| Vel KPH: " + to_string(piloto.vsiVelKPH) +
                "\t| Marcha: " + to_string(piloto.tlmMarcha) +
                "\t| Limitador: " + to_string(piloto.tlmLimitador)
            );
        }
        Verificador(piloto);
    }
    /*
    for (int i = 0; i < scNumPilotos; ++i)
    {
        PilotoInfo piloto;

        if (piloto.vsiID == tlm.mID)
        {
            piloto.tlmTempo = tlm.mElapsedTime;
            piloto.tlmID = tlm.mID;
            piloto.tlmVelKPH = velKPH;
            piloto.tlmMarcha = tlm.mGear;
            piloto.tlmLimitador = tlm.mSpeedLimiter;


            log.EscreverLog2
            (
                "Tempo: " + to_string(piloto.vsiTempo) +
                "\t| ID: " + to_string(piloto.vsiID) +
                "\t| Piloto: " + piloto.vsiPiloto2 +
                "\t| Veiculo: " + piloto.vsiVeiculo2 +
                "\t| Vel KPH: " + to_string(piloto.tlmVelKPH) +
                "\t| Marcha: " + to_string(piloto.tlmMarcha) +
                "\t| Limitador: " + to_string(piloto.tlmLimitador)
            );
        }

        Verificador(piloto);

    }
    */
}

void StartControl::Verificador(PilotoInfo& piloto)
{
    bool aplicaPenalidade = false;

    if (piloto.vsiPits == 0)
    {
        if (mLimitador != 0 && piloto.tlmLimitador != mLimitador)
        {
            aplicaPenalidade = true;
        }
        else if (mMarcha != 0 && piloto.tlmMarcha != mMarcha)
        {
            aplicaPenalidade = true;
        }
        else if (mMaxVelKPH != 0 && piloto.vsiVelKPH != mMaxVelKPH)
        {
            aplicaPenalidade = true;
        }
    }

    if (aplicaPenalidade && mPenalidade > -3)
    {
        char buffer[128];

        snprintf(buffer, sizeof(buffer), "/addpenalty %d %s", mPenalidade, piloto.vsiPiloto2.c_str());

        mMensagem = buffer;

        lg2.EscreverLog2("Penalidade aplicada: " + to_string(mPenalidade) + " para o piloto: " + piloto.vsiPiloto2);
    }
}