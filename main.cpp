#include <iostream>
#include <afxres.h>
#include <conio.h>
#include <cstdio>
#include "twain.h"

typedef TW_UINT16 (*_DSM_Entry)(pTW_IDENTITY pOrigin,
                               pTW_IDENTITY pDest,
                               TW_UINT32    DG,
                               TW_UINT16    DAT,
                               TW_UINT16    MSG,
                               TW_MEMREF    pData);


void set_capability(_DSM_Entry &pFunction, TW_IDENTITY &App, TW_IDENTITY &twIdentity) {
    TW_CAPABILITY tw_capability;
    TW_UINT16  res = pFunction(&App, &twIdentity, DG_CONTROL, DAT_CAPABILITY, MSG_GET, &tw_capability);
    printf("Set capability \n");

}


TW_UINT16 DSMCallback(pTW_IDENTITY _pOrigin,
                                 pTW_IDENTITY _pDest,
                                 TW_UINT32    _DG,
                                 TW_UINT16    _DAT,
                                 TW_UINT16    _MSG,
                                 TW_MEMREF    _pData){
    printf("CALLBACK \n");
}

TW_UINT16 register_callback(_DSM_Entry pFunction, TW_IDENTITY App, TW_IDENTITY twIdentity, HWND pHWND__) {
    TW_CALLBACK callback;
    callback.CallBackProc = (TW_MEMREF)DSMCallback;
    callback.RefCon = 0;
    TW_UINT16  res = pFunction(&App, &twIdentity, DG_CONTROL, DAT_CALLBACK, MSG_REGISTER_CALLBACK, (TW_MEMREF)&callback);
    return res;
}

TW_UINT16 show_ui(_DSM_Entry pFunction, TW_IDENTITY App, TW_IDENTITY twIdentity, HWND pHWND__) {
    TW_USERINTERFACE tw_userinterface;
    tw_userinterface.ShowUI = TRUE;
    tw_userinterface.ModalUI = FALSE;
    tw_userinterface.hParent = 0;
    TW_UINT16  res = pFunction(&App, &twIdentity, DG_CONTROL, DAT_USERINTERFACE, MSG_ENABLEDS, &tw_userinterface);
    return res;
}


int main() {
    TW_IDENTITY AppID;
    AppID.Id = 0;
    AppID.Version.MajorNum = 3; //Your app's version number
    AppID.Version.MinorNum = 5;
    AppID.Version.Language = TWLG_ENGLISH_USA;
    AppID.Version.Country = TWCY_USA;
    lstrcpy (AppID.Version.Info, "Your App's Version String");
    AppID.ProtocolMajor = 2; //Use yours not the one from twain.h
    AppID.ProtocolMinor = 2; //Use yours not the one from twain.h
    AppID.SupportedGroups = DF_APP2 | DG_IMAGE | DG_CONTROL;
    lstrcpy (AppID.Manufacturer, "App's Manufacturer");
    lstrcpy (AppID.ProductFamily, "App's Product Family");
    lstrcpy (AppID.ProductName, "Specific App Product Name");
    HMODULE hm = LoadLibrary("C:\\Users\\beznosovis\\Dropbox\\twaindsm-2.3.0.win.bin\\twain32\\TWAINDSM.dll");
    if(hm == NULL) {
        printf("TWAINDSM.dll not found\n");
    } else {
        _DSM_Entry dsm_entry = (_DSM_Entry)GetProcAddress(hm, "DSM_Entry");
        if(dsm_entry == NULL) {
            printf("Function not found\n");
        } else {
            printf("Load DDL. State 2\n");
            HWND parent = NULL;
            TW_UINT16 res = dsm_entry(&AppID, NULL, DG_CONTROL, DAT_PARENT, MSG_OPENDSM, parent);
            if(res == TWRC_SUCCESS) {
                printf("DSM is opened. State 3\n");
                TW_IDENTITY tw_identity;
                tw_identity.Id = 0;
                res = dsm_entry(&AppID, NULL, DG_CONTROL, DAT_IDENTITY, MSG_USERSELECT, (TW_MEMREF) &tw_identity);
                if(res == TWRC_CANCEL) {
                    printf("Cancel\n");
                } else if(res == TWRC_SUCCESS) {
//                    set_capability(dsm_entry, AppID, tw_identity);
                    res = dsm_entry(&AppID, NULL, DG_CONTROL, DAT_IDENTITY, MSG_OPENDS, (TW_MEMREF) &tw_identity);
                    if(res == TWRC_SUCCESS) {
                        printf("Success open DS\n");
//                        res = register_callback(dsm_entry, AppID, tw_identity, parent);
                        if(res == TWRC_SUCCESS) {
//                            printf("Success register callback\n");
                            res = show_ui(dsm_entry, AppID, tw_identity, parent);
                            if(res == TWRC_SUCCESS) {
                                printf("Show ui\n");
                                TW_UINT16  m_DSMessage = 0;
                                TW_UINT16  twRC;

                                while(!m_DSMessage)
                                {
                                    TW_EVENT twEvent;

//                                    // If we are using callbacks, there is nothing to do here except sleep
//                                    // and wait for our callback from the DS.  If we are not using them,
//                                    // then we have to poll the DSM.
//
//                                    // Pumping messages is for Windows only
                                    MSG Msg;
                                    if(!GetMessage((LPMSG)&Msg, NULL, 0, 0)) {
                                        break;
                                    }

//                                    twEvent.pEvent;
//                                    twEvent.TWMessage;
                                    TW_UINT16 twRC = dsm_entry(&AppID,
                                                       &tw_identity,
                                                       DG_CONTROL,
                                                       DAT_EVENT,
                                                       MSG_PROCESSEVENT,
                                                       (TW_MEMREF)&twEvent);

                                    if(twRC==TWRC_DSEVENT) {
                                        // check for message from Source
                                        switch (twEvent.TWMessage)
                                        {
                                            case MSG_XFERREADY:
                                            case MSG_CLOSEDSREQ:
                                            case MSG_CLOSEDSOK:
                                            case MSG_NULL:
//                                                gpTwainApplicationCMD->m_DSMessage = twEvent.TWMessage;
                                                break;

                                            default:
                                                printf("Error - Unknown message in MSG_PROCESSEVENT loop\n");
                                                break;
                                        }
                                    } else if(twRC == TWRC_FAILURE) {
                                        TranslateMessage ((LPMSG)&Msg);
                                        DispatchMessage ((LPMSG)&Msg);
                                        TW_STATUS twStatus;
                                        dsm_entry(&AppID, &tw_identity, DG_CONTROL, DAT_STATUS, MSG_GET, (TW_MEMREF) &twStatus);
                                        printf(twStatus.ConditionCode + "twStatus.\n");
                                    }

                                }
                                printf("All close\n");
                            } else if (res == TWRC_FAILURE) {
                                printf("Failed show ui\n");
                                TW_STATUS twStatus;
                                dsm_entry(&AppID, &tw_identity, DG_CONTROL, DAT_STATUS, MSG_GET, (TW_MEMREF) &twStatus);
                                printf(twStatus.ConditionCode + "twStatus.\n");
                            }
                        }
                    }
                } else if(res == TWRC_FAILURE) {
                    printf("Failure\n");
                }
            } else {
                printf("DSM is failed\n");
            }
        }
    }
    _getch();
    printf("Free library");
    FreeLibrary(hm);
    return 0;
}

