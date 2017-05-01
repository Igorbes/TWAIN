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

TW_UINT16 FAR PASCAL DSMCallback(pTW_IDENTITY _pOrigin,
                                 pTW_IDENTITY _pDest,
                                 TW_UINT32    _DG,
                                 TW_UINT16    _DAT,
                                 TW_UINT16    _MSG,
                                 TW_MEMREF    _pData){}

void show_ui(_DSM_Entry pFunction, TW_IDENTITY App, TW_IDENTITY twIdentity, HWND pHWND__) {
    TW_USERINTERFACE tw_userinterface;
    tw_userinterface.ShowUI = TRUE;
    tw_userinterface.ModalUI = FALSE;
    tw_userinterface.hParent = NULL;
    TW_UINT16  res = pFunction(&App, &twIdentity, DG_CONTROL, DAT_USERINTERFACE, MSG_ENABLEDS, &tw_userinterface);
    if(TWRC_SUCCESS == res) {
        printf("Show ui \n");
        TW_CALLBACK callback;
        callback.CallBackProc = (TW_MEMREF)DSMCallback;
        callback.RefCon = 0;

        res = pFunction(&App, &twIdentity, DG_CONTROL, DAT_CALLBACK, MSG_REGISTER_CALLBACK, (TW_MEMREF)&callback);
        if(TWRC_SUCCESS == res) {
           printf("Registered callback");
        }
    }
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
    HMODULE hm = LoadLibrary("C:\\Users\\Monstermash\\Dropbox\\twaindsm-2.3.0.win.bin\\twain32\\TWAINDSM.dll");
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
                    set_capability(dsm_entry, AppID, tw_identity);
                    res = dsm_entry(&AppID, NULL, DG_CONTROL, DAT_IDENTITY, MSG_OPENDS, (TW_MEMREF) &tw_identity);
                    if(res == TWRC_SUCCESS) {
                        printf("Success open DS\n");
                        show_ui(dsm_entry, AppID, tw_identity, parent);

                    }
                } else if(res == TWRC_FAILURE) {
                    printf("Failure\n");
                }
            } else {
                printf("DSM is failed\n");
            }
        }
    }
    std::string input;
//    for (;;)
//    {
//        std::cout << "\n(h for help) > ";
//        std::cin >> input;
//        std::cout << std::endl;
//    }
    _getch();
    printf("Free library");
    FreeLibrary(hm);
    return 0;
}

