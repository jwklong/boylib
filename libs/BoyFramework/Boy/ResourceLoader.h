#pragma once

#include <string>

namespace Boy
{
	class Image;
	class Sound;

	class ResourceLoader
	{
	public:
		
		ResourceLoader();
		~ResourceLoader();

		bool load(Image *image);
		bool load(Sound *sound);
		Image *createImage(const std::string &filename);
		Sound *createSound(const std::string &filename);

	};
}
