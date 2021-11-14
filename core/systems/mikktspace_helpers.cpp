#include "mikktspace_helpers.h"
#include <vector>

MyMikkTSpaceInferface::MyMikkTSpaceInferface() {
    m_getNumFaces = getNumFaces;
    m_getNumVerticesOfFace = getNumVerticesOfFace;
    m_getPosition = getPosition;
    m_getNormal = getNormal;
    m_getTexCoord = getTexCoord;
    m_setTSpaceBasic = setTSpaceBasic;
    m_setTSpace = nullptr;
}


int getNumFaces(const SMikkTSpaceContext * pContext) {
    std::vector<float>& data = *(std::vector<float> *)pContext->m_pUserData;

    return data.size() / 36;
}

int getNumVerticesOfFace(const SMikkTSpaceContext * pContext, const int iFace) {
    return 3;
}

void getPosition(const SMikkTSpaceContext * pContext, float fvPosOut[], const int iFace, const int iVert) {
    std::vector<float>& data = *(std::vector<float> *)pContext->m_pUserData;

    fvPosOut[0] = data[12 * (iFace * 3 + iVert) + 0];
    fvPosOut[1] = data[12 * (iFace * 3 + iVert) + 1];
    fvPosOut[2] = data[12 * (iFace * 3 + iVert) + 2];
}

void getNormal(const SMikkTSpaceContext * pContext, float fvNormOut[], const int iFace, const int iVert) {
    std::vector<float>& data = *(std::vector<float> *)pContext->m_pUserData;

    fvNormOut[0] = data[12 * (iFace * 3 + iVert) + 5];
    fvNormOut[1] = data[12 * (iFace * 3 + iVert) + 6];
    fvNormOut[2] = data[12 * (iFace * 3 + iVert) + 7];
}

void getTexCoord(const SMikkTSpaceContext * pContext, float fvTexcOut[], const int iFace, const int iVert) {
    std::vector<float>& data = *(std::vector<float> *)pContext->m_pUserData;

    fvTexcOut[0] = data[12 * (iFace * 3 + iVert) + 3];
    fvTexcOut[1] = data[12 * (iFace * 3 + iVert) + 4];
}
void setTSpaceBasic(const SMikkTSpaceContext * pContext, const float fvTangent[], const float fSign, const int iFace, const int iVert) {
    std::vector<float>& data = *(std::vector<float> *)pContext->m_pUserData;

    data[12 * (iFace * 3 + iVert) + 8] = fvTangent[0];
    data[12 * (iFace * 3 + iVert) + 9] = fvTangent[1];
    data[12 * (iFace * 3 + iVert) + 10] = fvTangent[2];
    data[12 * (iFace * 3 + iVert) + 11] = fSign;
}

void setTSpace(const SMikkTSpaceContext * pContext, const float fvTangent[], const float fvBiTangent[], const float fMagS, const float fMagT,
                    const tbool bIsOrientationPreserving, const int iFace, const int iVert) {
    std::vector<float>& data = *(std::vector<float> *)pContext->m_pUserData;

    data[12 * (iFace * 3 + iVert) + 8] = fvTangent[0];
    data[12 * (iFace * 3 + iVert) + 9] = fvTangent[1];
    data[12 * (iFace * 3 + iVert) + 10] = fvTangent[2];
    data[12 * (iFace * 3 + iVert) + 11] = bIsOrientationPreserving ? 1 : -1;
}