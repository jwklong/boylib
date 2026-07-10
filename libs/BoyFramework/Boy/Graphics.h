#pragma once

#include "BoyLib/Rect.h"
#include "BoyLib/Transform.h"
#include "BoyLib/Vector2.h"
#include "BoyLib/UString.h"

#include <stack>

namespace Boy
{
    typedef unsigned long Color;
	struct BoyVertex 
	{
		float x, y, z;
		Color color;
		float u, v;
	};

	class Image;
	class Font;
	class TriStrip;

	class Graphics
	{
	public:

		enum CompareFunc
		{
			CMP_NEVER,
			CMP_LESS,
			CMP_EQUAL,
			CMP_LEQUAL,
			CMP_GREATER,
			CMP_NOTEQUAL,
			CMP_GEQUAL,
			CMP_ALWAYS
		};

		enum DrawMode
		{
			DRAWMODE_NORMAL,
			DRAWMODE_ADDITIVE
		};

		Graphics(int width, int height);
		~Graphics();

		void drawImage(Image *img, int subrectX, int subrectY, int subrectW, int subrectH);
		void drawImage(Image *img);
		void drawImage(Image *img, const BoyLib::Rect &subrect) { drawImage(img, (int)subrect.getX(), (int)subrect.getY(), (int)subrect.getWidth(), (int)subrect.getHeight()); }

        void drawLine(int x0, int y0, int x1, int y1);
		void fillRect(int x0, int y0, int w, int h);
		void drawTriStrip(TriStrip *strip);

		void drawString(Font *font, const BoyLib::UString &str, float scale=1);

        void setAlpha(float alpha);
		void setColor(Color color);
        void setDrawMode(DrawMode mode);
		void setColorizationEnabled(bool enabled);

		void setZTestEnabled(bool enabled);
		bool isZTestEnabled();
		void setZWriteEnabled(bool enabled);
		bool isZWriteEnabled();
		void setZFunction(CompareFunc func);
		void setZ(float z);
		float getZ();

		void setAlphaTestEnabled(bool enabled);
		bool isAlphaTestEnabled();
		void setAlphaReferenceValue(int val);
		void setAlphaFunction(CompareFunc func);

		BoyLib::Transform &getTransform() { return mTransform; };
		void pushTransform();
		void popTransform();
		int getTransformStackSize();

		//compat
		void scale(float x, float y) { mTransform.scale(x, y); };
		void rotateDeg(float angle) { mTransform.rotateDegrees(angle); };
		void rotateRad(float angle) { mTransform.rotate(angle); };
		void translate(float x, float y) { mTransform.translate(x, y); };
		void setIdentity() { mTransform.reset(); };
		void preScale(float x, float y) { mTransform.preScale(x, y); };
		void preRotateDeg(float angle) { mTransform.preRotateDegrees(angle); };
		void preRotateRad(float angle) { mTransform.preRotate(angle); };
		void preTranslate(float x, float y) { mTransform.preTranslate(x, y); };

		void setClearZ(float z);
        void setClearColor(Color color);

		void setClipRect(int x, int y, int width, int height);

		int getWidth();
		int getHeight();

	private:
		int mWidth;
        int mHeight;

        Color mColor;
		bool mColorizationEnabled;

		float mZ;

		BoyLib::Transform mTransform;
		std::stack<BoyLib::Transform> mTransformStack;

	};
};
