#include "feature_triggerdisplay.h"

#include "MTH.h"
#include "GLI/GLI.h"
#include <math.h>

GLI_tdstMaterial* stCircleYellow;
GLI_tdstMaterial* stCircleRed;
GLI_tdstMaterial* stCircleBlue;

#define GLI_C_lIsTextured							1
#define C_TooFarLimitMargin 5.0f
#define C_TooFarLimitMargin 5.0f
#define C_TooFarLimitParticleScale 0.5f
#define C_lGouraudLineElement ((GLI_C_Mat_lIsSolid | GLI_C_Mat_lIsGouraud)- GLI_C_Mat_lIsNotWired)

DR_FEATURE_FUNC_OnActivate(TriggerDisplay, feature) {

	stCircleYellow = (GLI_tdstMaterial*)malloc(sizeof(GLI_tdstMaterial));
	GLI_fn_vInitMaterialDefaults(stCircleYellow);
	stCircleYellow->ulMaterialType = GLI_C_Mat_lIsTextured;

	stCircleRed = (GLI_tdstMaterial*)malloc(sizeof(GLI_tdstMaterial));
	GLI_fn_vInitMaterialDefaults(stCircleRed);
	stCircleRed->ulMaterialType = GLI_C_Mat_lIsTextured;

	stCircleBlue = (GLI_tdstMaterial*)malloc(sizeof(GLI_tdstMaterial));
	GLI_fn_vInitMaterialDefaults(stCircleBlue);
	stCircleBlue->ulMaterialType = GLI_C_Mat_lIsTextured;

	stCircleYellow->p_stTexture = GLI_fn_pstFindTexture("effets_speciaux\\faisc_centre_txy_ad.tga");;
	stCircleRed->p_stTexture = GLI_fn_pstFindTexture("effets_speciaux\\faisc_centre_mort_adnzw.tga");
	stCircleBlue->p_stTexture = GLI_fn_pstFindTexture("effets_speciaux\\faisc_centre_mort_bleu_adnzw.tga");
}

DR_FEATURE_FUNC_OnDeactivate(TriggerDisplay, feature) {

}

#define GLI_M_SWAP(a,b)\
    {\
        fSwapValue = (a);\
        (a) = (b);\
        (b) = fSwapValue ;\
    }\

/* ##M==================================================================================
NAME :          MTH3D_M_vGetVectorsInMatrix
DESCRIPTION :   Get all column Matrix to two Vectors
INPUT :         VaDest, VbDest, VcDest: address of MTH3D_tdstVector
				MatA : address of MTH3D_tdstMatrix
OUTPUT :        void
=======================================================================================*/
#define MTH3D_M_vGetVectorsInMatrix( VaDest, VbDest, VcDest, MatA)                      \
   {          (VaDest)->x=(MatA)->stCol_0.x;                                          \
              (VaDest)->y=(MatA)->stCol_0.y;                                          \
              (VaDest)->z=(MatA)->stCol_0.z;                                          \
              (VbDest)->x=(MatA)->stCol_1.x;                                          \
              (VbDest)->y=(MatA)->stCol_1.y;                                          \
              (VbDest)->z=(MatA)->stCol_1.z;                                          \
              (VcDest)->x=(MatA)->stCol_2.x;                                          \
              (VcDest)->y=(MatA)->stCol_2.y;                                          \
              (VcDest)->z=(MatA)->stCol_2.z; }

#define MTH3D_M_vCopyVector( VectDest, VectA)                                           \
	{	((unsigned long *) (VectDest))[0]=	((unsigned long *) (VectA))[0];				\
		((unsigned long *) (VectDest))[1]=	((unsigned long *) (VectA))[1];				\
		((unsigned long *) (VectDest))[2]=	((unsigned long *) (VectA))[2];				\
	}

void POS_fn_vGetTransformMatrix(POS_tdstCompletePosition* _hMatrix,
	MTH3D_tdstVector* _p_stI,
	MTH3D_tdstVector* _p_stJ,
	MTH3D_tdstVector* _p_stK)
{
	MTH3D_M_vGetVectorsInMatrix(_p_stI, _p_stJ, _p_stK, &(_hMatrix->stTransformMatrix));
}

/**************** GETTING THE TRANSLATION VERTEX OF A MATRIX *************/
void POS_fn_vGetTranslationVector(
	POS_tdstCompletePosition* _hMatrix,
	MTH3D_tdstVector* _p_stTrs
)
{
	MTH3D_M_vCopyVector(_p_stTrs, &(_hMatrix->stTranslationVector));
}


void GLI_xLoadMatrix(POS_tdstCompletePosition* p_stMatrix)
{
  *g_aDEF_stMatrixStack[*g_lNbMatrixInStack] = *p_stMatrix;

  *g_p_stCurrentMatrix = g_aDEF_stMatrixStack[*g_lNbMatrixInStack];

  *g_lNbMatrixInStack = *g_lNbMatrixInStack+1;

  return;
}

void GLI_xPopMatrix(void)
{
	if (g_lNbMatrixInStack == 0)
		return;

	*g_lNbMatrixInStack= *g_lNbMatrixInStack-1;

	*g_p_stCurrentMatrix = &g_aDEF_stMatrixStack[*g_lNbMatrixInStack];

	return;
}


void GLI_xSerialLinearOp(long lNbOfVertex, MTH3D_tdstVector* p_stSource, GLI_tdstAligned3DVector* p_stDest, POS_tdstCompletePosition* p_stMatrix)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MTH3D_tdstVector        stX, stY, stZ, stTrans;
	GLI_tdstAligned3DVector* p_stLastDest;
	float                   fSwapValue;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	POS_fn_vGetTransformMatrix(p_stMatrix, &stX, &stY, &stZ);
	GLI_M_SWAP(stX.y, stY.x);
	GLI_M_SWAP(stX.z, stZ.x);
	GLI_M_SWAP(stY.z, stZ.y);
	POS_fn_vGetTranslationVector(p_stMatrix, &stTrans);
	p_stLastDest = p_stDest + lNbOfVertex;
	for (; p_stDest < p_stLastDest; p_stDest++, p_stSource++)
	{
		p_stDest->xX = MTH3D_M_xDotProductVector(&stX, p_stSource) + stTrans.x;
		p_stDest->xY = MTH3D_M_xDotProductVector(&stY, p_stSource) + stTrans.y;
		p_stDest->xZ = MTH3D_M_xDotProductVector(&stZ, p_stSource) + stTrans.z;
	}
}

void GLI_DRV_vSetZClip(float ZClip, GLI_tdstInternalGlobalValuesFor3dEngine* p_stGlobaleMT)
{
	p_stGlobaleMT->fZClipping = ZClip;
	p_stGlobaleMT->fWClipping = 1.0f / ZClip;
}

/**********************************************************************************************/
/* Name: GLI_xDraw3DLine16 1.0*/
/* Goal: Draw 3d line unclipped and unprojected in 16b*/
/* Code: Philippe Vimont*/
/**********************************************************************************************/
/*void GLI_vSetZClip(float ZClip,GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT);*/
/*void GLI_NJA_vSendSingleLineToClip( GLD_tdstViewportAttributes *p_stVpt , GLI_tdstAligned3DVector *p_stVertex1 , GLI_tdstAligned2DVector *p_st2DVertex1 , GLI_tdstAligned3DVector *p_stVertex2 , GLI_tdstAligned2DVector *p_st2DVertex2 , GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT, long lDrawModeMask, GEO_tdstColor *p_stColor );*/

void GLI_xDraw3DLine16(GLD_tdstViewportAttributes* p_stVpt, MTH3D_tdstVector* p_stFirstPoint, MTH3D_tdstVector* p_stLastPoint, long lColor)
{
	GLI_tdstAligned2DVector				a2_st2DVertex[4] = {
		(GLI_tdstAligned2DVector) {0, 0},
		(GLI_tdstAligned2DVector) {0, 0},
		(GLI_tdstAligned2DVector) {0, 0},
		(GLI_tdstAligned2DVector) {0, 0},
	};

	GLI_tdstSpecificAttributesFor3D* p_st3DAtributes;

	GLI_tdstAligned3DVector				a2_stVertex[4] = {
		(GLI_tdstAligned3DVector){0,0,0,0},
		(GLI_tdstAligned3DVector){0,0,0,0},
		(GLI_tdstAligned3DVector){0,0,0,0},
		(GLI_tdstAligned3DVector){0,0,0,0},
	};

	GEO_tdstColor						stColor;

	p_st3DAtributes = (GLI_tdstSpecificAttributesFor3D*)(p_stVpt->p_vSpecificToXD);

	GLI_tdstInternalGlobalValuesFor3dEngine* globals = (*GLI_BIG_GLOBALS);

	GLI_DRV_vSetZClip((float)p_st3DAtributes->p_stCam->xNear, globals);

	/*turn points*/
	GLI_xSerialLinearOp(1, p_stFirstPoint, &a2_stVertex[0], *g_p_stCurrentMatrix);
	GLI_xSerialLinearOp(1, p_stLastPoint, &a2_stVertex[1], *g_p_stCurrentMatrix);

	/* project points       */
	GLI_xSerialProjection(p_st3DAtributes->p_stCam, 2, a2_stVertex, a2_st2DVertex);
	a2_st2DVertex[0].ulPackedColor = a2_st2DVertex[1].ulPackedColor = lColor;

	stColor.xR = (float)((lColor & 0x00FF0000) >> 16) * 1.0f;
	stColor.xG = (float)((lColor & 0x0000FF00) >> 8) * 1.0f;
	stColor.xB = (float)((lColor & 0x000000FF) >> 0) * 1.0f;
	stColor.xA = (float)255.0f;

	globals->lCurrentDrawMask = 0xFFFFFFFF;
	globals->lHierachDrawMask = 0xFFFFFFFF;
	globals->hCurrentMaterial = NULL;
	globals->gs_st_CurrentMatrix = *g_p_stCurrentMatrix;
	globals->gs_st_CameraMatrix = &p_st3DAtributes->p_stCam->stMatrix;
	globals->p_stCurrentCamera = p_st3DAtributes->p_stCam;
	GLI_DRV_vSetZClip((*GLI_BIG_GLOBALS)->p_stCurrentCamera->xNear, globals);

	/* Clamp the coordinates to the 0,0,640,480 range */
	/*
	if (a2_st2DVertex[0].xX < 0.0f) {
		a2_st2DVertex[0].xX = 0.0f;
	}
	if (a2_st2DVertex[0].xX > 640.0f) {
		a2_st2DVertex[0].xX = 640.0f;
	}
	if (a2_st2DVertex[0].xY < 0.0f) {
		a2_st2DVertex[0].xY = 0.0f;
	}
	if (a2_st2DVertex[0].xY > 480.0f) {
		a2_st2DVertex[0].xY = 480.0f;
	}

	if (a2_st2DVertex[1].xX < 0.0f) {
		a2_st2DVertex[1].xX = 0.0f;
	}
	if (a2_st2DVertex[1].xX > 640.0f) {
		a2_st2DVertex[1].xX = 640.0f;
	}
	if (a2_st2DVertex[1].xY < 0.0f) {
		a2_st2DVertex[1].xY = 0.0f;
	}
	if (a2_st2DVertex[1].xY > 480.0f) {
		a2_st2DVertex[1].xY = 480.0f;
	}
	*/
	/* END CLAMP */

	(*GLI_DRV_vSendSingleLineToClip)
	(
		p_stVpt,
		&a2_stVertex[0],
		&a2_st2DVertex[0],
		&a2_stVertex[1],
		&a2_st2DVertex[1],
		globals,
		C_lGouraudLineElement, &stColor
	);
}

void createTriangle(MTH3D_tdstVector p1, MTH3D_tdstVector p2, MTH3D_tdstVector p3, void* visualMat, float u1, float v1, float u2, float v2, float u3, float v3, float delayTime, unsigned long parameter) {
	AGO_vCreateTriangle(
		(float[]) {
		p1.x, p1.y, p1.z
	},
		(float[]) {
		p2.x, p2.y, p2.z
	},
		(float[]) {
		p3.x, p3.y, p3.z
	}, visualMat, u1, v1, u2, v2, u3, v3, delayTime, parameter);
}

DR_FEATURE_FUNC_Update(TriggerDisplay, feature) {

	HIE_tdstSuperObject* p_stSuperObj;

	LST_M_DynamicForEach((*GAM_g_p_stDynamicWorld), p_stSuperObj)
	{
		if (p_stSuperObj == rayman) {
			continue;
		}

		if (p_stSuperObj->ulType != HIE_C_Type_Actor) {
			continue;
		}

		HIE_tdstEngineObject* p_stActor = HIE_M_hSuperObjectGetActor(p_stSuperObj);

		MTH3D_tdstVector raymanPos = rayman->p_stGlobalMatrix->stPos;
		MTH3D_tdstVector pos = p_stSuperObj->p_stGlobalMatrix->stPos;

		MTH3D_tdstVector avgPos = (MTH3D_tdstVector){ raymanPos.x + pos.x * 0.5f, raymanPos.y + pos.y * 0.5f, raymanPos.z + pos.z * 0.5f };

		POS_tdstCompletePosition stMatrix;

		GLI_xGetCameraMatrix(((GLI_tdstSpecificAttributesFor3D*)(GAM_g_stEngineStructure->stViewportAttr.p_vSpecificToXD))->p_stCam, &stMatrix);
		GLI_xLoadMatrix(&stMatrix);

		GLI_xDraw3DLine16(&(GAM_g_stEngineStructure->stViewportAttr), &raymanPos, &pos, 0xFFFFFFF);

		GLI_xPopMatrix();

		if (!p_stActor->hStandardGame->ulCustomBits & (Std_C_CustBit_NoAIWhenTooFar | Std_C_CustBit_NoAnimPlayerWhenTooFar | Std_C_CustBit_NoMecaWhenTooFar)) {
			continue;
		}

		float tooFarLimit = (float)(p_stActor->hStandardGame->ucTooFarLimit) * 0.5f;
		if (tooFarLimit > 0) {


			float deltaX = raymanPos.x - pos.x;
			float deltaY = raymanPos.y - pos.y;
			float deltaZ = raymanPos.z - pos.z;

			float linearDist = (fabsf(deltaX) + fabsf(deltaY) + fabsf(deltaZ)) * 0.5f;
			float realDist = sqrtf(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);
			float normalizedX = deltaX / realDist;
			float normalizedY = deltaY / realDist;
			float normalizedZ = deltaZ / realDist;

			if (linearDist < tooFarLimit + C_TooFarLimitMargin && linearDist > tooFarLimit - C_TooFarLimitMargin) {

				float alpha = 1.0f - (fabsf(linearDist - tooFarLimit) / C_TooFarLimitMargin);
				float dist = 1.0f / (fabsf(normalizedX) + fabsf(normalizedY) + fabsf(normalizedZ));

				MTH3D_tdstVector diamondPos = (MTH3D_tdstVector){
					pos.x + (normalizedX * 2.0f * tooFarLimit * dist),
					pos.y + (normalizedY * 2.0f * tooFarLimit * dist),
					pos.z + (normalizedZ * 2.0f * tooFarLimit * dist),
				};

				AGO_vCreatePart(AGO_STILL | AGO_TIMELIMITED, &diamondPos, &MTH3D_C_ZeroVector, 0.02f, C_TooFarLimitParticleScale, C_TooFarLimitParticleScale, linearDist > tooFarLimit ? stCircleBlue : stCircleRed);
			}
		}
	}

	return NULL;
}

DR_FEATURE_FUNC_Display(TriggerDisplay, feature) {
}