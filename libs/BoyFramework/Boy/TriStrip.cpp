#include "TriStrip.h"

#include <cstring>

using namespace Boy;

TriStrip::TriStrip(int numVerts)
{
    mVertexCount = numVerts;
    mVerts = new BoyVertex[numVerts];
	std::memset(mVerts, 0, numVerts*sizeof(BoyVertex));
}

TriStrip::~TriStrip()
{
    delete[] mVerts;
}

void TriStrip::setColor(Color color)
{
    for (int i = 0; i < mVertexCount; i++)
    {
        mVerts[i].color = color;
    }
}

void TriStrip::setVertPos(int i, float x, float y, float z)
{
    mVerts[i].x = x;
    mVerts[i].y = y;
    mVerts[i].z = z;
}

void TriStrip::setVertTex(int i, float u, float v)
{
    mVerts[i].u = u;
    mVerts[i].v = v;
}

void TriStrip::setVertColor(int i, Color color)
{
    mVerts[i].color = color;
}