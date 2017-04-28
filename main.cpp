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
    HMODULE hm = LoadLibrary("TWAINDSM.dll");
    if(hm == NULL) {
        printf("TWAINDSM.dll not found\n");
    } else {
        _DSM_Entry dsm_entry = (_DSM_Entry)GetProcAddress(hm, "DSM_Entry");
        if(dsm_entry == NULL) {
            printf("Function not found\n");
        } else {
            printf("Function found\n");
            printf("Initialize the Source Manager\n");
            dsm_entry(&AppID, NULL, DG_CONTROL, DAT_PARENT, MSG_OPENDSM, NULL);
            printf("Select the Source\n");
            if(AppID.SupportedGroups == DF_DS2) {
                TW_ENTRYPOINT tw_entrypoint;
                TW_UINT16 res = dsm_entry(&AppID, NULL, DG_CONTROL, DAT_ENTRYPOINT, MSG_GET, (TW_MEMREF) &tw_entrypoint);
            } else{
                TW_IDENTITY tw_identity;
                tw_identity.Id = 0;
               lstrcpy (tw_identity.ProductName, "\0");
                dsm_entry(&AppID, NULL, DG_CONTROL, DAT_IDENTITY, MSG_USERSELECT, (TW_MEMREF) &tw_identity);
                printf("selected available source\n");
            }

            printf("Source is select\n");

        }


    }
    FreeLibrary(hm);
    _getch();
//    TW_UINT16 rc;
//    rc = DSM_Entry(&AppID, NULL, DG_CONTROL, DAT_PARENT, MSG_OPENDSM, NULL);
    return 0;
}