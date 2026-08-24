#include "Graphics.h"

#include "GL/gl.h"
#include <iostream>
#include <algorithm>
#include "Environment.h"
#include "Image.h"
#include "Font.h"
#include "BoyLib/Vector2.h"
#include "TriStrip.h"

using namespace Boy;

Graphics::Graphics(int width, int height)
{
    mWidth = width;
    mHeight = height;
    Graphics::setColor(0x00000000);
	mColorizationEnabled = false;
    mZ = 0;
}

Graphics::~Graphics()
{
	while (mTransformStack.size()>0)
	{
		mTransformStack.pop();
	}
}

void Graphics::drawImage(Image *img, int subrectX, int subrectY, int subrectW, int subrectH)
{
    if (img == NULL)
    {
        envDebugLog("WARNING: trying to draw image with NULL texture");
        return;
    }

	GLuint textureID = img->getTextureId();
    glBindTexture(GL_TEXTURE_2D, textureID); // bind

    float halfW = (float)subrectW / 2.0f;
    float halfH = (float)subrectH / 2.0f;

    BoyLib::Vector2 v0 = mTransform.applyTransform(-halfW, -halfH); // top left
    BoyLib::Vector2 v1 = mTransform.applyTransform(halfW, -halfH);  // top right
    BoyLib::Vector2 v2 = mTransform.applyTransform(halfW, halfH);   // bottom right
    BoyLib::Vector2 v3 = mTransform.applyTransform(-halfW, halfH);  // bottom left

    GLfloat vertices[] = {
        v0.x, v0.y, mZ, // top left
        v1.x, v1.y, mZ, // top right
        v2.x, v2.y, mZ, // bottom right
        v3.x, v3.y, mZ  // bottom left
    };

    GLfloat tX = (float)subrectX / img->getWidth();
    GLfloat tY = (float)subrectY / img->getHeight();
    GLfloat tW = (float)subrectW / img->getWidth();
    GLfloat tH = (float)subrectH / img->getHeight();
    GLfloat texCoords[] = {
        tX, tY,             // top left
        tX + tW, tY,        // top right
        tX + tW, tY + tH,   // bottom right
        tX, tY + tH         // bottom left
    };

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    if (!mColorizationEnabled) {
        glColor4f(1, 1, 1, 1);
    }

    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
    glDrawArrays(GL_QUADS, 0, 4);

    if (!mColorizationEnabled) {
        setColor(mColor);
    }

    glBindTexture(GL_TEXTURE_2D, 0); // unbind
}

void Graphics::drawImage(Image *img)
{
    drawImage(img, 0, 0, img->getWidth(), img->getHeight());
}

void Graphics::drawLine(int x0, int y0, int x1, int y1)
{
    glBegin(GL_LINES);

    BoyLib::Vector2 v0 = mTransform.applyTransform(x0, y0);
    BoyLib::Vector2 v1 = mTransform.applyTransform(x1, y1);
    glVertex2f(v0.x, v0.y);
    glVertex2f(v0.x, v0.y);

    glEnd();
}

void Graphics::fillRect(int x0, int y0, int w, int h)
{
    glBegin(GL_QUADS);

    BoyLib::Vector2 v0 = mTransform.applyTransform(x0, y0);
    BoyLib::Vector2 v1 = mTransform.applyTransform(x0 + w, y0);
    BoyLib::Vector2 v2 = mTransform.applyTransform(x0 + w, y0 + h);
    BoyLib::Vector2 v3 = mTransform.applyTransform(x0, y0 + h);
    glVertex3f(v0.x, v0.y, mZ);
    glVertex3f(v1.x, v1.y, mZ);
    glVertex3f(v2.x, v2.y, mZ);
    glVertex3f(v3.x, v3.y, mZ);

    glEnd();
}

void Graphics::drawTriStrip(TriStrip *strip)
{
    glBegin(GL_TRIANGLE_STRIP);
    Color oldColor = mColor;
    for (int i = 0; i < strip->mVertexCount; i++) {
        BoyVertex vert = strip->mVerts[i];
        BoyLib::Vector2 v = mTransform.applyTransform(vert.x, vert.y);
        setColor(vert.color);
        glTexCoord2f(vert.u, vert.v);
        glVertex3f(v.x, v.y, vert.z);
    }
    setColor(oldColor);
    glEnd();
}

void Graphics::drawString(Font *font, const BoyLib::UString &str, float scale)
{
    font->drawString(this, str, scale);
}

void Graphics::setAlpha(float alpha)
{
    Graphics::setColor((mColor & 0x00ffffff) | ((int)(255.0f * std::clamp(alpha, 0.0f, 1.0f)) << 24));
}

void Graphics::setColor(Color color)
{
    mColor = color;
    glColor4f(
        (float)((color >> 16) & 0xFF) / 0xFF,
        (float)((color >> 8) & 0xFF) / 0xFF,
        (float)(color & 0xFF) / 0xFF,
        (float)((color >> 24) & 0xFF) / 0xFF
    );
}

void Graphics::setColorizationEnabled(bool enabled)
{
	mColorizationEnabled = enabled;
}

void Graphics::setZTestEnabled(bool enabled)
{
    if (enabled) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}

bool Graphics::isZTestEnabled()
{
    return glIsEnabled(GL_DEPTH_TEST);
}

void Graphics::setZWriteEnabled(bool enabled)
{
    glDepthMask(enabled ? GL_TRUE : GL_FALSE);
}

bool Graphics::isZWriteEnabled()
{
    GLboolean enabled;
    glGetBooleanv(GL_DEPTH_WRITEMASK, &enabled);
    return enabled == GL_TRUE;
}

void Graphics::setZFunction(CompareFunc func)
{
    switch (func) {
        case CMP_NEVER: glDepthFunc(GL_NEVER); break;
        case CMP_LESS: glDepthFunc(GL_LESS); break;
        case CMP_EQUAL: glDepthFunc(GL_EQUAL); break;
        case CMP_LEQUAL: glDepthFunc(GL_LEQUAL); break;
        case CMP_GREATER: glDepthFunc(GL_GREATER); break;
        case CMP_NOTEQUAL: glDepthFunc(GL_NOTEQUAL); break;
        case CMP_GEQUAL: glDepthFunc(GL_GEQUAL); break;
        case CMP_ALWAYS: glDepthFunc(GL_ALWAYS); break;
    }
}

void Graphics::setZ(float z)
{
	assert(z>=0 && z<=1);
	mZ = z;
}

float Graphics::getZ()
{
	return mZ;
}

void Graphics::setAlphaTestEnabled(bool enabled)
{
    if (enabled) {
        glEnable(GL_ALPHA_TEST);
    } else {
        glDisable(GL_ALPHA_TEST);
    }
}

bool Graphics::isAlphaTestEnabled()
{
    return glIsEnabled(GL_ALPHA_TEST);
}

void Graphics::setAlphaReferenceValue(int value)
{
    GLint func;
    glGetIntegerv(GL_ALPHA_TEST_FUNC, &func);
    glAlphaFunc(func, (float)value / 255.0f);
}

void Graphics::setAlphaFunction(CompareFunc func)
{
    GLfloat ref;
    glGetFloatv(GL_ALPHA_TEST_REF, &ref);
    switch (func) {
        case CMP_NEVER: glAlphaFunc(GL_NEVER, ref); break;
        case CMP_LESS: glAlphaFunc(GL_LESS, ref); break;
        case CMP_EQUAL: glAlphaFunc(GL_EQUAL, ref); break;
        case CMP_LEQUAL: glAlphaFunc(GL_LEQUAL, ref); break;
        case CMP_GREATER: glAlphaFunc(GL_GREATER, ref); break;
        case CMP_NOTEQUAL: glAlphaFunc(GL_NOTEQUAL, ref); break;
        case CMP_GEQUAL: glAlphaFunc(GL_GEQUAL, ref); break;
        case CMP_ALWAYS: glAlphaFunc(GL_ALWAYS, ref); break;
    }
}

void Graphics::setDrawMode(DrawMode mode)
{
    switch (mode) {
        case DRAWMODE_NORMAL: glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
        case DRAWMODE_ADDITIVE: glBlendFunc(GL_SRC_ALPHA, GL_ONE); break;
    }
}

void Graphics::setClearZ(float z)
{
    glClearDepth(z);
}

void Graphics::setClearColor(Color color)
{
    glClearColor(
        (float)((color >> 16) & 0xFF) / 0xFF,
        (float)((color >> 8) & 0xFF) / 0xFF,
        (float)(color & 0xFF) / 0xFF,
        (float)((color >> 24) & 0xFF) / 0xFF
    );
}

void Graphics::pushTransform()
{
    mTransformStack.push(mTransform.clone());
}

void Graphics::popTransform()
{
    mTransform.set(mTransformStack.top());
    mTransformStack.pop();
}

int Graphics::getTransformStackSize()
{
	return (int)mTransformStack.size();
}

void Graphics::setClipRect(int x, int y, int width, int height)
{
    glScissor(x, y, width, height);
}

int Graphics::getWidth()
{
    return mWidth;
}

int Graphics::getHeight()
{
    return mHeight;
}