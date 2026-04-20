#pragma once

#include <array>
#include <stdio.h>
#include "Vector2.h"
#include "BoyUtil.h"

namespace BoyLib
{
    class Transform
    {
    public:
        Transform();

        void reset();
        Transform clone() const;

        Transform multiply(const Transform &b) const;
        Transform preMultiply(const Transform &b) const { return b.multiply(*this); };
		Transform operator*(const Transform &b) const { return multiply(b); };
		void operator*=(const Transform &b) { mMatrix = multiply(b).mMatrix; };

        void set(float a, float b, float c, float d, float e, float f);
        void set(const Transform &t) { mMatrix = t.mMatrix; };
		void operator=(const Transform &t) { mMatrix = t.mMatrix; };

        Vector2 applyTransform(float x, float y) const;
        Vector2 applyTransform(const Vector2 &vec) const { return applyTransform(vec.x, vec.y); };

        void translate(float x, float y);
        void translate(const Vector2 &vec) { translate(vec.x, vec.y); };

        void preTranslate(float x, float y);
        void preTranslate(const Vector2 &vec) { preTranslate(vec.x, vec.y); };

        void scale(float x, float y);
        void scale(float scalar) { scale(scalar, scalar); };
        void scale(const Vector2 &vec) { scale(vec.x, vec.y); };

        void preScale(float x, float y);
        void preScale(float scalar) { preScale(scalar, scalar); };
        void preScale(const Vector2 &vec) { preScale(vec.x, vec.y); };

        void rotate(float radians);
        void rotate(float radians, const Vector2 &vec);
        void rotateDegrees(float degrees) { rotate(deg2rad(degrees)); };
        void rotateDegrees(float degrees, const Vector2 &vec) { rotate(deg2rad(degrees), vec); };

        void preRotate(float radians);
        void preRotate(float radians, const Vector2 &vec);
        void preRotateDegrees(float degrees) { preRotate(deg2rad(degrees)); };
        void preRotateDegrees(float degrees, const Vector2 &vec) { preRotate(deg2rad(degrees), vec); };

        void debug() const { printf("(%f,%f,%f,%f,%f,%f)", mMatrix[0][0], mMatrix[1][0], mMatrix[0][1], mMatrix[1][1], mMatrix[0][2], mMatrix[1][2]); };

    private:
        std::array<std::array<float, 3>, 3> mMatrix;
    };
}