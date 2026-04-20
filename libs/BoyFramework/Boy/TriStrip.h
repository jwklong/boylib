#pragma once

#include "Graphics.h"

namespace Boy
{
	class TriStrip
	{
	public:

	    TriStrip(int numVerts);
		~TriStrip();

		void setColor(Color color);

		void setVertPos(int i, float x, float y, float z);
		void setVertTex(int i, float u, float v);
		void setVertColor(int i, Color color);

	public:

		int mVertexCount;
		BoyVertex *mVerts;

	};
};