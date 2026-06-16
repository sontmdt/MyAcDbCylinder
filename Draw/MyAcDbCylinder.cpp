#define _ALLOW_RTCc_IN_STL

#include <math.h>
#include <tchar.h>
#include "StdAfx.h"
#include "MyAcDbCylinder.h"
#include <acgi.h>
#include <dbfiler.h>
#include <gemat3d.h>
#include <dbvisualstyle.h>
#include <acedCmdNF.h>

constexpr double PI = 3.14159265358979323846;

void LogType(AcGiVisualStyle::Type type)
{
    switch (type)
    {
    case AcGiVisualStyle::kFlat:
        acutPrintf(L"\nkFlat");
        break;

    case AcGiVisualStyle::kFlatWithEdges:
        acutPrintf(L"\nkFlatWithEdges");
        break;

    case AcGiVisualStyle::kGouraud:
        acutPrintf(L"\nkGouraud");
        break;

    case AcGiVisualStyle::kGouraudWithEdges:
        acutPrintf(L"\nkGouraudWithEdges");
        break;

    case AcGiVisualStyle::k2DWireframe:
        acutPrintf(L"\nk2DWireframe");
        break;

    case AcGiVisualStyle::k3DWireframe:
        acutPrintf(L"\nk3DWireframe");
        break;

    case AcGiVisualStyle::kHidden:
        acutPrintf(L"\nkHidden");
        break;

    case AcGiVisualStyle::kBasic:
        acutPrintf(L"\nkBasic");
        break;

    case AcGiVisualStyle::kRealistic:
        acutPrintf(L"\nkRealistic");
        break;

    case AcGiVisualStyle::kConceptual:
        acutPrintf(L"\nkConceptual");
        break;

    case AcGiVisualStyle::kCustom:
        acutPrintf(L"\nkCustom");
        break;

    case AcGiVisualStyle::kDim:
        acutPrintf(L"\nkDim");
        break;

    case AcGiVisualStyle::kBrighten:
        acutPrintf(L"\nkBrighten");
        break;

    case AcGiVisualStyle::kThicken:
        acutPrintf(L"\nkThicken");
        break;

    case AcGiVisualStyle::kLinePattern:
        acutPrintf(L"\nkLinePattern");
        break;

    case AcGiVisualStyle::kFacePattern:
        acutPrintf(L"\nkFacePattern");
        break;

    case AcGiVisualStyle::kColorChange:
        acutPrintf(L"\nkColorChange");
        break;

    case AcGiVisualStyle::kFaceOnly:
        acutPrintf(L"\nkFaceOnly");
        break;

    case AcGiVisualStyle::kEdgeOnly:
        acutPrintf(L"\nkEdgeOnly");
        break;

    case AcGiVisualStyle::kDisplayOnly:
        acutPrintf(L"\nkDisplayOnly");
        break;

    case AcGiVisualStyle::kJitterOff:
        acutPrintf(L"\nkJitterOff");
        break;

    case AcGiVisualStyle::kOverhangOff:
        acutPrintf(L"\nkOverhangOff");
        break;

    case AcGiVisualStyle::kEdgeColorOff:
        acutPrintf(L"\nkEdgeColorOff");
        break;

    case AcGiVisualStyle::kShadesOfGray:
        acutPrintf(L"\nkShadesOfGray");
        break;

    case AcGiVisualStyle::kSketchy:
        acutPrintf(L"\nkSketchy");
        break;

    case AcGiVisualStyle::kXRay:
        acutPrintf(L"\nkXRay");
        break;

    case AcGiVisualStyle::kShadedWithEdges:
        acutPrintf(L"\nkShadedWithEdges");
        break;

    case AcGiVisualStyle::kShaded:
        acutPrintf(L"\nkShaded");
        break;

    case AcGiVisualStyle::kByViewport:
        acutPrintf(L"\nkByViewport");
        break;

    case AcGiVisualStyle::kByLayer:
        acutPrintf(L"\nkByLayer");
        break;

    case AcGiVisualStyle::kByBlock:
        acutPrintf(L"\nkByBlock");
        break;

    case AcGiVisualStyle::kEmptyStyle:
        acutPrintf(L"\nkEmptyStyle");
        break;

    default:
        acutPrintf(L"\nUnknown (%d)", static_cast<int>(type));
        break;
    }
}

static AcGiVisualStyle::Type getViewportVisualStyleType(AcGiViewportDraw* pViewportDraw)
{
    if (!pViewportDraw)
        return AcGiVisualStyle::kEmptyStyle;

    AcDbObjectId vsId = pViewportDraw->subEntityTraits().visualStyle();
    if (vsId.isNull()) {
        AcDbObjectId vpObjId = pViewportDraw->viewportObjectId();
        if (vpObjId.isNull())

            return AcGiVisualStyle::kEmptyStyle;

        AcDbObject* pObj = nullptr;
        if (acdbOpenObject(pObj, vpObjId, AcDb::kForRead) != Acad::eOk || !pObj)
            return AcGiVisualStyle::kEmptyStyle;

        if (AcDbViewport* pVp = AcDbViewport::cast(pObj)) {
            vsId = pVp->visualStyle();
        }
        else if (AcDbViewportTableRecord* pVtr = AcDbViewportTableRecord::cast(pObj)) {
            vsId = pVtr->visualStyle();
        }
        pObj->close();
    }

    if (vsId.isNull())
        return AcGiVisualStyle::kEmptyStyle;

    AcDbVisualStyle* pVs = nullptr;
    if (acdbOpenObject(pVs, vsId, AcDb::kForRead) != Acad::eOk || !pVs)
        return AcGiVisualStyle::kEmptyStyle;

    AcGiVisualStyle::Type t = pVs->type();
    pVs->close();
    return t;
}

ACRX_DXF_DEFINE_MEMBERS(
    MyAcDbCylinder,
    AcDbEntity,
    AcDb::kDHL_CURRENT,
    AcDb::kMReleaseCurrent,
    AcDbProxyEntity::kNoOperation,
    MYACDBCYLINDER,
    ""
)

// Constructors

MyAcDbCylinder::MyAcDbCylinder()
    : m_bottomCenter(0, 0, 0)
    , m_topCenter(0, 0, 1)
    , m_radius(1.0)
{
}

MyAcDbCylinder::MyAcDbCylinder(
    const AcGePoint3d& bottomCenter,
    const AcGePoint3d& topCenter,
    double radius)
    : m_bottomCenter(bottomCenter)
    , m_topCenter(topCenter)
    , m_radius(radius)
{
}

MyAcDbCylinder::~MyAcDbCylinder() {}

// Getters

double MyAcDbCylinder::radius() const { return m_radius; }
AcGePoint3d MyAcDbCylinder::bottomCenter() const { return m_bottomCenter; }
AcGePoint3d MyAcDbCylinder::topCenter() const { return m_topCenter; }
double MyAcDbCylinder::height() const { return m_bottomCenter.distanceTo(m_topCenter); }
AcGeVector3d MyAcDbCylinder::axis() const { return m_topCenter - m_bottomCenter; }

// Setters

Acad::ErrorStatus MyAcDbCylinder::setRadius(double r)
{
    if (r <= 0) return Acad::eInvalidInput;
    assertWriteEnabled();
    m_radius = r;
    invalidateRepresentationBlocks();
    return Acad::eOk;
}

Acad::ErrorStatus MyAcDbCylinder::setBottomCenter(const AcGePoint3d& pt)
{
    assertWriteEnabled();
    m_bottomCenter = pt;
    invalidateRepresentationBlocks();
    return Acad::eOk;
}

Acad::ErrorStatus MyAcDbCylinder::setTopCenter(const AcGePoint3d& pt)
{
    assertWriteEnabled();
    m_topCenter = pt;
    invalidateRepresentationBlocks();
    return Acad::eOk;
}

void MyAcDbCylinder::buildWireframe(
    AcGePoint3dArray& bottomRing,
    AcGePoint3dArray& topRing,
    int segments) const
{
    AcGeVector3d axisZ = axis().normal();
    AcGeVector3d axisX = axisZ.perpVector().normal();
    AcGeVector3d axisY = axisZ.crossProduct(axisX).normal();
    double step = 2.0 * PI / segments;
    for (int i = 0; i <= segments; i++) {
        double angle = i * step;
        AcGeVector3d offset = axisX * (m_radius * cos(angle))
            + axisY * (m_radius * sin(angle));
        bottomRing.append(m_bottomCenter + offset);
        topRing.append(m_topCenter + offset);
    }
}

static Acad::ErrorStatus requireAnonymousBTR(
    AcDbEntity* pEnt,
    AcDbObjectId& outBtrId)
{
    if (!outBtrId.isNull()) return Acad::eOk;

    AcDbDatabase* pDb = pEnt->database();
    if (!pDb) pDb = acdbHostApplicationServices()->workingDatabase();

    if (!pDb) return Acad::eNoDatabase;

    AcDbBlockTable* pBT = nullptr;
    Acad::ErrorStatus es = pDb->getBlockTable(pBT, AcDb::kForWrite);

    if (es != Acad::eOk) return es;

    AcDbBlockTableRecord* pBTR = new AcDbBlockTableRecord();

    pBTR->setName(L"*U");

    es = pBT->add(outBtrId, pBTR);

    pBTR->close();
    pBT->close();

    return es;
}

Acad::ErrorStatus MyAcDbCylinder::createRepresentationBlocks()
{
    assertWriteEnabled();

    if (wireBtrId.isNull())
    {
        Acad::ErrorStatus es = requireAnonymousBTR(this, wireBtrId);

        if (es != Acad::eOk)
            return es;

        AcDbBlockTableRecord* pBTR = nullptr;

        es = acdbOpenObject(
            pBTR,
            wireBtrId,
            AcDb::kForWrite);

        if (es != Acad::eOk)
            return es;

        createWireframeBlock(pBTR);

        pBTR->close();
    }
    if (realisticBtrId.isNull())
    {
        Acad::ErrorStatus es =
            requireAnonymousBTR(this, realisticBtrId);

        if (es != Acad::eOk)
            return es;

        AcDbBlockTableRecord* pBTR = nullptr;

        es = acdbOpenObject(
            pBTR,
            realisticBtrId,
            AcDb::kForWrite);

        if (es != Acad::eOk)
            return es;

        createRealisticBlock(pBTR);

        pBTR->close();
    }

    return Acad::eOk;
}

static void clearBlockEntities(AcDbBlockTableRecord* pBTR)
{
    AcDbBlockTableRecordIterator* pIter = nullptr;
    pBTR->newIterator(pIter);

    for (; !pIter->done(); pIter->step())
    {
        AcDbEntity* pEnt = nullptr;
        Acad::ErrorStatus es = pIter->getEntity(pEnt, AcDb::kForWrite);
        if (es == Acad::eOk)
        {
            pEnt->erase();
            pEnt->close();
        }
    }

    delete pIter;
}

void MyAcDbCylinder::invalidateRepresentationBlocks()
{
    if (!wireBtrId.isNull())
    {
        AcDbBlockTableRecord* pBTR = nullptr;
        Acad::ErrorStatus es = acdbOpenObject(pBTR, wireBtrId, AcDb::kForWrite);
        if (es == Acad::eOk)
        {
            clearBlockEntities(pBTR);
            createWireframeBlock(pBTR);
            pBTR->close();
        }
    }

    if (!realisticBtrId.isNull())
    {
        AcDbBlockTableRecord* pBTR = nullptr;
        Acad::ErrorStatus es = acdbOpenObject(pBTR, realisticBtrId, AcDb::kForWrite);
        if (es == Acad::eOk)
        {
            clearBlockEntities(pBTR);
            createRealisticBlock(pBTR);
            pBTR->close();
        }
    }

    recordGraphicsModified(true);
}

AcGeVector3d MyAcDbCylinder::getAxisX() const
{
    AcGeVector3d axisZ_cyl = axis().normal();
    AcGeVector3d worldZ(0, 0, 1);

    AcGeVector3d cross = axisZ_cyl.crossProduct(worldZ);
    if (cross.length() < 1e-6)
        return axisZ_cyl.perpVector().normal() * m_radius;

    return cross.normal() * m_radius;
}

void MyAcDbCylinder::createWireframeBlock(AcDbBlockTableRecord* pBTR) const
{
    AcDbObjectId tmpId;

    AcGeVector3d axisX = getAxisX();

    AcDbLine* pLeft = new AcDbLine(m_bottomCenter - axisX,
        m_topCenter - axisX);
    AcDbLine* pRight = new AcDbLine(m_bottomCenter + axisX,
        m_topCenter + axisX);
    AcDbLine* pBot = new AcDbLine(m_bottomCenter - axisX,
        m_bottomCenter + axisX);
    AcDbLine* pTop = new AcDbLine(m_topCenter - axisX,
        m_topCenter + axisX);

    pBTR->appendAcDbEntity(tmpId, pLeft);  pLeft->close();
    pBTR->appendAcDbEntity(tmpId, pRight); pRight->close();
    pBTR->appendAcDbEntity(tmpId, pBot);   pBot->close();
    pBTR->appendAcDbEntity(tmpId, pTop);   pTop->close();

    AcDbLine* pAxis = new AcDbLine(m_bottomCenter, m_topCenter);
    pAxis->setColorIndex(2);

    AcDbLinetypeTable* pLT = nullptr;
    AcDbDatabase* pDb = pBTR->database();
    if (pDb) {
        pDb->getLinetypeTable(pLT, AcDb::kForRead);
        AcDbObjectId ltId;
        if (pLT->getAt(L"DASHDOT", ltId) == Acad::eOk)
            pAxis->setLinetype(ltId);
        pLT->close();
    }

    pBTR->appendAcDbEntity(tmpId, pAxis);
    pAxis->close();
}
void MyAcDbCylinder::createRealisticBlock(AcDbBlockTableRecord* pBTR) const
{
    const int segments = 32;
    AcGePoint3dArray bottomRing, topRing;
    buildWireframe(bottomRing, topRing, segments);

    AcDbPolyFaceMesh* pMesh = new AcDbPolyFaceMesh();
    AcDbObjectId meshId;
    pBTR->appendAcDbEntity(meshId, pMesh);

    for (int i = 0; i <= segments; i++) {
        AcDbPolyFaceMeshVertex* pV = new AcDbPolyFaceMeshVertex(topRing[i]);
        AcDbObjectId vId;
        pMesh->appendVertex(vId, pV);
        pV->close();
    }
    for (int i = 0; i <= segments; i++) {
        AcDbPolyFaceMeshVertex* pV = new AcDbPolyFaceMeshVertex(bottomRing[i]);
        AcDbObjectId vId;
        pMesh->appendVertex(vId, pV);
        pV->close();
    }

    for (int i = 0; i < segments; i++) {
        AcDbFaceRecord* pFace = new AcDbFaceRecord(
            i + 1,
            i + 2,
            (segments + 1) + i + 2,
            (segments + 1) + i + 1);
        AcDbObjectId fId;
        pMesh->appendFaceRecord(fId, pFace);
        pFace->close();
    }
    pMesh->close();

    for (int i = 0; i < segments; i++) {
        int next = (i + 1) % segments;
        AcDbFace* pFace = new AcDbFace(
            m_bottomCenter,
            bottomRing[i],
            bottomRing[next],
            bottomRing[next]);
        AcDbObjectId fId;
        pBTR->appendAcDbEntity(fId, pFace);
        pFace->close();
    }

    for (int i = 0; i < segments; i++) {
        int next = (i + 1) % segments;
        AcDbFace* pFace = new AcDbFace(
            m_topCenter,
            topRing[next],
            topRing[i],
            topRing[i]);
        AcDbObjectId fId;
        pBTR->appendAcDbEntity(fId, pFace);
        pFace->close();
    }
}

void MyAcDbCylinder::dragStatus(
    const AcDb::DragStat status)
{
    //acutPrintf(L"\n[dragStatus] start");
    switch (status)
    {
    case AcDb::kDragStart:
		m_dragging = true;
        //acutPrintf(L"\n[dragStatus] kDragStart: %d", m_dragging);
        break;

    case AcDb::kDragEnd:
		m_dragging = false;
        //acutPrintf(L"\n[dragStatus] kDragEnd: %d", m_dragging);
        break;

    case AcDb::kDragAbort:
		m_dragging = false;
        //acutPrintf(L"\n[dragStatus] kDragAbort");
        break;
    }
    //acutPrintf(L"\n[dragStatus] end: %d", m_dragging);
}

Adesk::Boolean MyAcDbCylinder::subWorldDraw(AcGiWorldDraw* pWd)
{
    acutPrintf(L"\n[subWorldDraw]: %d", m_dragging);
    if (m_dragging == true)
    {
        AcGePoint3dArray bottomRing, topRing;
        buildWireframe(bottomRing, topRing, 32);

        for (int i = 0; i < 32; i++)
        {
            int next = (i + 1) % 32;

            pWd->geometry().polyline(2, &bottomRing[i]);
            pWd->geometry().polyline(2, &topRing[i]);

            AcGePoint3d pts[2] = { bottomRing[i], topRing[i] };
            pWd->geometry().polyline(2, pts);
        }
    }
    //else acutPrintf(L"\n[subWorldDraw] isDragging = FALSE this=%p", this);
    return Adesk::kFalse;
}

void MyAcDbCylinder::subViewportDraw(AcGiViewportDraw* pVd)
{
    assertReadEnabled();

    if (wireBtrId.isNull() || realisticBtrId.isNull())
    {
        upgradeOpen();
        createRepresentationBlocks();
        downgradeOpen();
    }

    AcDbBlockTableRecord* pBTR = nullptr;

    switch (getViewportVisualStyleType(pVd))
    {
        case AcGiVisualStyle::kRealistic:
            if (!realisticBtrId.isNull())
                acdbOpenObject(pBTR, realisticBtrId, AcDb::kForRead);
            break;
        default:
            if (!wireBtrId.isNull())
                acdbOpenObject(pBTR, wireBtrId, AcDb::kForRead);
            break;
    }

    if (pBTR != nullptr)
    {
        pVd->geometry().draw(pBTR);
        pBTR->close();
    }
}

Acad::ErrorStatus MyAcDbCylinder::subGetOsnapPoints(
    AcDb::OsnapMode     osnapMode,
    Adesk::GsMarker     gsSelectionMark,
    const AcGePoint3d& pickPoint,
    const AcGePoint3d& lastPoint,
    const AcGeMatrix3d& viewXform,
    AcGePoint3dArray& snapPoints,
    AcDbIntArray& geomIds) const
{
    assertReadEnabled();
    switch (osnapMode) {
    case AcDb::kOsModeIns:
    case AcDb::kOsModeCen:
        snapPoints.append(m_bottomCenter);
        snapPoints.append(m_topCenter);
        snapPoints.append(AcGePoint3d(
            (m_bottomCenter.x + m_topCenter.x) / 2.0,
            (m_bottomCenter.y + m_topCenter.y) / 2.0,
            (m_bottomCenter.z + m_topCenter.z) / 2.0
        ));
        break;
    default:
        break;
    }
    return Acad::eOk;
}

// Grip points

Acad::ErrorStatus MyAcDbCylinder::subGetGripPoints(
    AcGePoint3dArray& gripPoints,
    AcDbIntArray& osnapModes,
    AcDbIntArray& geomIds) const
{
    assertReadEnabled();

    AcGeVector3d axisX = getAxisX();

    AcGePoint3d midCenter(
        (m_bottomCenter.x + m_topCenter.x) / 2.0,
        (m_bottomCenter.y + m_topCenter.y) / 2.0,
        (m_bottomCenter.z + m_topCenter.z) / 2.0
    );

    gripPoints.append(m_bottomCenter);
    osnapModes.append(AcDb::kOsModeCen);
    geomIds.append(0);

    gripPoints.append(m_topCenter);
    osnapModes.append(AcDb::kOsModeCen);
    geomIds.append(0);

    gripPoints.append(midCenter);
    osnapModes.append(AcDb::kOsModeMid);
    geomIds.append(0);

    gripPoints.append(m_bottomCenter - axisX);
    osnapModes.append(0);
    geomIds.append(0);

    gripPoints.append(m_bottomCenter + axisX);
    osnapModes.append(0);
    geomIds.append(0);

    gripPoints.append(m_topCenter - axisX);
    osnapModes.append(0);
    geomIds.append(0);

    gripPoints.append(m_topCenter + axisX);
    osnapModes.append(0);
    geomIds.append(0);

    return Acad::eOk;
}

Acad::ErrorStatus MyAcDbCylinder::subMoveGripPointsAt(
    const AcDbIntArray& indices,
    const AcGeVector3d& offset)
{
    assertWriteEnabled();
    //acutPrintf(L"\n[subMoveGripPointsAt]");
    AcGeVector3d axisX = getAxisX();
    AcGeVector3d axisXDir = axisX.normal();

    for (int i = 0; i < indices.length(); i++) {
        if (indices[i] == 0) m_bottomCenter += offset;
        if (indices[i] == 1) m_topCenter += offset;
        if (indices[i] == 2) {
            m_bottomCenter += offset;
            m_topCenter += offset;
        }
        if (indices[i] == 3) m_radius -= offset.dotProduct(axisXDir);
        if (indices[i] == 4) m_radius += offset.dotProduct(axisXDir);
        if (indices[i] == 5) m_radius -= offset.dotProduct(axisXDir);
        if (indices[i] == 6) m_radius += offset.dotProduct(axisXDir);
    }

    if (m_radius < 1e-6) m_radius = 1e-6;

    invalidateRepresentationBlocks();

    return Acad::eOk;
}

// Transform 

Acad::ErrorStatus MyAcDbCylinder::subTransformBy(const AcGeMatrix3d& xform)
{
    assertWriteEnabled();
    m_bottomCenter.transformBy(xform);
    m_topCenter.transformBy(xform);

    AcGeVector3d scaleVec(
        xform.entry[0][0],
        xform.entry[1][0],
        xform.entry[2][0]
    );
    m_radius *= scaleVec.length();

    invalidateRepresentationBlocks();

    return Acad::eOk;
}
// Bounding box 

Acad::ErrorStatus MyAcDbCylinder::subGetGeomExtents(AcDbExtents& ext) const
{
    assertReadEnabled();
    AcGePoint3dArray bottomRing, topRing;
    buildWireframe(bottomRing, topRing, 32);

    for (int i = 0; i < bottomRing.length(); i++) ext.addPoint(bottomRing[i]);
    for (int i = 0; i < topRing.length(); i++) ext.addPoint(topRing[i]);
    return Acad::eOk;
}

// I/O

Acad::ErrorStatus MyAcDbCylinder::dwgOutFields(AcDbDwgFiler* filer) const
{
    assertReadEnabled();
    AcDbEntity::dwgOutFields(filer);
    filer->writePoint3d(m_bottomCenter);
    filer->writePoint3d(m_topCenter);
    filer->writeDouble(m_radius);
    return filer->filerStatus();
}

Acad::ErrorStatus MyAcDbCylinder::dwgInFields(AcDbDwgFiler* filer)
{
    assertWriteEnabled();
    AcDbEntity::dwgInFields(filer);
    filer->readPoint3d(&m_bottomCenter);
    filer->readPoint3d(&m_topCenter);
    filer->readDouble(&m_radius);
    return filer->filerStatus();
}

Acad::ErrorStatus MyAcDbCylinder::dxfOutFields(AcDbDxfFiler* filer) const
{
    assertReadEnabled();
    Acad::ErrorStatus es = AcDbEntity::dxfOutFields(filer);
    if (es != Acad::eOk) return es;

    filer->writeDouble(AcDb::kDxfReal, m_radius);
    filer->writePoint3d(AcDb::kDxfXCoord, m_bottomCenter);
    filer->writePoint3d(AcDb::kDxfXCoord + 1, m_topCenter);
    return filer->filerStatus();
}

Acad::ErrorStatus MyAcDbCylinder::dxfInFields(AcDbDxfFiler* filer)
{
    assertWriteEnabled();
    Acad::ErrorStatus es = AcDbEntity::dxfInFields(filer);
    if (es != Acad::eOk) return es;

    struct resbuf rb;
    while (filer->readResBuf(&rb) == Acad::eOk) {
        switch (rb.restype) {
        case AcDb::kDxfReal:
            m_radius = rb.resval.rreal;
            break;
        case AcDb::kDxfXCoord:
            m_bottomCenter = AcGePoint3d(
                rb.resval.rpoint[0],
                rb.resval.rpoint[1],
                rb.resval.rpoint[2]);
            break;
        case AcDb::kDxfXCoord + 1:
            m_topCenter = AcGePoint3d(
                rb.resval.rpoint[0],
                rb.resval.rpoint[1],
                rb.resval.rpoint[2]);
            break;
        case AcDb::kDxfReal + 1:
            break;
        }
    }
    return filer->filerStatus();
}