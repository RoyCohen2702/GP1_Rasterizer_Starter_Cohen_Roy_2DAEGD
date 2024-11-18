//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Maths.h"
#include "Texture.h"
#include "Utils.h"

using namespace dae;

Renderer::Renderer(SDL_Window* pWindow) :
	m_pWindow(pWindow)
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

	//Create Buffers
	m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;

	//m_pDepthBufferPixels = new float[m_Width * m_Height];

	//Initialize Camera
	m_Camera.Initialize(60.f, { .0f,.0f,-10.f });
}

Renderer::~Renderer()
{
	//delete[] m_pDepthBufferPixels;
}

void Renderer::Update(Timer* pTimer)
{
	m_Camera.Update(pTimer);
}

void Renderer::Render()
{
	//@START
	//Lock BackBuffer
	SDL_LockSurface(m_pBackBuffer);

	//RENDER LOGIC
	std::vector<Vector3> vertices_NDC{
		{	0.f,	0.5f,	1.f},
		{	.5f,	-.5f,	1.f},
		{	-.5f,	-.5f,	1.f}
	};

	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			ColorRGB finalColor{ colors::Black };

			//float gradient = px / static_cast<float>(m_Width);
			//gradient += py / static_cast<float>(m_Width);
			//gradient /= 2.0f;
			
			Vector2 P{ px + 0.5f, py + 0.5f };
			
			for (size_t i = 0; i < vertices_NDC.size(); i+=3)
			{
				Vector3 v0{ vertices_NDC[i]	};
				Vector3 v1{ vertices_NDC[i + 1]	};
				Vector3 v2{ vertices_NDC[i + 2]	};
			
			
				Vector2 A{ ((v0.x + 1) / 2) * m_Width, ((1 - v0.y) / 2) * m_Height };
				Vector2 B{ ((v1.x + 1) / 2) * m_Width, ((1 - v1.y) / 2) * m_Height };
				Vector2 C{ ((v2.x + 1) / 2) * m_Width, ((1 - v2.y) / 2) * m_Height };
			
				Vector2 edge0 =  B - A ;
				Vector2 edge1 =  C - B ;
				Vector2 edge2 =  A - C ;
			
				Vector2 AP = P - A;
				Vector2 BP = P - B;
				Vector2 CP = P - C; 
			
				if (Vector2::Cross(edge0, AP) >= 0.f &&
					Vector2::Cross(edge1, BP) >= 0.f &&
					Vector2::Cross(edge2, CP) >= 0.f)
				{
			
					finalColor = colors::White;
				}
			}



			//Update Color in Buffer
			finalColor.MaxToOne();

			m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));
		}
	}

	//@END
	//Update SDL Surface
	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

void Renderer::VertexTransformationFunction(const std::vector<Vertex>& vertices_in, std::vector<Vertex>& vertices_out) const
{
	//Todo > W1 Projection Stage


}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}
