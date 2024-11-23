#ifndef STARTCONTROL_HPP_
#define STARTCONTROL_HPP_

#include "InternalsPlugin.hpp"  // classe base da qual os objetos de plugin devem deriva
#include "RaceDirector.hpp"
#include "Log.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

/*
struct PilotoInfo {
    long vsiID;
    string vsiPiloto2;
    string vsiVeiculo2;
    bool vsiPits;
    double vsiVelKPH;

    int tlmMarcha;
    unsigned char tlmLimitador;
};
*/

class StartControl
{
  public:

    StartControl();
    ~StartControl();

    // Getters
    int GetLigado() const;
    int GetMarchaLimite() const;
    int GetLimitador() const;
    double GetMaxVelKPH() const;
    int GetPenalidade() const;
    const string& GetMensagem() const;

    // Setters
    void SetLigado(int info);
    void SetMarchaLimite(int info);
    void SetLimitador(int info);
    void SetMaxVelKPH(double info);
    void SetPenalidade(int info);
    void SetMensagem(const string& msg);


    void AtualizarPilotoScr2(const VehicleScoringInfoV01& vsi);            // Atualizar PilotoInfo com Scoring Data    
    void AtualizarPilotoTlm2(const TelemInfoV01& tlm);              // Atualizar PilotoInfo com Telem Data

  private:

    // Custom variables
    int mLigado;
    int mLimitador;
    int mMarcha;
    double mMaxVelKPH;
    int mPenalidade;

    // Penalty Message
    string mMensagem;

    long mMaxPilotos;
    long mNumPilotos;

    /*
    vector<PilotoInfo> mPilotos;
    */
 
    // void Verificador(PilotoInfo& piloto);                      // Verifica o grid na largada

};

#endif // STARTCONTROL_HPP_