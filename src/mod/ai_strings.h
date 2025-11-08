#pragma once
#include "ACP_Ray2.h"

#ifdef __cplusplus
extern "C" {
#endif

  // Lookup functions
  const char* AI_GetNodeString(const AI_tdstNodeInterpret* node);
  const char* AI_GetTypeInterpretString(AI_tdeTypeInterpret value);
  const char* AI_GetVariableTypeString(AI_tdeVariableType value);
  const char* AI_GetDsgVarTypeString(AI_tdeDsgVarType value);
  const char* AI_GetKeyWordString(AI_tdeKeyWordId value);
  const char* AI_GetOperatorString(AI_tdeOperator value);
  const char* AI_GetFunctionString(AI_tdeFuncId value);
  const char* AI_GetProcedureString(AI_tdeProcedureId value);
  const char* AI_GetConditionString(AI_tdeCondition value);
  const char* AI_GetFieldString(AI_tdeField value);
  const char* AI_GetMetaActionString(AI_tdeMetaAction value);

  extern const char* AI_KeyWordIdStrings[];
  extern const char* AI_MetaActionStrings[];
  extern const char* AI_OperatorStrings[];
  extern const char* AI_ProcedureStrings[];
  extern const char* AI_FunctionStrings[];
  extern const char* AI_FieldStrings[];
  extern const char* AI_ConditionStrings[];
  extern const char* AI_DsgVarTypeStrings[];

#ifdef __cplusplus
}
#endif