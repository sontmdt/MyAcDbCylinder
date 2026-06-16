#define _ALLOW_RTCc_IN_STL

#include "StdAfx.h"
#include "resource.h"
#include "MyAcDbCylinder.h"

#define szRDS _RXST("TDS")
#include "acedCmdNF.h"

static void SetTopView()
{
    acedCommandS(RTSTR, L"_.PLAN", RTSTR, L"", RTNONE);
}

class CDrawApp : public AcRxArxApp
{
public:
    CDrawApp() : AcRxArxApp() {}

    virtual AcRx::AppRetCode On_kInitAppMsg(void* pkt)
    {
        AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg(pkt);

        MyAcDbCylinder::rxInit();
        acrxBuildClassHierarchy();

        SetTopView();

        return retCode;
    }

    virtual AcRx::AppRetCode On_kUnloadAppMsg(void* pkt)
    {
        AcRx::AppRetCode retCode = AcRxArxApp::On_kUnloadAppMsg(pkt);

        deleteAcRxClass(MyAcDbCylinder::desc());

        return retCode;
    }

    virtual void RegisterServerComponents() {}

    static void TDSMyGroupDraw()
    {
        SetTopView();

        double radius = 1.0;

        if (acedGetReal(L"\nNhap radius <1>: ", &radius) != RTNORM)
            return;

        ads_point p1, p2;

        if (acedGetPoint(NULL, L"\nChon tam day 1: ", p1) != RTNORM)
            return;

        if (acedGetPoint(p1, L"\nChon tam day 2: ", p2) != RTNORM)
            return;

        AcGePoint3d bottomCenter(p1[X], p1[Y], p1[Z]);
        AcGePoint3d topCenter(p2[X], p2[Y], p2[Z]);

        MyAcDbCylinder* pCyl =
            new MyAcDbCylinder(bottomCenter, topCenter, radius);

        AcDbDatabase* pDb =
            acdbHostApplicationServices()->workingDatabase();

        AcDbBlockTable* pBT = nullptr;
        pDb->getBlockTable(pBT, AcDb::kForRead);

        AcDbBlockTableRecord* pBTR = nullptr;
        pBT->getAt(
            ACDB_MODEL_SPACE,
            pBTR,
            AcDb::kForWrite);

        pBT->close();

        AcDbObjectId objId;
        pBTR->appendAcDbEntity(objId, pCyl);

        pBTR->close();
        pCyl->close();
    }
};

IMPLEMENT_ARX_ENTRYPOINT(CDrawApp)

ACED_ARXCOMMAND_ENTRY_AUTO(
    CDrawApp,
    TDSMyGroup,
    Draw,
    Draw,
    ACRX_CMD_MODAL,
    NULL)