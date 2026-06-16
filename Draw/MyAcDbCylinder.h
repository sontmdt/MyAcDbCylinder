#pragma once

#include "dbmain.h"
#include "dbents.h"
#include "gepnt3d.h"
#include "gevec3d.h"
#include "gemat3d.h"

class MyAcDbCylinder : public AcDbEntity
{
public:
    ACRX_DECLARE_MEMBERS(MyAcDbCylinder);

    MyAcDbCylinder();
    MyAcDbCylinder(
        const AcGePoint3d& bottomCenter,
        const AcGePoint3d& topCenter,
        double radius);
    virtual ~MyAcDbCylinder();

    // Getters
    double radius() const;
    AcGePoint3d bottomCenter() const;
    AcGePoint3d topCenter() const;
    double height() const;
    AcGeVector3d axis() const;

    // Setters
    Acad::ErrorStatus setRadius(double r);
    Acad::ErrorStatus setBottomCenter(const AcGePoint3d& pt);
    Acad::ErrorStatus setTopCenter(const AcGePoint3d& pt);

    // Drawing
    virtual Adesk::Boolean subWorldDraw(AcGiWorldDraw* pWd) override;
    virtual void subViewportDraw(AcGiViewportDraw* pVd) override;

    // Osnap
    virtual Acad::ErrorStatus subGetOsnapPoints(
        AcDb::OsnapMode     osnapMode,
        Adesk::GsMarker     gsSelectionMark,
        const AcGePoint3d& pickPoint,
        const AcGePoint3d& lastPoint,
        const AcGeMatrix3d& viewXform,
        AcGePoint3dArray& snapPoints,
        AcDbIntArray& geomIds) const override;

    // Grip points
    virtual Acad::ErrorStatus subGetGripPoints(
        AcGePoint3dArray& gripPoints,
        AcDbIntArray& osnapModes,
        AcDbIntArray& geomIds) const override;

    virtual Acad::ErrorStatus subMoveGripPointsAt(
        const AcDbIntArray& indices,
        const AcGeVector3d& offset) override;
    void dragStatus(
        const AcDb::DragStat status
    ) override;

    virtual Acad::ErrorStatus subTransformBy(const AcGeMatrix3d& xform) override;

    // Bounding box
    virtual Acad::ErrorStatus subGetGeomExtents(AcDbExtents& ext) const override;
    // I/O
    virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler* filer) const override;
    virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler* filer) override;
    virtual Acad::ErrorStatus dxfOutFields(AcDbDxfFiler* filer) const override;
    virtual Acad::ErrorStatus dxfInFields(AcDbDxfFiler* filer) override;

private:
    AcGePoint3d  m_bottomCenter;
    AcGePoint3d  m_topCenter;
    double       m_radius;

    AcDbObjectId wireBtrId;
    AcDbObjectId realisticBtrId;

    bool m_wireDirty;
    bool m_realisticDirty;
	bool m_dragging = false;
    // Geometry helpers
    AcGeVector3d getAxisX() const;

    void buildWireframe(
        AcGePoint3dArray& bottomRing,
        AcGePoint3dArray& topRing,
        int segments) const;

    void createWireframeBlock(AcDbBlockTableRecord* pBTR) const;
    void createRealisticBlock(AcDbBlockTableRecord* pBTR) const;

    // Representation cache management
    Acad::ErrorStatus ensureRepresentation(
        AcDbObjectId& btrId,
        bool& dirty,
        void (MyAcDbCylinder::* builder)(AcDbBlockTableRecord*) const);

    Acad::ErrorStatus createRepresentationBlocks();
    void invalidateRepresentationBlocks();
};