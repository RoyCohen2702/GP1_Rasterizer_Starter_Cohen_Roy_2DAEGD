//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Maths.h"
#include "Texture.h"
#include "Utils.h"


#include <iostream>

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

	m_pDepthBufferPixels = new float[m_Width * m_Height];

	//Initialize Camera
	m_Camera.Initialize(60.f, { .0f,.0f,-10.f });
}

Renderer::~Renderer()
{
	delete[] m_pDepthBufferPixels;
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

	//Clear BackBuffer
	Uint32 clearColor = SDL_MapRGB(m_pBackBuffer->format, 100, 100, 100);
	SDL_FillRect(m_pBackBuffer, nullptr, clearColor);

	//RENDER LOGIC
	//std::vector<Vector3> vertices_NDC{
	//	{	0.0f,	0.5f,	1.f},
	//	{	0.5f,	-.5f,	1.f},
	//	{	-.5f,	-.5f,	1.f}
	//};

	//std::vector<Vertex> vertices_World{
	//	// Triangle 0
	//	{{	 0.f,	2.f,	0.f	}, {	1,	0,	0	}},
	//	{{	 1.5f,	-1.f,	0.f	}, {	1,	0,	0	}},
	//	{{	-1.5f,	-1.f,	0.f	}, {	1,	0,	0	}},
	//
	//	// Triangle 1
	//	{{	 0.f,	4.f,	2.f	}, {	1,	0,	0	}},
	//	{{	 3.f,	-2.f,	2.f	}, {	0,	1,	0	}},
	//	{{	-3.f,	-2.f,	2.f	}, {	0,	0,	1	}}
	//};

	std::vector<Mesh> meshes_World
	{
		Mesh{
		// Vertices
			{
				Vertex{ { -3.f,	 3.f,	-2.f }},
				Vertex{ { 0.f,	 3.f,	-2.f }},
				Vertex{ { 3.f,	 3.f,	-2.f }},
				Vertex{ { -3.f,	 0.f,	-2.f }},
				Vertex{ { 0.f,	 0.f,	-2.f }},
				Vertex{ { 3.f,	 0.f,	-2.f }},
				Vertex{ { -3.f,	-3.f,	-2.f }},
				Vertex{ { 0.f,	-3.f,	-2.f }},
				Vertex{ { 3.f,	-3.f,	-2.f }}
			},
	
		// Indices
			{	
				3, 0, 1,	1, 4, 3,	4, 1, 2,
				2, 5, 4,	6, 3, 4,	4, 7, 6,
				7, 4, 5,	5, 8, 7
			},	
	
		// Topology
			PrimitiveTopology::TriangleList
		},

		Mesh{
		// Vertices
			{
				Vertex{ { -3.f,	 3.f,	-2.f }},
				Vertex{ { 0.f,	 3.f,	-2.f }},
				Vertex{ { 3.f,	 3.f,	-2.f }},
				Vertex{ { -3.f,	 0.f,	-2.f }},
				Vertex{ { 0.f,	 0.f,	-2.f }},
				Vertex{ { 3.f,	 0.f,	-2.f }},
				Vertex{ { -3.f,	-3.f,	-2.f }},
				Vertex{ { 0.f,	-3.f,	-2.f }},
				Vertex{ { 3.f,	-3.f,	-2.f }}
			},
		// Indices
			{
				3, 0, 4, 1, 5, 2,
				2, 6,
				6, 3, 7, 4, 8, 5
			},
		// Topology
			PrimitiveTopology::TriangleStrip
		}
	};

	int meshIndex{ 1 };
	PrimitiveTopology topology{ meshes_World[meshIndex].primitiveTopology};

	std::vector<Vertex>& vertices_World{
		meshes_World[meshIndex].vertices
	};

	std::vector<uint32_t>& indices{
		meshes_World[meshIndex].indices
	};

	std::vector<Vertex> vertices_NDC{};

	m_Camera.CalculateViewMatrix();
	VertexTransformationFunction(vertices_World, vertices_NDC);

	

	for (size_t i = 0; i < m_Width * m_Height; ++i) {
		m_pDepthBufferPixels[i] = std::numeric_limits<float>::max();
	}

	// Rasterization
	uint16_t size = indices.size() - (topology == PrimitiveTopology::TriangleList ? 0 : 2);

	for (size_t i = 0; i < size; i += (topology == PrimitiveTopology::TriangleList ? 3 : 1))
	{
		// Vertices
		Vector2 v0{};
		Vector2 v1{};
		Vector2 v2{};

		if (topology == PrimitiveTopology::TriangleStrip && i % 2 != 0) {
			v0 = { vertices_NDC[indices[i]].position.x,	 vertices_NDC[indices[i]].position.y };
			v1 = { vertices_NDC[indices[i + 2]].position.x, vertices_NDC[indices[i + 2]].position.y };
			v2 = { vertices_NDC[indices[i + 1]].position.x, vertices_NDC[indices[i + 1]].position.y }; 
		}
		else {
			v0 = { vertices_NDC[indices[i]].position.x,	 vertices_NDC[indices[i]].position.y };
			v1 = { vertices_NDC[indices[i + 1]].position.x, vertices_NDC[indices[i + 1]].position.y };
			v2 = { vertices_NDC[indices[i + 2]].position.x, vertices_NDC[indices[i + 2]].position.y };
		}


		// NDC Coordinates
		Vector2 A{ ((v0.x + 1) / 2) * m_Width, ((1 - v0.y) / 2) * m_Height };
		Vector2 B{ ((v1.x + 1) / 2) * m_Width, ((1 - v1.y) / 2) * m_Height };
		Vector2 C{ ((v2.x + 1) / 2) * m_Width, ((1 - v2.y) / 2) * m_Height };

		// Edges
		Vector2 edge0 = B - A;
		Vector2 edge1 = C - B;
		Vector2 edge2 = A - C;

		// Bounding box
		int minX = std::max(0, static_cast<int>(std::floor(std::min({ A.x, B.x, C.x }))));
		int minY = std::max(0, static_cast<int>(std::floor(std::min({ A.y, B.y, C.y }))));

		int maxX = std::min(m_Width - 1, static_cast<int>(std::ceil(std::max({ A.x, B.x, C.x }))));
		int maxY = std::min(m_Height - 1, static_cast<int>(std::ceil(std::max({ A.y, B.y, C.y }))));

		for (int px{ minX }; px < maxX; ++px)
		{
			for (int py{ minY }; py < maxY; ++py)
			{
				ColorRGB finalColor{ colors::Black };
				Vector2 P{ px + 0.5f, py + 0.5f };

				// Direction from NDC to P(ixel Point)
				Vector2 AP = P - A;
				Vector2 BP = P - B;
				Vector2 CP = P - C;

				// Barycentric weights
				float w0 = (Vector2::Cross(edge1, BP));
				float w1 = (Vector2::Cross(edge2, CP));
				float w2 = (Vector2::Cross(edge0, AP));

				// Total Triangle Area 
				float total = w0 + w1 + w2;

				w0 /= total;
				w1 /= total;
				w2 /= total;


				if (w0 >= 0 && w1 >= 0 && w2 >= 0)
				{
					// Interpolate Depth
					float depth = w0 * vertices_NDC[indices[i]].position.z +
						w1 * vertices_NDC[indices[i + 1]].position.z +
						w2 * vertices_NDC[indices[i + 2]].position.z;

					int pixelIndex = py * m_Width + px;

					// Depth check
					if (depth < m_pDepthBufferPixels[pixelIndex])
					{
						if (topology == PrimitiveTopology::TriangleStrip && i % 2 != 0) {
							finalColor += vertices_NDC[indices[i]].color * w0 + vertices_NDC[indices[i + 2]].color * w1 + vertices_NDC[indices[i + 1]].color * w2;
						}
						else {
							finalColor += vertices_NDC[indices[i]].color * w0 + vertices_NDC[indices[i + 1]].color * w1 + vertices_NDC[indices[i + 2]].color * w2;
						}

						// Depth write
						m_pDepthBufferPixels[pixelIndex] = depth;

						//Update Color in Buffer
						finalColor.MaxToOne();

						m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
							static_cast<uint8_t>(finalColor.r * 255),
							static_cast<uint8_t>(finalColor.g * 255),
							static_cast<uint8_t>(finalColor.b * 255));
					}
				}
			}

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
	vertices_out.resize(vertices_in.size());

	const float FOV{ m_Camera.fov };
	const float aspectRatio{ float(m_Width) / float(m_Height) };

	//Todo > W1 Projection Stage
	for (size_t i = 0; i < vertices_in.size(); ++i)
	{
		// World Space
		Vector4 worldSpace{
			vertices_in[i].position.x,
			vertices_in[i].position.y,
			vertices_in[i].position.z,
			1.f
		};

		// View Space
		Vector4 viewSpace{ m_Camera.viewMatrix.TransformPoint(worldSpace) };


		const float nearPlane = 0.1f;
		const float farPlane = 100.0f;

		// Projection Space
		Vector4 projected{
			viewSpace.x / (aspectRatio * FOV),
			viewSpace.y / FOV,
			(viewSpace.z - nearPlane) / (farPlane - nearPlane),
			viewSpace.z
		};

		// Projection divide 
		projected.x /= projected.w;
		projected.y /= projected.w;
		projected.z /= projected.w;

		vertices_out[i].position = projected;
		vertices_out[i].color = vertices_in[i].color;
	}
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}
