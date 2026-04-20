#pragma once

#include "Resource.h"
#include "GL/gl.h"

namespace Boy
{
	class Image : public Resource
	{
	public:

		Image(ResourceLoader *loader, const std::string &path);
		~Image();

		int getWidth();
		int getHeight();

		void setSize(int width, int height);

		inline GLuint getTextureId() { return mTextureID; }
		void setTextureId(GLuint id) { mTextureID = id; }

		bool init();

	protected:

		GLuint mTextureID;

		int mWidth;
		int mHeight;
		
	};
}