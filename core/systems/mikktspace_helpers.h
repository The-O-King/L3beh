#ifndef MIKKTSPACE_HELPERS
#define MIKKTSPACE_HELPERS

#include "mikktspace.h"



int getNumFaces(const SMikkTSpaceContext * pContext);
int getNumVerticesOfFace(const SMikkTSpaceContext * pContext, const int iFace);
void getPosition(const SMikkTSpaceContext * pContext, float fvPosOut[], const int iFace, const int iVert);
void getNormal(const SMikkTSpaceContext * pContext, float fvNormOut[], const int iFace, const int iVert);
void getTexCoord(const SMikkTSpaceContext * pContext, float fvTexcOut[], const int iFace, const int iVert);
void setTSpaceBasic(const SMikkTSpaceContext * pContext, const float fvTangent[], const float fSign, const int iFace, const int iVert);
void setTSpace(const SMikkTSpaceContext * pContext, const float fvTangent[], const float fvBiTangent[], const float fMagS, const float fMagT,
                    const tbool bIsOrientationPreserving, const int iFace, const int iVert);

struct MyMikkTSpaceInferface : SMikkTSpaceInterface {
    MyMikkTSpaceInferface();
};

#endif