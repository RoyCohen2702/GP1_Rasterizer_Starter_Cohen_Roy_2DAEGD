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

	m_DisplayDepthBuffer = false;
	m_RotationAngle = 10.f;
	m_RotationSpeed = 1.f;

	//Initialize Camera
	m_Camera.Initialize(45.f, { .0f,.0f, -50.f });

	//m_pTexture = Texture::LoadFromFile("resources/tuktuk.png");

	m_pVehicleDiffuse	= Texture::LoadFromFile("./Resources/vehicle_diffuse.png");
	m_pVehicleGloss		= Texture::LoadFromFile("./Resources/vehicle_gloss.png");
	m_pVehicleNormal	= Texture::LoadFromFile("./Resources/vehicle_normal.png");
	m_pVehicleSpecular	= Texture::LoadFromFile("./Resources/vehicle_specular.png");
}

Renderer::~Renderer()
{
	delete[] m_pDepthBufferPixels;

	delete	m_pVehicleDiffuse;
	delete	m_pVehicleGloss;
	delete	m_pVehicleNormal;
	delete	m_pVehicleSpecular;

}

void Renderer::Update(Timer* pTimer)
{
	m_Camera.Update(pTimer);
	m_RotationAngle +=  m_RotationSpeed * pTimer->GetElapsed();
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

	//std::vector<Mesh> meshes_World
	//{
	//	Mesh{
	//	// Vertices
	//		{
	//			Vertex{ { -3.f,	 3.f,	-2.f }, {colors::White}, {	0,	0	} },
	//			Vertex{ { 0.f,	 3.f,	-2.f }, {colors::White}, { .5,	0	} },
	//			Vertex{ { 3.f,	 3.f,	-2.f }, {colors::White}, {	1,	0	} },
	//			Vertex{ { -3.f,	 0.f,	-2.f }, {colors::White}, {	0, .5	} },
	//			Vertex{ { 0.f,	 0.f,	-2.f }, {colors::White}, { .5, .5	} },
	//			Vertex{ { 3.f,	 0.f,	-2.f }, {colors::White}, {	1, .5	} },
	//			Vertex{ { -3.f,	-3.f,	-2.f }, {colors::White}, {	0,	1	} },
	//			Vertex{ { 0.f,	-3.f,	-2.f }, {colors::White}, { .5,	1	} },
	//			Vertex{ { 3.f,	-3.f,	-2.f }, {colors::White}, {	1,	1	} }
	//		},
	//
	//	// Indices
	//		{	
	//			3, 0, 1,	1, 4, 3,	4, 1, 2,
	//			2, 5, 4,	6, 3, 4,	4, 7, 6,
	//			7, 4, 5,	5, 8, 7
	//		},	
	//
	//	// Topology
	//		PrimitiveTopology::TriangleList
	//	},
	//
	//	Mesh{
	//	// Vertices
	//		{
	//			Vertex{ { -3.f,	 3.f,	-2.f }, {colors::White} , {	0,	0	} },
	//			Vertex{ { 0.f,	 3.f,	-2.f }, {colors::White} , { .5,	0	} },
	//			Vertex{ { 3.f,	 3.f,	-2.f }, {colors::White} , {	1,	0	} },
	//			Vertex{ { -3.f,	 0.f,	-2.f }, {colors::White} , {	0, .5	} },
	//			Vertex{ { 0.f,	 0.f,	-2.f }, {colors::White} , { .5, .5	} },
	//			Vertex{ { 3.f,	 0.f,	-2.f }, {colors::White} , {	1, .5	} },
	//			Vertex{ { -3.f,	-3.f,	-2.f }, {colors::White} , {	0,	1	} },
	//			Vertex{ { 0.f,	-3.f,	-2.f }, {colors::White} , { .5,	1	} },
	//			Vertex{ { 3.f,	-3.f,	-2.f }, {colors::White} , {	1,	1	} }
	//		},
	//	// Indices
	//		{
	//			3, 0, 4, 1, 5, 2,
	//			2, 6,
	//			6, 3, 7, 4, 8, 5
	//		},
	//	// Topology
	//		PrimitiveTopology::TriangleStrip
	//	}
	//};

	// Mesh Index
	// -----------------
	// 0: TriangleList
	// 1: TriangleStrip
	//int meshIndex{ 0 };


	Mesh meshes_World{};

	Utils::ParseOBJ("resources/vehicle.obj", meshes_World.vertices, meshes_World.indices);

	//PrimitiveTopology topology{ meshes_World.primitiveTopology };
	
	// TODO: Fix this
	PrimitiveTopology topology{ PrimitiveTopology::TriangleList };

	std::vector<uint32_t>& indices{
		meshes_World.indices
	};


	m_Camera.CalculateViewMatrix();
	VertexTransformationFunction(meshes_World.vertices, meshes_World.vertices_out);

	std::vector<Vertex_Out>& vertices_NDC{
		meshes_World.vertices_out
	};
	

	for (size_t i = 0; i < m_Width * m_Height; ++i) {
		m_pDepthBufferPixels[i] = std::numeric_limits<float>::max();
	}

	// Rasterization
	uint16_t size = indices.size() - (topology == PrimitiveTopology::TriangleList ? 0 : 2);

	for (size_t i = 0; i < size; i += (topology == PrimitiveTopology::TriangleList ? 3 : 1))
	{
		// Vertices
		Vector2 v0{ vertices_NDC[indices[i]].position.x,	 vertices_NDC[indices[i]].position.y };
		Vector2 v1{};
		Vector2 v2{};

		// z positions
		float z0 = vertices_NDC[indices[i]].position.z;
		float z1{};
		float z2{};

		// w positions
		float zw0 = vertices_NDC[indices[i]].position.w;
		float zw1{};
		float zw2{};

		if (topology == PrimitiveTopology::TriangleStrip && i % 2 != 0) {
			v1 = { vertices_NDC[indices[i + 2]].position.x, vertices_NDC[indices[i + 2]].position.y };
			v2 = { vertices_NDC[indices[i + 1]].position.x, vertices_NDC[indices[i + 1]].position.y }; 

			z1 = vertices_NDC[indices[i + 2]].position.z;
			z2 = vertices_NDC[indices[i + 1]].position.z;

			zw1 = vertices_NDC[indices[i + 2]].position.w;
			zw2 = vertices_NDC[indices[i + 1]].position.w;
		}
		else {
			v1 = { vertices_NDC[indices[i + 1]].position.x, vertices_NDC[indices[i + 1]].position.y };
			v2 = { vertices_NDC[indices[i + 2]].position.x, vertices_NDC[indices[i + 2]].position.y };

			z1 = vertices_NDC[indices[i + 1]].position.z;
			z2 = vertices_NDC[indices[i + 2]].position.z;

			zw1 = vertices_NDC[indices[i + 1]].position.w;
			zw2 = vertices_NDC[indices[i + 2]].position.w;
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

				// Check if point is inside the triangle
				if (w0 >= 0 && w1 >= 0 && w2 >= 0)
				{
					// zBuffer
					float zBufferValue = 1 / ((w0 / z0) +
						(w1 / z1) +
						(w2 / z2));

					// Interpolated Depth -> using correct depth interpolation with w value
					float interpolatedDepth = 1 / ((w0 / zw0) +
						(w1 / zw1) +
						(w2 / zw2));


					int pixelIndex = py * m_Width + px;

					// Depth check
					if (zBufferValue > 0 && zBufferValue < 1) {
						if (zBufferValue < m_pDepthBufferPixels[pixelIndex])
						{
							if (!m_DisplayDepthBuffer) {
								// Texture
								Vector2 textureColor{};
								if (topology == PrimitiveTopology::TriangleStrip && i % 2 != 0) {
									textureColor = (((vertices_NDC[indices[i]].uv / zw0) * w0) +
										((vertices_NDC[indices[i + 2]].uv / zw1) * w1) +
										((vertices_NDC[indices[i + 1]].uv / zw2) * w2)) * interpolatedDepth;
								}
								else {
									textureColor = (((vertices_NDC[indices[i]].uv / zw0) * w0) +
										((vertices_NDC[indices[i + 1]].uv / zw1) * w1) +
										((vertices_NDC[indices[i + 2]].uv / zw2) * w2)) * interpolatedDepth;
								}

								finalColor += m_pVehicleDiffuse->Sample(textureColor);
							}
							else {
								float depth = Remap(zBufferValue, 0.985f, 1.f, 0.f, 1.f);
								ColorRGB remappedDepth{ depth, depth, depth };
								finalColor += remappedDepth;
							}

							// Depth write
							m_pDepthBufferPixels[pixelIndex] = zBufferValue;

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
	}

	//@END
	//Update SDL Surface
	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

float Renderer::Remap(float value, float low1, float high1, float low2, float high2) const {
	return (value - low1) / (high1 - low1) * (high2 - low2) + low2;
}

void Renderer::VertexTransformationFunction(const std::vector<Vertex>& vertices_in, std::vector<Vertex_Out>& vertices_out) const
{
	vertices_out.resize(vertices_in.size());

	const float FOV{ m_Camera.fov };
	const float aspectRatio{ float(m_Width) / float(m_Height) };

	const float nearPlane = 1.f;
	const float farPlane = 1000.f;

	const float A = farPlane / (farPlane - nearPlane);
	const float B = (-farPlane * nearPlane) / (farPlane - nearPlane);

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

		Matrix worldMatrix{ Matrix::CreateRotationY(m_RotationAngle)};



		// View Space
		Vector4 viewSpace{ m_Camera.viewMatrix.TransformPoint(worldMatrix.TransformPoint(worldSpace)) };

		// Projection Space
		Matrix projectionMatrix{
			{ 1 / (aspectRatio * FOV), 0, 0, 0 },
			{ 0, 1 / FOV, 0, 0 },
			{ 0, 0, A, 1 },
			{ 0, 0, B, 0}
		};

		Vector4 projected{ projectionMatrix.TransformPoint(viewSpace) };

		//Matrix worldViewProjectionMatrix = projectionMatrix * m_Camera.viewMatrix * worldMatrix;
		//Vector4 projected{ worldViewProjectionMatrix.TransformPoint(worldSpace) };
		
		// Projection divide
		projected.x /= projected.w;
		projected.y /= projected.w;
		projected.z /= projected.w;
		projected.w = projected.w;

		//std::cout << "x: " << projected.x << "\t y: " << projected.y << "\t z:" << projected.z << "\t w:" << projected.w << std::endl;
		vertices_out[i].position = projected;
		vertices_out[i].color = vertices_in[i].color;
		vertices_out[i].uv = vertices_in[i].uv;
	}
}

void dae::Renderer::ToggleDepthBufferAndFinalColor()
{
	m_DisplayDepthBuffer = !m_DisplayDepthBuffer;
}

void dae::Renderer::ToggleRotation()
{
	if (m_RotationSpeed == 0.f)
		m_RotationSpeed = 1.f;
	else
		m_RotationSpeed = 0.f;
}

void dae::Renderer::ToggleNormalMap()
{
}

void dae::Renderer::CycleShadingMode()
{

}

void dae::Renderer::PixelShading(const std::vector<Vertex_Out>& v)
{
	Vector3 lightDirection = { .577f, .577f, .577f };
	float lightIntensity = 7.f;
	float shininess = 25.f;
	Vector3 AmbientColor = { .25f, .25f, .25f };
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}
