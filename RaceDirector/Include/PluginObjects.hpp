//�������������������������������������������
//�                                                                         �
//� Module: Header file for plugin object types                             �
//�                                                                         �
//� Description: interface declarations for plugin objects                  �
//�                                                                         �
//� This source code module, and all information, data, and algorithms      �
//� associated with it, are part of isiMotor Technology (tm).               �
//�                 PROPRIETARY AND CONFIDENTIAL                            �
//� Copyright (c) 1996-2013 Image Space Incorporated.  All rights reserved. �
//�                                                                         �
//� Change history:                                                         �
//�   tag.2008.02.15: created                                               �
//�                                                                         �
//�������������������������������������������

#ifndef _PLUGIN_OBJECTS_HPP_
#define _PLUGIN_OBJECTS_HPP_


// O rF atualmente usa empacotamento de 4 bytes ... qualquer que seja o empacotamento atual sera
// restaurado no final deste include com outro #pragma.
#pragma pack( push, 4 )


//�������������������������������������������
//� tipos de plugins                                                        �
//�������������������������������������������

enum PluginObjectType
{
  PO_INVALID      = -1,
  //-------------------
  PO_GAMESTATS    =  0,
  PO_NCPLUGIN     =  1,
  PO_IVIBE        =  2,
  PO_INTERNALS    =  3,
  PO_RFONLINE     =  4,
  //-------------------
  PO_MAXIMUM
};


//�������������������������������������������
//�  PluginObject                                                          �
//�    - interface usada pelas classes de plugin.                          �
//�������������������������������������������

class PluginObject
{
 private:

  class PluginInfo *mInfo;             // usado pelo executavel principal para obter informacoes sobre o plugin que implementa este objeto

 public:

  void SetInfo( class PluginInfo *p )  { mInfo = p; }        // usado pelo executavel principal
  class PluginInfo *GetInfo() const    { return( mInfo ); }  // usado pelo executavel principal
  class PluginInfo *GetInfo()          { return( mInfo ); }  // usado pelo executavel principal
};


//�������������������������������������������
//� typedefs para funcoes DLL - mais facil usar um typedef do que digitar   �
//� fora a sintaxe complicada para declarar e converter ponteiros de funcao �
//�������������������������������������������

typedef const char *      ( __cdecl *GETPLUGINNAME )();
typedef PluginObjectType  ( __cdecl *GETPLUGINTYPE )();
typedef int               ( __cdecl *GETPLUGINVERSION )();
typedef PluginObject *    ( __cdecl *CREATEPLUGINOBJECT )();
typedef void              ( __cdecl *DESTROYPLUGINOBJECT )( PluginObject *obj );


//�������������������������������������������
//�������������������������������������������

// Veja #pragma no topo do arquivo
#pragma pack( pop )

#endif // _PLUGIN_OBJECTS_HPP_

