#include "StdAfx.h"

#include <vphysics/virtualmesh.h>

#include "Physics_Collision.h"
#include "convert.h"
#include "Physics_KeyParser.h"
#include "phydata.h"

#include "tier0/vprof.h"

// memdbgon must be the last include file in a .cpp file!!!
//#include "tier0/memdbgon.h"

/****************************
* CLASS CCollisionQuery
****************************/

// FIXME: Bullet doesn't use triangles to represent shapes internally!
class CCollisionQuery : public ICollisionQuery {
	public:
		CCollisionQuery(btCollisionShape *pShape) {m_pShape = pShape;}

		// number of convex pieces in the whole solid
		int					ConvexCount();
		// triangle count for this convex piece
		int					TriangleCount(int convexIndex);
		// get the stored game data
		uint				GetGameData(int convexIndex);
		// Gets the triangle's verts to an array
		void				GetTriangleVerts(int convexIndex, int triangleIndex, Vector *verts);
		void				SetTriangleVerts(int convexIndex, int triangleIndex, const Vector *verts);
		
		// returns the 7-bit material index
		int					GetTriangleMaterialIndex(int convexIndex, int triangleIndex);
		// sets a 7-bit material index for this triangle
		void				SetTriangleMaterialIndex(int convexIndex, int triangleIndex, int index7bits);

	private:
		btCollisionShape *	m_pShape;
};

int CCollisionQuery::ConvexCount() {
	if (m_pShape->isCompound()) {
		btCompoundShape *pShape = (btCompoundShape *)m_pShape;
		return pShape->getNumChildShapes();
	}

	return 0;
}

int CCollisionQuery::TriangleCount(int convexIndex) {
	NOT_IMPLEMENTED
	return 0;
}

unsigned int CCollisionQuery::GetGameData(int convexIndex) {
	NOT_IMPLEMENTED
	return 0;
}

void CCollisionQuery::GetTriangleVerts(int convexIndex, int triangleIndex, Vector *verts) {
	NOT_IMPLEMENTED
}

void CCollisionQuery::SetTriangleVerts(int convexIndex, int triangleIndex, const Vector *verts) {
	NOT_IMPLEMENTED
}

int CCollisionQuery::GetTriangleMaterialIndex(int convexIndex, int triangleIndex) {
	NOT_IMPLEMENTED
	return 0;
}

void CCollisionQuery::SetTriangleMaterialIndex(int convexIndex, int triangleIndex, int index7bits) {
	NOT_IMPLEMENTED
}

/****************************
* CLASS CPhysicsCollision
****************************/

// NOTE:
// CPhysCollide is a btCompoundShape
// CPhysConvex is a btConvexHullShape

CPhysConvex *CPhysicsCollision::ConvexFromVerts(Vector **pVerts, int vertCount) {
	btConvexHullShape *pConvex = new btConvexHullShape;

	for (int i = 0; i < vertCount; i++) {
		btVector3 vert;
		ConvertPosToBull(*pVerts[i], vert);

		pConvex->addPoint(vert);
	}

	// Optimize the shape.
	btShapeHull *hull = new btShapeHull(pConvex);
	hull->buildHull(COLLISION_MARGIN);
	delete pConvex;
	pConvex = new btConvexHullShape((btScalar *)hull->getVertexPointer(), hull->numVertices());
	delete hull;

	return (CPhysConvex *)pConvex;
}

CPhysConvex *CPhysicsCollision::ConvexFromPlanes(float *pPlanes, int planeCount, float mergeDistance) {
	NOT_IMPLEMENTED
	return NULL;
}

float CPhysicsCollision::ConvexVolume(CPhysConvex *pConvex) {
	NOT_IMPLEMENTED
	return 0;
}

float CPhysicsCollision::ConvexSurfaceArea(CPhysConvex *pConvex) {
	NOT_IMPLEMENTED
	return 0;
}

void CPhysicsCollision::ConvexFree(CPhysConvex *pConvex) {
	delete (btConvexShape *)pConvex;
}

void CPhysicsCollision::SetConvexGameData(CPhysConvex *pConvex, unsigned int gameData) {
	NOT_IMPLEMENTED
}

CPolyhedron *CPhysicsCollision::PolyhedronFromConvex(CPhysConvex *const pConvex, bool bUseTempPolyhedron) {
	NOT_IMPLEMENTED
	return NULL;
}

CPhysConvex *CPhysicsCollision::ConvexFromConvexPolyhedron(const CPolyhedron &ConvexPolyhedron) {
	NOT_IMPLEMENTED
	return NULL;
}

void CPhysicsCollision::ConvexesFromConvexPolygon(const Vector &vPolyNormal, const Vector *pPoints, int iPointCount, CPhysConvex **ppOutput) {
	NOT_IMPLEMENTED
}

// TODO: Support this, lua Entity:PhysicsInitMultiConvex uses this!
// IVP internally used QHull to generate the convexes.
CPhysPolysoup *CPhysicsCollision::PolysoupCreate() {
	NOT_IMPLEMENTED
	return NULL;
}

void CPhysicsCollision::PolysoupDestroy(CPhysPolysoup *pSoup) {
	NOT_IMPLEMENTED
}

void CPhysicsCollision::PolysoupAddTriangle(CPhysPolysoup *pSoup, const Vector &a, const Vector &b, const Vector &c, int materialIndex7bits) {
	NOT_IMPLEMENTED
}

CPhysCollide *CPhysicsCollision::ConvertPolysoupToCollide(CPhysPolysoup *pSoup, bool useMOPP) {
	NOT_IMPLEMENTED
	return NULL;
}

CPhysCollide *CPhysicsCollision::ConvertConvexToCollide(CPhysConvex **ppConvex, int convexCount) {
	if (convexCount == 0) return NULL;

	btCompoundShape *pCompound = new btCompoundShape;
	for (int i = 0; i < convexCount; i++) {
		btCollisionShape *pShape = (btCollisionShape *)ppConvex[i];
		pCompound->addChildShape(btTransform::getIdentity(), pShape);
	}

	return (CPhysCollide *)pCompound;
}

CPhysCollide *CPhysicsCollision::ConvertConvexToCollideParams(CPhysConvex **pConvex, int convexCount, const convertconvexparams_t &convertParams) {
	NOT_IMPLEMENTED
	return ConvertConvexToCollide(pConvex, convexCount);
}

void CPhysicsCollision::DestroyCollide(CPhysCollide *pCollide) {
	if (!pCollide) return;

	btCollisionShape *pShape = (btCollisionShape *)pCollide;

	// Compound shape? Delete all of it's children.
	if (pShape->isCompound()) {
		btCompoundShape *pCompound = (btCompoundShape *)pShape;
		int numChildShapes = pCompound->getNumChildShapes();
		for (int i = 0; i < numChildShapes; i++) {
			delete (btCollisionShape *)pCompound->getChildShape(i);
		}

		// Also delete our PhysicsShapeInfo thing.
		delete (PhysicsShapeInfo *)pCompound->getUserPointer();
	}

	delete pShape;
}

int CPhysicsCollision::CollideSize(CPhysCollide *pCollide) {
	NOT_IMPLEMENTED
	return 0;
}

int CPhysicsCollision::CollideWrite(char *pDest, CPhysCollide *pCollide, bool bSwap) {
	NOT_IMPLEMENTED
	return 0;
}

CPhysCollide *CPhysicsCollision::UnserializeCollide(char *pBuffer, int size, int index) {
	NOT_IMPLEMENTED
	return NULL;
}

float CPhysicsCollision::CollideVolume(CPhysCollide *pCollide) {
	NOT_IMPLEMENTED
	return 0;
}

float CPhysicsCollision::CollideSurfaceArea(CPhysCollide *pCollide) {
	NOT_IMPLEMENTED
	return 0;
}

// This'll return the farthest possible vector that's still within our collision mesh.
// direction argument is a normalized Vector, literally a direction.
// TODO: Methods on doing this?
// Does bullet provide any APIs for doing this?
// Should we do a raytrace that starts outside of our mesh and goes inwards?
Vector CPhysicsCollision::CollideGetExtent(const CPhysCollide *pCollide, const Vector &collideOrigin, const QAngle &collideAngles, const Vector &direction) {
	VPROF_BUDGET("CPhysicsCollision::CollideGetExtent", VPROF_BUDGETGROUP_PHYSICS);
	if (!pCollide) return collideOrigin;

	btCollisionShape *pShape = (btCollisionShape *)pCollide;
	btCollisionObject *pObject = new btCollisionObject;
	pObject->setCollisionShape(pShape);

	// Setup our position
	btVector3 bullPos;
	btMatrix3x3 bullAng;
	ConvertPosToBull(collideOrigin, bullPos);
	ConvertRotationToBull(collideAngles, bullAng);
	btTransform trans(bullAng, bullPos);

	// Compensate for mass offset.
	PhysicsShapeInfo *pInfo = (PhysicsShapeInfo *)pShape->getUserPointer();
	if (pInfo)
		trans *= btTransform(btMatrix3x3::getIdentity(), pInfo->massCenter);

	pObject->setWorldTransform(trans);

	// Do our raytrace.

	// Cleanup
	delete pObject;

	NOT_IMPLEMENTED
	return collideOrigin;
}

// Called every frame when an object is reported to be asleep in CPhysicsObject::IsAsleep!
void CPhysicsCollision::CollideGetAABB(Vector *pMins, Vector *pMaxs, const CPhysCollide *pCollide, const Vector &collideOrigin, const QAngle &collideAngles) {
	VPROF_BUDGET("CPhysicsCollision::CollideGetAABB", VPROF_BUDGETGROUP_PHYSICS);
	if (!pCollide || (!pMins && !pMaxs)) return;

	// Bullet returns very different AABBs than Havok.
	btCollisionShape *shape = (btCollisionShape *)pCollide;

	btVector3 pos, mins, maxs;
	btMatrix3x3 rot;

	ConvertPosToBull(collideOrigin, pos);
	ConvertRotationToBull(collideAngles, rot);
	btTransform transform(rot, pos);

	PhysicsShapeInfo *shapeInfo = (PhysicsShapeInfo *)shape->getUserPointer();
	if (shapeInfo)
		transform *= btTransform(btMatrix3x3::getIdentity(), shapeInfo->massCenter);

	shape->getAabb(transform, mins, maxs);

	if (pMins)
		ConvertPosToHL(mins, *pMins);

	if (pMaxs)
		ConvertPosToHL(maxs, *pMaxs);
}

void CPhysicsCollision::CollideGetMassCenter(CPhysCollide *pCollide, Vector *pOutMassCenter) {
	if (!pCollide || !pOutMassCenter) return;

	btCollisionShape *pShape = (btCollisionShape *)pCollide;
	PhysicsShapeInfo *pInfo = (PhysicsShapeInfo *)pShape->getUserPointer();

	if (pInfo) {
		Vector massCenter;
		ConvertPosToHL(pInfo->massCenter, massCenter);

		*pOutMassCenter = massCenter;
	}
}

void CPhysicsCollision::CollideSetMassCenter(CPhysCollide *pCollide, const Vector &massCenter) {
	if (!pCollide) return;

	btCollisionShape *pShape = (btCollisionShape *)pCollide;
	PhysicsShapeInfo *pInfo = (PhysicsShapeInfo *)pShape->getUserPointer();

	if (pInfo) {
		btVector3 bullMassCenter;
		ConvertPosToBull(massCenter, bullMassCenter);

		pInfo->massCenter = bullMassCenter;
	}
}

Vector CPhysicsCollision::CollideGetOrthographicAreas(const CPhysCollide *pCollide) {
	NOT_IMPLEMENTED
	return Vector(0, 0, 0);
}

void CPhysicsCollision::CollideSetOrthographicAreas(CPhysCollide *pCollide, const Vector &areas) {
	NOT_IMPLEMENTED
}

int CPhysicsCollision::CollideIndex(const CPhysCollide *pCollide) {
	NOT_IMPLEMENTED
	return 0;
}

int CPhysicsCollision::GetConvexesUsedInCollideable(const CPhysCollide *pCollideable, CPhysConvex **pOutputArray, int iOutputArrayLimit) {
	const btCollisionShape *pShape = (btCollisionShape *)pCollideable;
	if (!pShape->isCompound()) return 0;

	const btCompoundShape *pCompound = (btCompoundShape *)pShape;
	int numSolids = pCompound->getNumChildShapes();
	for (int i = 0; i < numSolids && i < iOutputArrayLimit; i++) {
		const btCollisionShape *pConvex = pCompound->getChildShape(i);
		pOutputArray[i] = (CPhysConvex *)pConvex;
	}

	return numSolids > iOutputArrayLimit ? iOutputArrayLimit : numSolids;
}

CPhysConvex *CPhysicsCollision::BBoxToConvex(const Vector &mins, const Vector &maxs) {
	if (mins == maxs) return NULL;

	btVector3 btmins, btmaxs;
	ConvertPosToBull(mins, btmins);
	ConvertPosToBull(maxs, btmaxs);
	btVector3 halfSize = (btmaxs - btmins) / 2;
	btBoxShape *box = new btBoxShape(halfSize.absolute());

	return (CPhysConvex *)box;
}

CPhysCollide *CPhysicsCollision::BBoxToCollide(const Vector &mins, const Vector &maxs) {
	CPhysConvex *pConvex = BBoxToConvex(mins, maxs);
	if (!pConvex) return NULL;

	btCompoundShape *pCompound = new btCompoundShape;
	pCompound->setMargin(COLLISION_MARGIN);

	btVector3 btmins, btmaxs;
	ConvertPosToBull(mins, btmins);
	ConvertPosToBull(maxs, btmaxs);
	btVector3 halfsize = (btmaxs - btmins) / 2;

	pCompound->addChildShape(btTransform(btMatrix3x3::getIdentity(), btmins + halfsize), (btCollisionShape *)pConvex);
	return (CPhysCollide *)pCompound;
}

CPhysConvex *CPhysicsCollision::CylinderToConvex(const Vector &mins, const Vector &maxs) {
	if (mins == maxs) return NULL;

	btVector3 btmins, btmaxs;
	ConvertPosToBull(mins, btmins);
	ConvertPosToBull(maxs, btmaxs);
	btVector3 halfSize = (btmaxs - btmins) / 2;

	btCylinderShape *pShape = new btCylinderShape(halfSize.absolute());

	return (CPhysConvex *)pShape;
}

CPhysCollide *CPhysicsCollision::CylinderToCollide(const Vector &mins, const Vector &maxs) {
	CPhysConvex *pConvex = CylinderToConvex(mins, maxs);
	if (!pConvex) return NULL;

	btCompoundShape *pCompound = new btCompoundShape;
	pCompound->setMargin(COLLISION_MARGIN);

	btVector3 btmins, btmaxs;
	ConvertPosToBull(mins, btmins);
	ConvertPosToBull(maxs, btmaxs);
	btVector3 halfsize = (btmaxs - btmins) / 2;

	pCompound->addChildShape(btTransform(btMatrix3x3::getIdentity(), btmins + halfsize), (btCollisionShape *)pConvex);
	return (CPhysCollide *)pCompound;
}

CPhysConvex *CPhysicsCollision::ConeToConvex(const float radius, const float height) {
	btConeShape *pShape = new btConeShape(ConvertDistanceToBull(radius), ConvertDistanceToBull(height));
	return (CPhysConvex *)pShape;
}

CPhysCollide *CPhysicsCollision::ConeToCollide(const float radius, const float height) {
	CPhysConvex *pConvex = ConeToConvex(radius, height);
	if (!pConvex) return NULL;

	btCompoundShape *pCompound = new btCompoundShape;
	pCompound->setMargin(COLLISION_MARGIN);

	// TODO: We need to center the cone shape so that the middle of it is the origin. Find the maths to do it with.
	NOT_IMPLEMENTED

	return (CPhysCollide *)pCompound;
}

CPhysConvex *CPhysicsCollision::SphereToConvex(const float radius) {
	if (radius <= 0) return NULL;

	btSphereShape *pShape = new btSphereShape(ConvertDistanceToBull(radius));
	return (CPhysConvex *)pShape;
}

CPhysCollide *CPhysicsCollision::SphereToCollide(const float radius) {
	CPhysConvex *pConvex = SphereToConvex(radius);
	if (!pConvex) return NULL;

	btCompoundShape *pCompound = new btCompoundShape;
	pCompound->setMargin(COLLISION_MARGIN);

	// TODO: Center it.
	NOT_IMPLEMENTED

	return (CPhysCollide *)pCompound;
}

void CPhysicsCollision::TraceBox(const Vector &start, const Vector &end, const Vector &mins, const Vector &maxs, const CPhysCollide *pCollide, const Vector &collideOrigin, const QAngle &collideAngles, trace_t *ptr) {
	Ray_t ray;
	ray.Init(start, end, mins, maxs);
	return TraceBox(ray, pCollide, collideOrigin, collideAngles, ptr);
}

void CPhysicsCollision::TraceBox(const Ray_t &ray, const CPhysCollide *pCollide, const Vector &collideOrigin, const QAngle &collideAngles, trace_t *ptr) {
	return TraceBox(ray, MASK_ALL, NULL, pCollide, collideOrigin, collideAngles, ptr);
}

// TODO: Use contentsMask
void CPhysicsCollision::TraceBox(const Ray_t &ray, unsigned int contentsMask, IConvexInfo *pConvexInfo, const CPhysCollide *pCollide, const Vector &collideOrigin, const QAngle &collideAngles, trace_t *ptr) {
	VPROF_BUDGET("CPhysicsCollision::TraceBox", VPROF_BUDGETGROUP_PHYSICS);

	btVector3 btvec;
	btMatrix3x3 btmatrix;

	btCollisionObject *object = new btCollisionObject;
	btCollisionShape *shape = (btCollisionShape *)pCollide;
	object->setCollisionShape(shape);

	ConvertPosToBull(collideOrigin, btvec);
	ConvertRotationToBull(collideAngles, btmatrix);
	btTransform transform(btmatrix, btvec);

	PhysicsShapeInfo *shapeInfo = (PhysicsShapeInfo *)shape->getUserPointer();
	if (shapeInfo)
		transform *= btTransform(btMatrix3x3::getIdentity(), shapeInfo->massCenter);

	object->setWorldTransform(transform);

	btVector3 startv, endv;
	ConvertPosToBull(ray.m_Start + ray.m_StartOffset, startv);
	ConvertPosToBull(ray.m_Start + ray.m_StartOffset + ray.m_Delta, endv);

	btTransform startt(btMatrix3x3::getIdentity(), startv);
	btTransform endt(btMatrix3x3::getIdentity(), endv);

	// Single line trace must be supported in TraceBox? Yep, you betcha.
	if (ray.m_IsRay) {
		btCollisionWorld::ClosestRayResultCallback cb(startv, endv);
		btCollisionWorld::rayTestSingle(startt, endt, object, shape, transform, cb);

		ptr->fraction = cb.m_closestHitFraction;
		ConvertPosToHL(cb.m_hitPointWorld, ptr->endpos);
		ConvertDirectionToHL(cb.m_hitNormalWorld, ptr->plane.normal);
	} else {
		ConvertPosToBull(ray.m_Extents, btvec);
		btBoxShape *box = new btBoxShape(btvec.absolute());

		btCollisionWorld::ClosestConvexResultCallback cb(startv, endv);
		btCollisionWorld::objectQuerySingle(box, startt, endt, object, shape, transform, cb, 0.001f);

		ptr->fraction = cb.m_closestHitFraction;
		ConvertPosToHL(cb.m_hitPointWorld, ptr->endpos);
		ConvertDirectionToHL(cb.m_hitNormalWorld, ptr->plane.normal);

		delete box;
	}

	delete object;
}

void CPhysicsCollision::TraceCollide(const Vector &start, const Vector &end, const CPhysCollide *pSweepCollide, const QAngle &sweepAngles, const CPhysCollide *pCollide, const Vector &collideOrigin, const QAngle &collideAngles, trace_t *pTrace) {
	btVector3 bullVec;
	btMatrix3x3 bullMatrix;

	// Create the collision object
	btCollisionObject *object = new btCollisionObject;
	btCollisionShape *shape = (btCollisionShape *)pCollide;
	object->setCollisionShape(shape);
	ConvertRotationToBull(collideAngles, bullMatrix);
	ConvertPosToBull(collideOrigin, bullVec);
	btTransform transform(bullMatrix, bullVec);

	PhysicsShapeInfo *shapeInfo = (PhysicsShapeInfo *)shape->getUserPointer();
	if (shapeInfo)
		transform *= btTransform(btMatrix3x3::getIdentity(), shapeInfo->massCenter);
	object->setWorldTransform(transform);

	// Create the sweep collision object
	btCollisionObject *pSweepObject = new btCollisionObject;
	btCollisionShape *pSweepShape = (btCollisionShape *)pSweepCollide;
	pSweepObject->setCollisionShape(pSweepShape);
	ConvertRotationToBull(sweepAngles, bullMatrix);
	btTransform bullSweepTransform(bullMatrix);

	PhysicsShapeInfo *pSweepShapeInfo = (PhysicsShapeInfo *)pSweepShape->getUserPointer();
	if (pSweepShapeInfo)
		bullSweepTransform *= btTransform(btMatrix3x3::getIdentity(), pSweepShapeInfo->massCenter);
	pSweepObject->setWorldTransform(bullSweepTransform);

	btVector3 bullStartVec, bullEndVec;
	ConvertPosToBull(start, bullStartVec);
	ConvertPosToBull(end, bullEndVec);
	btTransform bullStartT(btMatrix3x3::getIdentity(), bullStartVec);
	btTransform bullEndT(btMatrix3x3::getIdentity(), bullEndVec);

	btCollisionWorld::ClosestConvexResultCallback cb(bullStartVec, bullEndVec);
	// btCollisionWorld::objectQuerySingle(pSweepObject, bullStartT, bullEndT, object, shape, transform, cb, 0);

	// Cleanup
	delete object;
	delete pSweepObject;

	NOT_IMPLEMENTED
}

bool CPhysicsCollision::IsBoxIntersectingCone(const Vector &boxAbsMins, const Vector &boxAbsMaxs, const truncatedcone_t &cone) {
	NOT_IMPLEMENTED
	return false;
}

void GetAllLedges(const ivpcompactledgenode_t *node, CUtlVector<const ivpcompactledge_t *> *vecOut) {
	if (node->IsTerminal()) {
		vecOut->AddToTail(node->GetCompactLedge());
	} else {
		const ivpcompactledgenode_t *rs = node->GetRightSon();
		const ivpcompactledgenode_t *ls = node->GetLeftSon();
		GetAllLedges(rs, vecOut);
		GetAllLedges(ls, vecOut);
	}
}

// Purpose: Loads and converts an ivp mesh to a bullet mesh.
void CPhysicsCollision::VCollideLoad(vcollide_t *pOutput, int solidCount, const char *pBuffer, int bufferSize, bool swap) {
	memset(pOutput, 0, sizeof(*pOutput));
	pOutput->solidCount = solidCount;
	pOutput->solids = new CPhysCollide *[solidCount];

	int position = 0;
	for (int i = 0; i < solidCount; i++) {
		int size = *(int *)(pBuffer + position);
		position += 4; // Skip the size int.

		pOutput->solids[i] = (CPhysCollide *)(pBuffer + position);
		position += size;
	}
	pOutput->pKeyValues = new char[bufferSize - position];
	memcpy(pOutput->pKeyValues, pBuffer + position, bufferSize - position);

	// swap possibly meaning bitswap?
	DevMsg("VPhysics: VCollideLoad with %d solids, swap is %s\n", solidCount, swap ? "true" : "false");

	// Now for the fun part:
	// We must convert all of the ivp shapes into something we can read.
	for (int i = 0; i < solidCount; i++) {
		const char *solid = (const char *)pOutput->solids[i];
		const compactsurfaceheader_t *surfaceheader = (compactsurfaceheader_t *)pOutput->solids[i];
		const ivpcompactsurface_t *ivpsurface = (ivpcompactsurface_t *)((char *)pOutput->solids[i] + sizeof(compactsurfaceheader_t));

		if (surfaceheader->vphysicsID != MAKEID('V', 'P', 'H', 'Y')
				|| surfaceheader->version != 0x100
				|| surfaceheader->modelType != 0x0
				|| ivpsurface->dummy[2] != MAKEID('I', 'V', 'P', 'S')) {
			Warning("VPhysics: Could not load mesh! (bad/unsupported file format)");
			pOutput->solids[i] = NULL;
			continue;
		}

		PhysicsShapeInfo *info = new PhysicsShapeInfo;
		btVector3 massCenter;
		ConvertIVPPosToBull(ivpsurface->mass_center, massCenter);
		info->massCenter = massCenter;

		btCompoundShape *pCompound = new btCompoundShape;
		pCompound->setMargin(COLLISION_MARGIN);
		pCompound->setUserPointer(info);

		// Add all of the ledges up
		CUtlVector<const ivpcompactledge_t *> ledges;
		GetAllLedges((const ivpcompactledgenode_t *)((char *)ivpsurface + ivpsurface->offset_ledgetree_root), &ledges);

		for (int j = 0; j < ledges.Count(); j++) {
			const char *vertices = (const char *)ledges[j] + ledges[j]->c_point_offset;

			btConvexHullShape *pConvex = new btConvexHullShape;
			pConvex->setMargin(COLLISION_MARGIN);

			const char *tris = (const char *)ledges[j] + sizeof(ivpcompactledge_t);
			for (int k = 0; k < ledges[j]->n_triangles; k++) {
				const ivpcompacttriangle_t *ivptri = (ivpcompacttriangle_t *)(tris + (k * sizeof(ivpcompacttriangle_t)));
				Assert(k == ivptri->tri_index);

				for (int l = 0; l < 3; l++) {
					short index = ivptri->c_three_edges[l].start_point_index;
					float *verts = (float *)(vertices + index * 16);

					btVector3 vertex;
					ConvertIVPPosToBull(verts, vertex);
					pConvex->addPoint(vertex);
				}
			}

			pCompound->addChildShape(btTransform(btMatrix3x3::getIdentity(), -info->massCenter), pConvex);
		}

		pOutput->solids[i] = (CPhysCollide *)pCompound;
	}
}

void CPhysicsCollision::VCollideUnload(vcollide_t *pVCollide) {
	for (int i = 0; i < pVCollide->solidCount; i++) {
		DestroyCollide(pVCollide->solids[i]);
	}

	delete [] pVCollide->solids;
	pVCollide->solids = NULL;

	delete pVCollide->pKeyValues;
	pVCollide->pKeyValues = NULL;
}

IVPhysicsKeyParser *CPhysicsCollision::VPhysicsKeyParserCreate(const char *pKeyData) {
	return new CPhysicsKeyParser(pKeyData);
}

void CPhysicsCollision::VPhysicsKeyParserDestroy(IVPhysicsKeyParser *pParser) {
	delete (CPhysicsKeyParser *)pParser;
}

int CPhysicsCollision::CreateDebugMesh(CPhysCollide const *pCollisionModel, Vector **outVerts) {
	if (!pCollisionModel || !outVerts) return 0;

	int count = 0;
	btCompoundShape *compound = (btCompoundShape *)pCollisionModel;
	for (int i = 0; i < compound->getNumChildShapes(); i++)
		count += ((btConvexHullShape *)compound->getChildShape(i))->getNumVertices();
	
	*outVerts = new Vector[count];
	int k = 0;

	// BUG: This doesn't work on concave meshes! (Lots of lines connecting far apart vertices)
	for (int i = 0; i < compound->getNumChildShapes(); i++) {
		btConvexHullShape *hull = (btConvexHullShape *)compound->getChildShape(i);
		for (int j = hull->getNumVertices()-1; j >= 0; j--) { // ugh, source wants the vertices in this order or shit begins to draw improperly
			btVector3 pos;
			hull->getVertex(j, pos);
			ConvertPosToHL(pos, (*outVerts)[k++]);
		}
	}

	return count;
}

void CPhysicsCollision::DestroyDebugMesh(int vertCount, Vector *outVerts) {
	delete [] outVerts;
}

ICollisionQuery *CPhysicsCollision::CreateQueryModel(CPhysCollide *pCollide) {
	return new CCollisionQuery((btCollisionShape *)pCollide);
}

void CPhysicsCollision::DestroyQueryModel(ICollisionQuery *pQuery) {
	delete (CCollisionQuery *)pQuery;
}

IPhysicsCollision *CPhysicsCollision::ThreadContextCreate() {
	return new CPhysicsCollision;
}

void CPhysicsCollision::ThreadContextDestroy(IPhysicsCollision *pThreadContext) {
	delete (CPhysicsCollision *)pThreadContext;
}

CPhysCollide *CPhysicsCollision::CreateVirtualMesh(const virtualmeshparams_t &params) {
	IVirtualMeshEvent *handler = params.pMeshEventHandler;

	virtualmeshlist_t list;
	handler->GetVirtualMesh(params.userData, &list);

	// FIXME: MEMORY LEAK - Find out where to delete this.
	btTriangleMesh *btmesh = new btTriangleMesh;
	btVector3 btvec[3];
	for (int i = 0; i < list.triangleCount; i++) {
		ConvertPosToBull(list.pVerts[list.indices[i*3+0]], btvec[0]);
		ConvertPosToBull(list.pVerts[list.indices[i*3+1]], btvec[1]);
		ConvertPosToBull(list.pVerts[list.indices[i*3+2]], btvec[2]);
		btmesh->addTriangle(btvec[0], btvec[1], btvec[2], true);
	}

	// FIXME: MEMORY LEAK - Find out where to delete this.
	btBvhTriangleMeshShape *bull = new btBvhTriangleMeshShape(btmesh, true);
	bull->setMargin(COLLISION_MARGIN);
	return (CPhysCollide *)bull;
}

// Function only called once in server.dll - seems pretty pointless for valve to make this function.
bool CPhysicsCollision::SupportsVirtualMesh() {
	return true;
}

bool CPhysicsCollision::GetBBoxCacheSize(int *pCachedSize, int *pCachedCount) {
	NOT_IMPLEMENTED
	return false;
}

void CPhysicsCollision::OutputDebugInfo(const CPhysCollide *pCollide) {
	btCollisionShape *pShape = (btCollisionShape *)pCollide;

	Msg("Type: %s\n", pShape->getName());
	if (pShape->isCompound()) {
		btCompoundShape *pCompound = (btCompoundShape *)pShape;
		Msg("Num child shapes: %d\n", pCompound->getNumChildShapes());

		Msg("-- CHILD SHAPES:\n");
		for (int i = 0; i < pCompound->getNumChildShapes(); i++) {
			OutputDebugInfo((CPhysCollide *)pCompound->getChildShape(i));
		}
		Msg("---\n");
	} else if (pShape->isConvex()) {
		// FIXME: Spheres don't work with this!
		//btConvexHullShape *pConvex = (btConvexHullShape *)pShape;
		//Msg("Num vertices: %d\n", pConvex->getNumVertices());
	}
}

unsigned int CPhysicsCollision::ReadStat(int statID) {
	NOT_IMPLEMENTED
	return 0;
}

CPhysicsCollision g_PhysicsCollision;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CPhysicsCollision, IPhysicsCollision, VPHYSICS_COLLISION_INTERFACE_VERSION, g_PhysicsCollision);