#pragma once

// DsgVar names for Rayman
typedef enum DV_Rayman {
	DV_RAY_POING_GrappinBis,
	DV_RAY_HELICO_SuperHelico,
	DV_RAY_HELICO_AccelerationVerticaleSuperHelico,
	DV_RAY_HELICO_GraviteSuperHelico,
	DV_RAY_HELICO_VitesseHorizontaleMax,
	DV_RAY_HELICO_VitesseInterdictionHelicoGlisse,
	DV_RAY_HELICO_PeutSauter,
	DV_RAY_ANIM_vecAxeDeRotation,
	DV_RAY_RAY_ReinitMode,
	DV_RAY_RAY_Etat,
	DV_RAY_RAY_PeutSeDeplacer,
	DV_RAY_RAY_Rebond,
	DV_RAY_RAY_RebondAutorise,
	DV_RAY_RAY_Gravite,
	DV_RAY_RAY_GraviteVariable,
	DV_RAY_RAY_SautRelatif,
	DV_RAY_RAY_ReagitAuxCommandes,
	DV_RAY_RAY_TpsChuteMaxi,
	DV_RAY_RAY_FightDistance,
	DV_RAY_INTERN_PoingTempsAppuye,
	DV_RAY_INTERN_PoingDemande,
	DV_RAY_INTERN_SpeedZ,
	DV_RAY_INTERN_RayVitesseInit,
	DV_RAY_INTERN_SAUT_TempsAppuye,
	DV_RAY_INTERN_BoucleSaut,
	DV_RAY_INTERN_SAUT_CompteurEssaisAccrochage,
	DV_RAY_INTERN_SAUT_VX,
	DV_RAY_INTERN_SAUT_VY,
	DV_RAY_INTERN_SAUT_VZ,
	DV_RAY_INTERN_SAUT_TYPE,
	DV_RAY_INTERN_SAUT_VY_LIMITE,
	DV_RAY_INTERN_BloqueVitesse,
	DV_RAY_INTERN_BloqueQuart,
	DV_RAY_INTERN_BLOC_NullSpeed,
	DV_RAY_INTERN_DateDernierContactAvecLeSol,
	DV_RAY_INTERN_Tmp,
	DV_RAY_INTERN_Tmp2,
	DV_RAY_INTERN_TmpInt,
	DV_RAY_INTERN_TmpVector,
	DV_RAY_INTERN_TmpVector1,
	DV_RAY_INTERN_TmpVector2,
	DV_RAY_INTERN_TmpPerso,
	DV_RAY_INTERN_mainDInit,
	DV_RAY_INTERN_mainGInit,
	DV_RAY_INTERN_EssaieAccrocher,
	DV_RAY_INTERN_GiBloqueAvant,
	DV_RAY_INTERN_HelicoPeutSauter,
	DV_RAY_INTERN_NAGE_rVY,
	DV_RAY_INTERN_NAGE_rRZ,
	DV_RAY_INTERN_NAGE_rAltitudeSurface,
	DV_RAY_INTERN_NAGE_bVeutAvancer,
	DV_RAY_INTERN_NAGE_bVeutPlonger,
	DV_RAY_INTERN_NAGE_bVeutTourner,
	DV_RAY_INTERN_NAGE_vecAssiette,
	DV_RAY_INTERN_NAGE_vecHorizon,
	DV_RAY_INTERN_NAGE_vecVisee,
	DV_RAY_INTERN_GRAPPIN_iRetournementEffectue,
	DV_RAY_Force_DecrocheGrappin,
	DV_RAY_INTERN_PersosRamassables,
	DV_RAY_PersoGenerated,
	DV_RAY_INTERN_PersoPlateForme,
	DV_RAY_INTERN_EntreDeuxPossible,
	DV_RAY_INTERN_EntreDeuxAxe,
	DV_RAY_INTERN_EntreDeuxVecteurPos,
	DV_RAY_INTERN_EntreDeuxMilieu,
	DV_RAY_INTERN_EntreDeuxPos,
	DV_RAY_INTERN_EntreDeuxProduitScalaire,
	DV_RAY_INTERN_EntreDeuxAngleAvecAxe,
	DV_RAY_INTERN_CollisionneurType,
	DV_RAY_INTERN_PafVecteur,
	DV_RAY_INTERN_VitessePere,
	DV_RAY_INTERN_VitesseDeRotation,
	DV_RAY_RAY_PafOriente,
	DV_RAY_pad_force_inertie,
	DV_RAY_PAD_Analog_Threshold,
	DV_RAY_PAD_Analog_WaitToWalk,
	DV_RAY_PAD_Analog_WalkToWait,
	DV_RAY_Time4RayTransparent,
	DV_RAY_RAY_RayIsHit,
	DV_RAY_INTERN_RebondEnCours,
	DV_RAY_INTERN_RattrappeLaPrune,
	DV_RAY_NAGE_rVitesseTranslationMax,
	DV_RAY_NAGE_HierarchieDeSurface,
	DV_RAY_SAUT_TempsImpulsionMaximal,
	DV_RAY_GI_RayLastVisee,
	DV_RAY_RAY_PeutPasserEnNageDepuisMarche,
	DV_RAY_INTERN_SautVitesseVersDescente,
	DV_RAY_INTERN_RaySlideSpeed,
	DV_RAY_INTERN_OrienteEnAttente,
	DV_RAY_InternSautAChangeAnimDescente,
	DV_RAY_INTERN_VientDeLHelico,
	DV_RAY_INTERN_NageProfondeur,
	DV_RAY_INTERN_EnGlisseRapide,
	DV_RAY_INTERN_GenereFumee,
	DV_RAY_INTERN_NewAssiete,
	DV_RAY_InitialPositionIsRemembered,
	DV_RAY_InitialTranslationInMap,
	DV_RAY_InitialSightInMap,
	DV_RAY_CoupeLePetitSaut,
	DV_RAY_RAY_CanGrab,
	DV_RAY_RAY_PeutFaireDelHelicoDepuisLaGlisse,
	DV_RAY_RAY_MaxRunSpeed,
	DV_RAY_RAY_ImpulsionSautAutomatique,
	DV_RAY_RAY_InfluencePenteSurVitesse,
	DV_RAY_RAY_InertiePadReduite,
	DV_RAY_RAY_IgnoreLesPersosNonAffiches,
	DV_RAY_RAY_SimplePaf,
	DV_RAY_RAY_ShootTargetWeightX,
	DV_RAY_RAY_ShootTargetWeightXY,
	DV_RAY_RAY_ShootTargetWeightXYZ,
	DV_RAY_INTERN_Tir_ActionAccumule,
	DV_RAY_INTERN_Tir_ActionCanarde,
	DV_RAY_INTERN_Tir_ActionRetourAttente,
	DV_RAY_INTERN_Tir_PeutAccumuler,
	DV_RAY_INTERN_Tir_AccumulationADonf,
	DV_RAY__COUNT,
} DV_Rayman;