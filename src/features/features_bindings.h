#pragma once

#define ActionRepeat(action) (IPT_g_stInputStructure->d_stEntryElementArray[(action)].lState == 1 || (IPT_g_stInputStructure->d_stEntryElementArray[(action)].lState > 15 && IPT_g_stInputStructure->d_stEntryElementArray[(action)].lState % 5 == 0))

#define MENU_ACTION_MASTER IPT_E_Entry_Action_Affiche_Jauge
#define MENU_TRIG_ACTIVATE IPT_M_bActionJustInvalidated(MENU_ACTION_MASTER)
#define MENU_TRIG_DEACTIVATE IPT_M_bActionJustInvalidated(MENU_ACTION_MASTER)

#define MENU_TRIG_VALIDATE IPT_M_bActionJustValidated(IPT_E_Entry_Action_Sauter)

#define MENU_TRIG_PREV ActionRepeat(IPT_E_Entry_Action_Camera_TourneGauche)
#define MENU_TRIG_NEXT ActionRepeat(IPT_E_Entry_Action_Camera_TourneDroite)

#define MENU_TRIG_RIGHT ActionRepeat(IPT_E_Entry_Action_Clavier_Droite)
#define MENU_TRIG_LEFT ActionRepeat(IPT_E_Entry_Action_Clavier_Gauche)
#define MENU_TRIG_UP ActionRepeat(IPT_E_Entry_Action_Clavier_Haut)
#define MENU_TRIG_DOWN ActionRepeat(IPT_E_Entry_Action_Clavier_Bas)