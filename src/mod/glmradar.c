#include "glmradar.h"
#include "globals.h"
#include "cpa_functions.h"
#include "dsgvarnames.h"
#include <math.h>
#include <ACP_Ray2.h>
#include <LST.h>
#include <mod/debugger.h>
#include "ui/ui_bridge.h"

/*
 * Defines
 */

#define YLT_St_ry_saut1_cycled 19
#define YLT_SautReception 2
#define YLT_Attente 1
#define GLM_Radar_Action IPT_E_Entry_Action_TransePolochus
#define GLM_Radar_TeleportDetectionThreshold 5

/*
 * Globals
 */

HIE_tdstEngineObject* alw_rayman;
long alwaysRaymanObjectType = 1000;
HIE_tdstSuperObject* spawned_rayman;

/*
 * Functions
 */

HIE_tdstSuperObject* CreateObject(POS_tdstCompletePosition position, tdObjectType modelType)
{
	//DR_Debugger();

  HIE_tdstSuperObject* h_stSuperObject = NULL;
	ALW_tdstAlwaysModelList* h_AlwaysModelChild;
	unsigned long i;
	LST_M_DynamicForEachIndex(&(ALW_g_stAlways->hLstAlwaysModel), h_AlwaysModelChild, i)
	{
		if (h_AlwaysModelChild->lObjectModelType == modelType)
		{
			printf("Always list index is %i, engine object is %x\n", i, h_AlwaysModelChild->p_stAlwaysObject);
			h_stSuperObject = ALW_g_stAlways->p_stAlwaysSuperObject;

      // Find a new slot for the new always object
			for (i = 0; (i < ALW_g_stAlways->ulMaxNbOfAlways) && (ALW_g_stAlways->a_stAlwaysGenerator[i] != NULL || LST_M_DynamicGetNextBrother(h_stSuperObject) != NULL); i++, h_stSuperObject++);

			if (i >= ALW_g_stAlways->ulMaxNbOfAlways)
			{
				return NULL;
			}

			HIE_tdstEngineObject* srcEngineObject = h_AlwaysModelChild->p_stAlwaysObject;
			HIE_tdstEngineObject* dstEngineObject = h_stSuperObject->hLinkedObject.p_stActor;

			printf("Superobject slot is %i, address is%x, dst engine object is %x\n\n", i, h_stSuperObject, dstEngineObject);
			
			if (dstEngineObject != NULL) {
				AI_tdstMind* dstBrain = dstEngineObject->hBrain;

				printf("dstBrain is %x\n", dstBrain);

				if (dstBrain != NULL) {
					AI_tdstMind* dstMind = dstEngineObject->hBrain->p_stMind;

					printf("dstMind is %x\n", dstMind);
				}
			}
			else {
				printf("dstEngineObject is NULL!\n");
			}
		}
	}

	//DR_Debugger();
	
	return ALW_fn_p_stAllocateAlways(
		modelType,
		*GAM_g_p_stDynamicWorld,
		HIE_M_hGetMainActor(),
		0,
		&position);
}

void CreateAlwaysRaymanObject() {

	if (alw_rayman != NULL) return;
	if (g_DR_rayman == NULL) return;

	alw_rayman = fn_p_stAllocateAlwaysEngineObject(
		g_DR_rayman->hLinkedObject.p_stActor->hStandardGame->lObjectFamilyType,
		alwaysRaymanObjectType,
		C_AlwaysObjectType);

	AI_fn_vBrainCopyClone(alw_rayman, g_DR_rayman->hLinkedObject.p_stActor);
	DNM_fn_vDynamCopyClone(alw_rayman, g_DR_rayman->hLinkedObject.p_stActor);

	alw_rayman->h3dData->h_CurrentObjectsTable =
		alw_rayman->h3dData->h_InitialObjectsTable = alw_rayman->h3dData->h_Family->hDefaultObjectsTable;
	alw_rayman->h3dData->h_InitialState = alw_rayman->h3dData->h_Family->stForStateArray.hFirstElementSta;
	alw_rayman->h3dData->ulNumberOfChannels = alw_rayman->h3dData->h_Family->ulNumberOfChannels;

	fn_vAddAnAlwaysModel(alw_rayman);
}

void RotationDuringGLMTest() {

	float rotation = 0.0f;

	if (IPT_M_bActionIsValidated(IPT_E_Entry_Action_Clavier_Droite))
	{
		rotation -= 1.0f;
	}
	if (IPT_M_bActionIsValidated(IPT_E_Entry_Action_Clavier_Gauche))
	{
		rotation += 1.0f;
	}
	
  float xAnalogicValue = IPT_g_stInputStructure->d_stEntryElementArray[0].xAnalogicValue;
	if (fabs(xAnalogicValue) > 10.0f)
	{
		rotation -= (xAnalogicValue / 40.0f);
	}

	if (fabs(rotation) > 0.1f) {

		AI_tdstNodeInterpret nodes[3] = {
			[0] = {
				.eType = AI_E_ti_Procedure,
				.ucDepth = 0,
				.uParam = AI_E_proc_RotatePersoAroundZ,
			},
			[1] = {
				.eType = AI_E_ti_Real,
				.ucDepth = 1,
				.uParam.xValue = rotation,
			},
			[2] = {
				.eType = AI_E_ti_EndTree,
			}
		};

		AI_tdstGetSetParam getSetParam;
		AI_fn_p_stEvalTree(g_DR_rayman, &nodes, &getSetParam);
	}

}

void RunOneGlmTest(GlmRadarData* data, int numChecks, HIE_tdstSuperObject* dummySPO, HIE_tdstEngineObject* dummy, MTH3D_tdstVector startGLM)
{
	POS_tdstCompletePosition newPos = *dummySPO->p_stGlobalMatrix;
	newPos.stPos.z += 0.02f;

	memcpy(dummySPO->p_stGlobalMatrix, &newPos, sizeof(POS_tdstCompletePosition));
	memcpy(dummySPO->p_stLocalMatrix, &newPos, sizeof(POS_tdstCompletePosition));

	memcpy(dummy->hBrain->p_stMind->p_stDsgMem->p_cDsgMemBuffer, g_DR_rayman->hLinkedObject.p_stActor->hBrain->p_stMind->p_stDsgMem->p_cDsgMemBuffer, dummy->hBrain->p_stMind->p_stAIModel->p_stDsgVar->ulBufferSize);

	// Set initial GLM vec
	*(MTH_tdstVector*)ACT_DsgVarPtr(dummy, DV_RAY_INTERN_TmpVector2) = startGLM;
	data->g_DR_glmDirectionFrom = startGLM;
	data->g_DR_glmDirectionTo = startGLM;

	data->g_DR_glmTeleport = (MTH3D_tdstVector){ 0 };

	memset(data->g_DR_glmCoordinateList, 0, sizeof(data->g_DR_glmCoordinateList));

	for (int i = 0;i < numChecks;i++) {
		PLA_fn_bSetNewState(dummySPO, ACT_GetStateByIndex(dummy, YLT_St_ry_saut1_cycled), TRUE, FALSE);
		ACT_ChangeComportRule(dummy, YLT_SautReception);

		memcpy(&dummy->hDynam->p_stDynamics->stDynamicsBase.stPreviousMatrix, &newPos, sizeof(POS_tdstCompletePosition));
		memcpy(&dummy->hDynam->p_stDynamics->stDynamicsBase.stCurrentMatrix, &newPos, sizeof(POS_tdstCompletePosition));
		memcpy(dummySPO->p_stGlobalMatrix, &newPos, sizeof(POS_tdstCompletePosition));
		memcpy(dummySPO->p_stLocalMatrix, &newPos, sizeof(POS_tdstCompletePosition));

		GAM_fn_vMakeCharacterMechanicallyReact(dummySPO);
		GAM_fn_vMakeCharacterReact(dummySPO);

		MTH3D_tdstVector posAfterAttempt = dummy->hDynam->p_stDynamics->stDynamicsBase.stPreviousMatrix.stPos;

		float xDiff = posAfterAttempt.x - newPos.stPos.x;
		float yDiff = posAfterAttempt.y - newPos.stPos.y;
		float zDiff = posAfterAttempt.z - newPos.stPos.z;
		float distanceMovedSquared = xDiff * xDiff + yDiff * yDiff + zDiff * zDiff;
		float distanceMoved = sqrtf(distanceMovedSquared);

		MTH_tdstVector newGlmPos = *(MTH_tdstVector*)ACT_DsgVarPtr(dummy, DV_RAY_INTERN_TmpVector2);
		data->g_DR_glmCoordinateList[i] = newGlmPos;

		if (distanceMoved > GLM_Radar_TeleportDetectionThreshold) {

			// Teleport detected, run one more frame to update the position
			GAM_fn_vMakeCharacterMechanicallyReact(dummySPO);
			GAM_fn_vMakeCharacterReact(dummySPO);

			posAfterAttempt = dummy->hDynam->p_stDynamics->stDynamicsBase.stPreviousMatrix.stPos;
			data->g_DR_glmTeleport = posAfterAttempt;
			break;
		}
		else {
			data->g_DR_glmDirectionTo = newGlmPos;
		}
	}
}

void TestGLM() {

	HIE_tdstSuperObject* dummySPO = CreateObject(*g_DR_rayman->p_stGlobalMatrix, alwaysRaymanObjectType);

	if (dummySPO == NULL) {
		return;
	}

	HIE_tdstEngineObject* dummy = dummySPO->hLinkedObject.p_stActor;
	dummy->hStandardGame->lObjectModelType = g_DR_rayman->hLinkedObject.p_stActor->hStandardGame->lObjectModelType;

	AI_fn_vBrainCopyClone(dummy, g_DR_rayman->hLinkedObject.p_stActor);
	COL_fn_vCollSetCopyClone(dummy, g_DR_rayman->hLinkedObject.p_stActor);
	DNM_fn_vDynamCopyClone(dummy, g_DR_rayman->hLinkedObject.p_stActor);

	memset(&g_DR_glmData, 0, sizeof(g_DR_glmData));
	for (int bookmarkIndex = 0; bookmarkIndex < g_DR_glmBookmarkCount; bookmarkIndex++) {
		GlmRadarData* data = &g_DR_glmData[bookmarkIndex];
		int numChecks = GLMRadar_NumChecks / g_DR_glmBookmarkCount;

		RunOneGlmTest(data, numChecks < 10 ? 10 : numChecks, dummySPO, dummy, g_DR_glmBookmarks[bookmarkIndex]);
	}

	fn_vKillEngineObjectOrAlwaysByPointer(dummy);
}

/*
 * Hooked functions
 */

void GLMRadar_Before_fn_vEngine() {

	if (!DR_Settings_Get_Util_EnableGLMRadar()) {
		return;
	}

	CreateAlwaysRaymanObject();

	char* raymanReacts = (char*)ACT_DsgVarPtr(g_DR_rayman->hLinkedObject.p_stActor, DV_RAY_RAY_ReagitAuxCommandes);
	if (IPT_M_bActionJustValidated(GLM_Radar_Action))
	{
		g_DR_rayman->hLinkedObject.p_stActor->hStandardGame->ulCustomBits |= Std_C_CustBit_NoMechanic;
		*raymanReacts = FALSE;
	}

	if (IPT_M_bActionIsValidated(GLM_Radar_Action))
	{
		RotationDuringGLMTest();
		TestGLM();
	}

	if (IPT_M_bActionJustInvalidated(GLM_Radar_Action))
	{
		g_DR_rayman->hLinkedObject.p_stActor->hStandardGame->ulCustomBits &= ~Std_C_CustBit_NoMechanic;
		*raymanReacts = TRUE;
	}

}


void GLMRadar_Init() {

	CreateAlwaysRaymanObject();
}

void GLMRadar_DeInit()
{
	if (alw_rayman != NULL) {
		alw_rayman->hBrain = NULL;
		fn_vRemoveAnAlwaysModel(alw_rayman);
		alw_rayman = NULL;
	}
}