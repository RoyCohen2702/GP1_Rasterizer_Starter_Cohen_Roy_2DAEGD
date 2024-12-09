#include "Texture.h"
#include "Vector2.h"
#include <SDL_image.h>

#include <iostream>


namespace dae
{
	Texture::Texture(SDL_Surface* pSurface) :
		m_pSurface{ pSurface },
		m_pSurfacePixels{ (uint32_t*)pSurface->pixels }
	{

	}

	Texture::~Texture()
	{
		if (m_pSurface)
		{
			SDL_FreeSurface(m_pSurface);
			m_pSurface = nullptr;
		}
	}

	Texture* Texture::LoadFromFile(const std::string& path)
	{
		//TODO
		//Load SDL_Surface using IMG_LOAD
		//Create & Return a new Texture Object (using SDL_Surface)

		SDL_Surface* pSurface = IMG_Load(path.c_str());
		if (!pSurface)
		{
			return nullptr;
		}

		Texture* pTexture = new Texture(pSurface);
		return pTexture;
	}

	ColorRGB Texture::Sample(const Vector2& uv) const
	{
		//TODO
		//Sample the correct texel for the given uv

		if (!m_pSurface || !m_pSurfacePixels) {
			return ColorRGB{ 0.0f, 0.0f, 0.0f }; // Return black if surface is not initialized
		}
		
		// Clamp UV coordinates to [0, 1]
		float u = std::clamp(uv.x, 0.0f, 1.0f);
		float v = std::clamp(uv.y, 0.0f, 1.0f);

		// Convert uv to pixel coordinates
		int px = static_cast<int>(u * m_pSurface->w);
		int py = static_cast<int>(v * m_pSurface->h);

		// Compute the 1D pixel index
		uint32_t pixel = m_pSurfacePixels[px + (m_pSurface->w * py)];

		// Extract RGB components
		Uint8 r, g, b;
		SDL_GetRGB(pixel, m_pSurface->format, &r, &g, &b);

		return ColorRGB{ r / 255.0f, g / 255.0f, b / 255.0f };
	}
}