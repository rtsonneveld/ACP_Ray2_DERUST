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
    
    label << "0x" << std::hex << std::uppercase << (void*)spo;
  }

  return label.str();
}