#include "Transform.h"

#include <cmath>

using namespace BoyLib;

Transform::Transform()
{
    reset();
}

void Transform::reset()
{
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            mMatrix[i][j] = 0.0f;
        }
    }
    
    mMatrix[0][0] = 1.0f;
    mMatrix[1][1] = 1.0f;
    mMatrix[2][2] = 1.0f;
}

Transform Transform::clone() const
{
    Transform output;
    output.mMatrix = mMatrix;
    return output;
}

Transform Transform::multiply(const Transform &b) const
{
    Transform result;

    for (int i = 0; i < 3; ++i) { 
        for (int j = 0; j < 3; ++j) { 
            float sum = 0.0f;
            for (int k = 0; k < 3; ++k) {
                sum += mMatrix[i][k] * b.mMatrix[k][j];
            }
            result.mMatrix[i][j] = sum;
        }
    }
    
    return result;
}

void Transform::set(float a, float b, float c, float d, float e, float f)
{
    reset();
    mMatrix[0][0] = a;
    mMatrix[1][0] = b;
    mMatrix[0][1] = c;
    mMatrix[1][1] = d;
    mMatrix[0][2] = e;
    mMatrix[1][2] = f;
}

Vector2 Transform::applyTransform(float x, float y) const
{
    float ox = mMatrix[0][0]*x + mMatrix[0][1]*y + mMatrix[0][2];
    float oy = mMatrix[1][0]*x + mMatrix[1][1]*y + mMatrix[1][2];

    return Vector2(ox, oy);
}

void Transform::translate(float x, float y)
{
    Transform T;

    T.mMatrix[0][2] = x;
    T.mMatrix[1][2] = y;

    mMatrix = multiply(T).mMatrix;
}

void Transform::preTranslate(float x, float y)
{
    Transform a;
    a.translate(x, y);
    mMatrix = a.multiply(*this).mMatrix;
}

void Transform::scale(float x, float y)
{
    Transform T;

    T.mMatrix[0][0] = x;
    T.mMatrix[1][1] = y;

    mMatrix = multiply(T).mMatrix;
}

void Transform::preScale(float x, float y)
{
    Transform a;
    a.scale(x, y);
    mMatrix = a.multiply(*this).mMatrix;
}

void Transform::rotate(float radians)
{
    float c = std::cos(-radians);
    float s = std::sin(-radians);

    Transform T;

    T.mMatrix[0][0] = c;
    T.mMatrix[0][1] = -s;
    T.mMatrix[1][0] = s;
    T.mMatrix[1][1] = c;

    mMatrix = multiply(T).mMatrix;
}

void Transform::rotate(float radians, const Vector2 &vec)
{
    translate(vec.x, vec.y);
    rotate(radians);
    translate(-vec.x, -vec.y);
}

void Transform::preRotate(float radians)
{
    Transform a;
    a.rotate(radians);
    mMatrix = a.multiply(*this).mMatrix;
}

void Transform::preRotate(float radians, const Vector2 &vec)
{
    Transform a;
    a.rotate(radians, vec);
    mMatrix = a.multiply(*this).mMatrix;
}