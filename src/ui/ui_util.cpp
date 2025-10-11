#include "ui_util.hpp"
#include <sstream>
#include <ACP_Ray2.h>

std::string SPO_Name(HIE_tdstSuperObject* spo) {

  std::stringstream label;


  if (spo->ulType == HIE_C_Type_Actor) {

    HIE_tdstEngineObject* actor = HIE_M_hSuperObjectGetActor(spo);
    if (actor == nullptr || actor->hStandardGame == nullptr) {
      label << "null";
      return label.str();
    }
    GAM_tdxObjectType lPersonalType = HIE_M_lActorGetPersonalType(actor);

    auto familyName = HIE_fn_szGetObjectFamilyName(spo);
    auto modelName = HIE_fn_szGetObjectModelName(spo);

    bool isAlways = lPersonalType == GAM_C_InvalidObjectType || lPersonalType >= GAM_C_AlwaysObjectType;

    if (isAlways) {
      label << "[Alw] ";
    }

    if (GAM_g_stEngineStructure->g_hMainActor == spo) {
      label << "[Main] ";
    }

    if (familyName != nullptr) {
      label << familyName << " / ";
    } else {
      label << "null family!" << " / ";
    }
    if (modelName != nullptr) {
      label << modelName << " / ";
    } else {
      label << "null model!" << " / ";
    }

    if (isAlways) {
      label << (lPersonalType - GAM_C_AlwaysObjectType);
    }
    else {
      auto name = HIE_fn_szGetObjectPersonalName(spo);
      if (name != nullptr) {
        label << name;
      }
      else {
        label << "null";
      }
    }

  }
  else {
    switch (spo->ulType) {
      case HIE_C_Type_Sector: label << "Sector "; break;
      case HIE_C_Type_IPO: label << "IPO "; break;
      case HIE_C_Type_PO: label << "PO "; break;
      case HIE_C_Type_Actor: label << "Actor "; break;
    }
  }

  label << " @0x" << std::hex << std::uppercase << (void*)spo;

  return label.str();
}

ZDX_tdeCollSetPrivilegedActivation fn_eGetPrivilegedActivationZoneOfCharacter(unsigned char _ucZoneType,
  HIE_tdstSuperObject* _hCharacter,
  ACP_tdxIndex _xIndex)
{
  ZDX_tdstCollSet* hCollSet = _hCharacter->hLinkedObject.p_stActor->hCollSet;
  /*if (hCollSet)
    return(CS_fn_eGetPrivilegedActivationZone(_ucZoneType, hCollSet, _xIndex));*/
  return ZDX_E_edmNumberOfPrivilegedActivation;
}


bool ZDX_IsActive(HIE_tdstSuperObject* _hCharacter,
  unsigned char _ucZoneType,
  unsigned char _ucZoneNumber)
{
  ZDX_tdstGeometricZdxList* p_stGeoZdxList;
  ZDX_tdeCollSetPrivilegedActivation ePrivilege;
  ZDX_tdstZoneSetList* p_stZoneSet;
  ZDX_tdstZdxList* p_stZdxList = NULL;
  ZDX_tdstZdxIndexList* p_stZdxIndex;
  ACP_tdxIndex					xI;
  int								iIndex, iMax;
  /*

  ePrivilege = fn_eGetPrivilegedActivationZoneOfCharacter(_ucZoneType, _hCharacter, _ucZoneNumber);
  if (ePrivilege == edmPriviligedActivated) 
  {
    p_stZdxList = (tdstZdxList*)CS_fn_hGetZdxList(_ucZoneType, _hCharacter);
    LST2_M_StaticGetElementNumber(&p_stZdxList->hGeoZdxList, p_stGeoZdxList, _ucZoneNumber, xI);
    return(p_stGeoZdxList->hGeoObj);
  }
  else if (ePrivilege == edmPriviligedDefault)
  {
    /* search on current activation*//*
    p_stZoneSet = fn_p_stGetCurrentZoneSetOfType2(_ucZoneType, _hCharacter);
    if (p_stZoneSet)
    {
      p_stZdxIndex = LST2_M_StaticGetFirstElement(&p_stZoneSet->hZdxIndexList);
      iMax = LST2_M_StaticGetNumberOfElements(&p_stZoneSet->hZdxIndexList);
      iIndex = 0;
      while ((iIndex < iMax) && (_ucZoneNumber > fn_uwGetIndexFromZdxIndexList(p_stZdxIndex)))
      {
        p_stZdxIndex = LST2_M_StaticGetNextElement(p_stZdxIndex);
        iIndex++;
      }
      if ((iIndex < iMax) && (_ucZoneNumber == fn_uwGetIndexFromZdxIndexList(p_stZdxIndex)))
      {
        p_stZdxList = (tdstZdxList*)CS_fn_hGetZdxList(_ucZoneType, _hCharacter);
        LST2_M_StaticGetElementNumber(&p_stZdxList->hGeoZdxList, p_stGeoZdxList, _ucZoneNumber, xI);
        return(p_stGeoZdxList->hGeoObj);
      }
    }
  }*/
  return NULL;
}