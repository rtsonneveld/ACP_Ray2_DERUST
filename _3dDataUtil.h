#pragma once
#include <ACP_Ray2.h>

typedef struct GAM_tdst3dData* MS_tdxHandleTo3dData;
typedef struct HIE_tdstFamilyList* tdxHandleToFamilyList;
typedef struct HIE_tdstState* tdxHandleToState;
typedef struct HIE_tdstSuperObject* HIE_tdxHandleToSuperObject;
typedef struct POS_stCompletePosition* POS_tdxHandleToPosition;
typedef struct GMT_tdstGameMaterial* GMT_tdxHandleToGameMaterial;

#define M_GetMSHandle(p_stSuperObject,Name) ((MS_tdxHandleTo##Name) ((struct HIE_tdstEngineObject *)(p_stSuperObject->hLinkedObject.p_Void))->h##Name)

tdxHandleToState fn_h3dDataGetInitialState(MS_tdxHandleTo3dData h_3dData)
{
	return(h_3dData->h_InitialState);
}

void fn_v3dDataSetInitialState(MS_tdxHandleTo3dData h_3dData, tdxHandleToState h_InitialState)
{
	h_3dData->h_InitialState = h_InitialState;
}


tdxHandleToState fn_h3dDataGetCurrentState(MS_tdxHandleTo3dData h_3dData)
{
	return(h_3dData->h_CurrentState);
}

void fn_v3dDataSetCurrentState(MS_tdxHandleTo3dData h_3dData, tdxHandleToState h_CurrentState)
{
	h_3dData->h_CurrentState = h_CurrentState;
}


tdxHandleToState fn_h3dDataGetFirstStateOfAction(MS_tdxHandleTo3dData h_3dData)
{
	return(h_3dData->h_FirstStateOfAction);
}

void fn_v3dDataSetFirstStateOfAction(MS_tdxHandleTo3dData h_3dData, tdxHandleToState h_FirstStateOfAction)
{
	h_3dData->h_FirstStateOfAction = h_FirstStateOfAction;
}


HIE_tdstObjectsTablesList* fn_h3dDataGetCurrentObjectsTable(MS_tdxHandleTo3dData h_3dData)
{
	return(h_3dData->h_CurrentObjectsTable);
}

void fn_v3dDataSetCurrentObjectsTable(MS_tdxHandleTo3dData h_3dData, HIE_tdstObjectsTablesList* h_CurrentObjectsTable)
{
	h_3dData->h_CurrentObjectsTable = h_CurrentObjectsTable;
}


HIE_tdstObjectsTablesList* fn_h3dDataGetInitialObjectsTable(MS_tdxHandleTo3dData h_3dData)
{
	return(h_3dData->h_InitialObjectsTable);
}

void fn_v3dDataSetInitialObjectsTable(MS_tdxHandleTo3dData h_3dData, HIE_tdstObjectsTablesList* h_InitialObjectsTable)
{
	h_3dData->h_InitialObjectsTable = h_InitialObjectsTable;
}


tdxHandleToFamilyList fn_h3dDataGetFamily(MS_tdxHandleTo3dData h_3dData)
{
	return(h_3dData->h_Family);
}

void fn_v3dDataSetFamily(MS_tdxHandleTo3dData h_3dData, tdxHandleToFamilyList h_Family)
{
	h_3dData->h_Family = h_Family;
}


POS_tdstCompletePosition* fn_p_st3dDataGetAbsoluteMatrix(MS_tdxHandleTo3dData _h_3dData)
{
	return((_h_3dData->p_stGLIObjectAbsoluteMatrix));
}

void fn_v3dDataSetAbsoluteMatrix(MS_tdxHandleTo3dData _h_3dData, POS_tdstCompletePosition* _p_stMatrix)
{
	_h_3dData->p_stGLIObjectAbsoluteMatrix = _p_stMatrix;
}

/*
void fn_v3dDataCopyAbsoluteMatrix(MS_tdxHandleTo3dData _h_3dData, POS_tdstCompletePosition* _p_stMatrix)
{
	POS_fn_vCopyMatrix(_h_3dData->p_stGLIObjectAbsoluteMatrix, _p_stMatrix);
}
*/


POS_tdstCompletePosition* fn_p_st3dDataGetMatrix(MS_tdxHandleTo3dData h_3dData)
{
	return(&(h_3dData->stGLIObjectMatrix));
}

/*
void fn_v3dDataSetMatrix(MS_tdxHandleTo3dData h_3dData, POS_tdxHandleToPosition _hMatrix)
{
	POS_fn_vCopyMatrix(&(h_3dData->stGLIObjectMatrix), _hMatrix);
}
*/


unsigned short fn_uw3dDataGetCurrentFrame(MS_tdxHandleTo3dData h_3dData)
{
	return(h_3dData->uwCurrentFrame);
}

void fn_v3dDataSetCurrentFrame(MS_tdxHandleTo3dData h_3dData, unsigned short uwCurrentFrame)
{
	h_3dData->uwCurrentFrame = uwCurrentFrame;
}


GAM_tdstCouple* fn_p_st3dDataGetCurrentHieCouples(MS_tdxHandleTo3dData h_3dData)
{
	return(h_3dData->p_stCurrentHieCouples);
}

void fn_v3dDataSetCurrentHieCouples(MS_tdxHandleTo3dData h_3dData, GAM_tdstCouple* _p_stCpl)
{
	h_3dData->p_stCurrentHieCouples = _p_stCpl;
}

/* ANNECY MT - 22/09/98 {*/

unsigned long fn_ul3dDataGetCurrentHieNbCouples(MS_tdxHandleTo3dData h_3dData)
{
	return((unsigned long)h_3dData->wCurrentHieNbCouples);
}

void fn_v3dDataSetCurrentHieNbCouples(MS_tdxHandleTo3dData h_3dData, unsigned long _ulNbCpl)
{
	h_3dData->wCurrentHieNbCouples = (unsigned short)_ulNbCpl;
}


unsigned short fn_w3dDataGetSizeOfArrayOfElts3d(MS_tdxHandleTo3dData h_3dData)
{
	return(h_3dData->wSizeOfArrayOfElts3d);
}

void fn_v3dDataSetSizeOfArrayOfElts3d(MS_tdxHandleTo3dData h_3dData, unsigned short _wSize)
{
	h_3dData->wSizeOfArrayOfElts3d = _wSize;
}
/* END ANNECY MT }*/


unsigned char fn_uc3dDataGetRepeatAnimation(MS_tdxHandleTo3dData h_3dData)
{
	return(h_3dData->ucRepeatAnimation);
}

void fn_v3dDataSetRepeatAnimation(MS_tdxHandleTo3dData h_3dData, unsigned char ucRepeatAnimation)
{
	h_3dData->ucRepeatAnimation = ucRepeatAnimation;
}


void fn_v3dDataDecRepeatAnimation(MS_tdxHandleTo3dData h_3dData)
{
	(h_3dData->ucRepeatAnimation)--;
}

unsigned char fn_uc3dDataGetNextEvent(MS_tdxHandleTo3dData h_3dData)
{
	return(h_3dData->ucNextEvent);
}


void fn_v3dDataSetNextEvent(MS_tdxHandleTo3dData h_3dData, unsigned char ucNextEvent)
{
	h_3dData->ucNextEvent = ucNextEvent;
}

void fn_v3dDataAddNextEvent(MS_tdxHandleTo3dData h_3dData, char _cNextEvent)
{
	h_3dData->ucNextEvent = (unsigned char)((long)(h_3dData->ucNextEvent) + (long)_cNextEvent);
}

/*
void fn_v3dDataAllocateEventActivation(MS_tdxHandleTo3dData h_3dData, unsigned long ulNumber)
{
	h_3dData->d_ucEventActivation = (unsigned char*)TMP_M_p_Malloc(sizeof(unsigned char) * ulNumber);
}
*/


/*
void fn_v3dDataFreeEventActivation(MS_tdxHandleTo3dData h_3dData)
{
	if (h_3dData->d_ucEventActivation)
		TMP_M_Free(h_3dData->d_ucEventActivation);
}
*/

void fn_v3dDataSetEventActivationByIndex(MS_tdxHandleTo3dData h_3dData, unsigned long ulIndex, unsigned char ucValue)
{
	h_3dData->d_ucEventActivation[ulIndex] = ucValue;
}


unsigned char fn_uc3dDataGetEventActivationByIndex(MS_tdxHandleTo3dData h_3dData, unsigned long ulIndex)
{
	return(h_3dData->d_ucEventActivation[ulIndex]);
}

void fn_v3dDataAddEventActivationByIndex(MS_tdxHandleTo3dData h_3dData, unsigned long ulIndex, char _cValue)
{
	h_3dData->d_ucEventActivation[ulIndex] = (unsigned char)((long)(h_3dData->d_ucEventActivation[ulIndex]) + (long)_cValue);
}


struct GAM_tdstFrame3d* fn_p_st3dDataGetCurrentFrame(MS_tdxHandleTo3dData h_3dData)
{
	return(&h_3dData->stFrame3d);
}

tdxHandleToState fn_h3dDataGetWantedState(MS_tdxHandleTo3dData h_3dData)
{
	return(h_3dData->h_WantedState);
}

void fn_v3dDataSetWantedState(MS_tdxHandleTo3dData h_3dData, tdxHandleToState h_WantedState)
{
	h_3dData->h_WantedState = h_WantedState;
}

void fn_v3dDataSetStateInLastFrame(MS_tdxHandleTo3dData h_3dData, tdxHandleToState h_StateInLastFrame)
{
	h_3dData->h_StateInLastFrame = h_StateInLastFrame;
}


unsigned char fn_uc3dDataGetFlagModifState(MS_tdxHandleTo3dData h_3dData)
{
	return(h_3dData->ucFlagModifState);
}

void fn_v3dDataSetFlagModifState(MS_tdxHandleTo3dData h_3dData, unsigned char ucFlagModifState)
{
	h_3dData->ucFlagModifState = ucFlagModifState;
}



void fn_v3dDataSetAnimMatrixHasChanged(MS_tdxHandleTo3dData h_3dData)
{
	h_3dData->bAnimMatrixChanged = TRUE;
}

void fn_v3dDataResetAnimMatrixHasChanged(MS_tdxHandleTo3dData h_3dData)
{
	h_3dData->bAnimMatrixChanged = FALSE;
}


BOOL fn_b3dDataHasAnimMatrixChanged(MS_tdxHandleTo3dData h_3dData)
{
	return h_3dData->bAnimMatrixChanged;
}



unsigned char fn_uc3dDataGetFlagEndOfAnim(MS_tdxHandleTo3dData h_3dData)
{
	return(h_3dData->ucFlagEndOfAnim);
}

void fn_v3dDataSetFlagEndOfAnim(MS_tdxHandleTo3dData h_3dData, unsigned char ucFlagEndOfAnim)
{
	h_3dData->ucFlagEndOfAnim = ucFlagEndOfAnim;
}
/**************************  / tmp debut GIZMO*/

unsigned short fn_uw3dDataGetForcedFrame(MS_tdxHandleTo3dData h_3dData)
{
	return(h_3dData->uwForcedFrame);
}

void fn_v3dDataSetForcedFrame(MS_tdxHandleTo3dData h_3dData, unsigned short uwForcedFrame)
{
	h_3dData->uwForcedFrame = uwForcedFrame;
}
/* tmp fin GIZMO*/


unsigned char fn_uc3dDataGetFlagEndState(MS_tdxHandleTo3dData h_3dData)
{
	return(h_3dData->ucFlagEndState);
}

void fn_v3dDataSetFlagEndState(MS_tdxHandleTo3dData h_3dData, unsigned char ucFlagEndState)
{
	h_3dData->ucFlagEndState = ucFlagEndState;
}

/* MR2503*/

void* fn_h3dDataGetChannelSOList(MS_tdxHandleTo3dData h_3dData)
{
	return (h_3dData->hArrayOfChannels);
}

/* CHN_tdxHandleToCASList fn_h3dDataGetChannelActivationList(MS_tdxHandleTo3dData h_3dData)
{
	return (h_3dData->hChannelActivationList);
}
*/

void* fn_h3dDataGetMorphList(MS_tdxHandleTo3dData h_3dData)
{
	return (h_3dData->hMorphChannelList);
}


unsigned long fn_ul3dDataGetNumberOfChannels(MS_tdxHandleTo3dData h_3dData)
{
	return (h_3dData->ulNumberOfChannels);
}


void fn_v3dDataSetNumberOfChannels(MS_tdxHandleTo3dData h_3dData, unsigned long ulNum)
{
	h_3dData->ulNumberOfChannels = ulNum;
}


void fn_v3dDataSetChannelSOList(MS_tdxHandleTo3dData h_3dData, void* hArray)
{
	h_3dData->hArrayOfChannels = hArray;
}



void fn_v3dDataSetFirstActiveChannel(MS_tdxHandleTo3dData h_3dData, void* hFirstActiveChannel)
{
	h_3dData->hFirstActiveChannel = hFirstActiveChannel;
}


void* fn_h3dDataGetFirstActiveChannel(MS_tdxHandleTo3dData h_3dData)
{
	return h_3dData->hFirstActiveChannel;
}


unsigned long fn_ul3dDataGetStartTime(MS_tdxHandleTo3dData h_3dData)
{
	return (h_3dData->ulStartTime);
}


void fn_v3dDataSetStartTime(MS_tdxHandleTo3dData h_3dData, unsigned long ulValue)
{
	h_3dData->ulStartTime = ulValue;
}


BOOL fn_b3dDataGetSkipCurrentFrame(MS_tdxHandleTo3dData h_3dData)
{
	return (h_3dData->bSkipCurrentFrame);
}


void fn_v3dDataSetSkipCurrentFrame(MS_tdxHandleTo3dData h_3dData, BOOL bValue)
{
	h_3dData->bSkipCurrentFrame = (unsigned char)bValue;
}


unsigned long fn_ul3dDataGetTimeDelay(MS_tdxHandleTo3dData h_3dData)
{
	return (h_3dData->ulTimeDelay);
}


void fn_v3dDataSetTimeDelay(MS_tdxHandleTo3dData h_3dData, unsigned long ulValue)
{
	h_3dData->ulTimeDelay = ulValue;
}


unsigned long fn_ul3dDataGetTimePreviousFrame(MS_tdxHandleTo3dData h_3dData)
{
	return (h_3dData->ulTimePreviousFrame);
}


void fn_v3dDataSetTimePreviousFrame(MS_tdxHandleTo3dData h_3dData, unsigned long ulValue)
{
	h_3dData->ulTimePreviousFrame = ulValue;
}


short fn_s3dDataGetLastFrame(MS_tdxHandleTo3dData h_3dData)
{
	return (h_3dData->sLastFrame);
}


void fn_v3dDataSetLastFrame(MS_tdxHandleTo3dData h_3dData, short sValue)
{
	h_3dData->sLastFrame = sValue;
}


BOOL fn_b3dDataGetStateJustModified(MS_tdxHandleTo3dData h_3dData)
{
	return (h_3dData->bStateJustModified);
}


void fn_v3dDataSetStateJustModified(MS_tdxHandleTo3dData h_3dData, BOOL bValue)
{
	h_3dData->bStateJustModified = (unsigned char)bValue;
}


unsigned short fn_uw3dDataGetNbEngineFrameSinceLastMechEvent(MS_tdxHandleTo3dData h_3dData)
{
	return h_3dData->uwNbEngineFrameSinceLastMechEvent;
}


void fn_v3dDataSetNbEngineFrameSinceLastMechEvent(MS_tdxHandleTo3dData h_3dData, unsigned short uwValue)
{
	h_3dData->uwNbEngineFrameSinceLastMechEvent = uwValue;
}


unsigned char fn_uc3dDataGetFrameRate(MS_tdxHandleTo3dData h_3dData)
{
	return h_3dData->ucFrameRate;
}

MTH_tdxReal fn_x3dDataGetShadowScaleX(MS_tdxHandleTo3dData _h3dData)
{
	return _h3dData->xShadowScaleX;
}

void fn_v3dDataSetShadowScaleX(MS_tdxHandleTo3dData _h3dData, MTH_tdxReal _xScaleX)
{
	_h3dData->xShadowScaleX = _xScaleX;
}


MTH_tdxReal fn_x3dDataGetShadowScaleY(MS_tdxHandleTo3dData _h3dData)
{
	return _h3dData->xShadowScaleY;
}

void fn_v3dDataSetShadowScaleY(MS_tdxHandleTo3dData _h3dData, MTH_tdxReal _xScaleY)
{
	_h3dData->xShadowScaleY = _xScaleY;
}

/*XB unused field in U64 engine*/
#ifndef U64

short fn_x3dDataGetShadowQuality(MS_tdxHandleTo3dData _h3dData)
{
	return _h3dData->xShadowQuality;
}

void fn_v3dDataSetShadowQuality(MS_tdxHandleTo3dData _h3dData, short _xQuality)
{
	_h3dData->xShadowQuality = _xQuality;
}
#endif /* U64 */
/*End XB*/


/*XB unused field in U64 engine*/
#ifndef U64

void fn_v3dDataSetShadowTexture(MS_tdxHandleTo3dData _h3dData, GLI_tdstTexture* _p_stShadowTexture)
{
	_h3dData->p_stShadowTexture = _p_stShadowTexture;
}


void fn_v3dDataSetShadowMaterial(MS_tdxHandleTo3dData _h3dData, GMT_tdxHandleToGameMaterial _p_stShadowMaterial)
{
	_h3dData->p_stShadowMaterial = _p_stShadowMaterial;
}
#endif /* U64 */
/*End XB*/


MTH_tdxReal fn_x3dDataGetShadowScaleXFromSO(HIE_tdxHandleToSuperObject _hSO)
{
	return M_GetMSHandle(_hSO, 3dData)->xShadowScaleX;
}

void fn_v3dDataSetShadowScaleXInSO(HIE_tdxHandleToSuperObject _hSO, MTH_tdxReal _xScaleX)
{
	M_GetMSHandle(_hSO, 3dData)->xShadowScaleX = _xScaleX;
}


MTH_tdxReal fn_x3dDataGetShadowScaleYFromSO(HIE_tdxHandleToSuperObject _hSO)
{
	return M_GetMSHandle(_hSO, 3dData)->xShadowScaleY;
}

void fn_v3dDataSetShadowScaleYInSO(HIE_tdxHandleToSuperObject _hSO, MTH_tdxReal _xScaleY)
{
	M_GetMSHandle(_hSO, 3dData)->xShadowScaleY = _xScaleY;
}

/*XB unused field in U64 engine*/
#ifndef U64

short fn_x3dDataGetShadowQualityFromSO(HIE_tdxHandleToSuperObject _hSO)
{
	return M_GetMSHandle(_hSO, 3dData)->xShadowQuality;
}


GMT_tdxHandleToGameMaterial fn_x3dDataGetShadowMaterialFromSO(HIE_tdxHandleToSuperObject _hSO)
{
	return M_GetMSHandle(_hSO, 3dData)->p_stShadowMaterial;
}
#endif /* U64 */
/*End XB*/


#if !defined(U64)	/* MT {*/

MTH3D_tdstVector* fn_p_st3dDataGetSHWDeformationVector(MS_tdxHandleTo3dData _h3dData)
{
	return &(_h3dData->stSHWDeformationVector);
}


MTH_tdxReal* fn_p_x3dDataGetSHWHeight(MS_tdxHandleTo3dData _h3dData)
{
	return &(_h3dData->xSHWHeight);
}


MTH3D_tdstVector* fn_p_st3dDataGetSHWDeformationVectorFromSO(HIE_tdxHandleToSuperObject _hSO)
{
	return &(M_GetMSHandle(_hSO, 3dData)->stSHWDeformationVector);
}


MTH_tdxReal* fn_p_x3dDataGetSHWHeightFromSO(HIE_tdxHandleToSuperObject _hSO)
{
	return &(M_GetMSHandle(_hSO, 3dData)->xSHWHeight);
}
#endif /* U64 - MT }*/

void fn_v3dDataSetDrawMask(MS_tdxHandleTo3dData _h3dData, long _lDrawMask)
{
	_h3dData->lDrawMask = _lDrawMask;
}


long fn_l3dDataGetDrawMask(MS_tdxHandleTo3dData _h3dData)
{
	return _h3dData->lDrawMask;
}



unsigned char fn_uc3dDataGetBrainComputationFrequency(MS_tdxHandleTo3dData h_3dData)
{
	return(unsigned char)(h_3dData ? h_3dData->ucBrainComputationFrequency : 1);
}


unsigned char fn_uc3dDataGetBrainComputationFrequencySO(HIE_tdxHandleToSuperObject _hCharacter)
{
	return(unsigned char)(_hCharacter ? fn_uc3dDataGetBrainComputationFrequency(M_GetMSHandle(_hCharacter, 3dData)) : 1);
}


#if !defined(U64) /*MT*/

unsigned char fn_uc3dDataGetLightComputationFrequency(MS_tdxHandleTo3dData h_3dData)
{
	return(unsigned char)(h_3dData ? h_3dData->ucLightComputationFrequency : 1);
}


unsigned char fn_uc3dDataGetLightComputationFrequencySO(HIE_tdxHandleToSuperObject _hCharacter)
{
	return(unsigned char)(_hCharacter ? fn_uc3dDataGetLightComputationFrequency(M_GetMSHandle(_hCharacter, 3dData)) : 1);
}
#endif /*U64*/

#ifdef D_USE_LIPSYNC

void fn_v3dDataSetLipsTable(MS_tdxHandleTo3dData h_3dData, tdxHandleOfLipsSynchroTable hLipsTable)
{
	h_3dData->hLipsTable = hLipsTable;
}


tdxHandleOfLipsSynchroTable fn_x3dDataGetLipsTable(MS_tdxHandleTo3dData h_3dData)
{
	return h_3dData->hLipsTable;
}


void fn_v3dDataSetStartSynchroFrame(MS_tdxHandleTo3dData h_3dData, unsigned short uwStartSynchroframe)
{
	h_3dData->uwStartSynchroFrame = uwStartSynchroframe;
}


unsigned short fn_uw3dDataGetStartSynchroFrame(MS_tdxHandleTo3dData h_3dData)
{
	return h_3dData->uwStartSynchroFrame;
}


void fn_v3dDataSetCurrentSynchroFrame(MS_tdxHandleTo3dData h_3dData, unsigned short uwCurrentSynchroframe)
{
	h_3dData->uwCurrentSynchroFrame = uwCurrentSynchroframe;
}


unsigned short fn_uw3dDataGetCurrentSynchroFrame(MS_tdxHandleTo3dData h_3dData)
{
	return h_3dData->uwCurrentSynchroFrame;
}
#endif /* D_USE_LIPSYNC */

/* MR1007*/
/* BOOL fn_b3dDataIsCurrentFrameInterpolated(MS_tdxHandleTo3dData h_3dData)
{
	return h_3dData->bCurrentFrameIsInterpolated;
}


void fn_v3dDataSetCurrentFrameInterpolated(MS_tdxHandleTo3dData h_3dData,BOOL bInter)
{
	h_3dData->bCurrentFrameIsInterpolated=bInter;
}
*/
/* GARBAGE: TO REMOVE */

void fn_vInitBubbleEffect()
{
}


BOOL fn_l3dDataGetLastComputeEngineFrame(MS_tdxHandleTo3dData h_3dData)
{
	return h_3dData->lLastComputeFrame;
}


void fn_v3dDataSetLastComputeEngineFrame(MS_tdxHandleTo3dData h_3dData, long lLastComputeFrame)
{
	h_3dData->lLastComputeFrame = lLastComputeFrame;
}


unsigned char fn_uc3dDataGetUserEventFlags(MS_tdxHandleTo3dData h_3dData)
{
	return h_3dData->ucUserEventFlags;
}


void fn_v3dDataSetUserEventFlags(MS_tdxHandleTo3dData h_3dData, unsigned char _ucFlags)
{
	h_3dData->ucUserEventFlags = _ucFlags;
}



void fn_v3dDataUpdateMainBrainCounter(MS_tdxHandleTo3dData h_3dData)
{
	if (h_3dData) h_3dData->uwBrainMainCounter++;
}


unsigned short fn_uw3dDataGetMainBrainCounter(MS_tdxHandleTo3dData h_3dData)
{
	return(unsigned short)(h_3dData ? h_3dData->uwBrainMainCounter : 0);
}

