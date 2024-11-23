#ifndef _LOG_HPP_
#define _LOG_HPP_

#include "InternalsPlugin.hpp"  // classe base da qual os objetos de plugin devem deriva
#include "RaceDirector.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>


using namespace std;


class Log
{
  public:

	  Log();
	  ~Log();

	  // Getter
	  int GetLog() const;
	  const string& GetLocal() const;

	  // Setter
	  void SetLog(int log);
	  void SetLocal(const string& local);


	  void SetNomeArquivo();

	  // void EscreverLog(const char* openStr, const char* fmt, ...);
	  void EscreverLog2(const string msg) const;

	  void Limpar();

  private:

	  int mLog;
	  string mLocal;
};

#endif // _LOG_HPP_