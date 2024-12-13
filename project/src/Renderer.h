#pragma once

#include <cstdint>
#include <vector>

#include "Camera.h"

//#include<Windows.h>
//DWORD nvOptimusEnablement = 1;

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Texture;
	struct Mesh;
	struct Vertex;
	struct Vertex_Out;
	class Timer;
	class Scene;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(Timer* pTimer);
		void Render();

		bool SaveBufferToImage() const;

		void VertexTransformationFunction(const std::vector<Vertex>& vertices_in, std::vector<Vertex_Out>& vertices_out) const;

		void ToggleDepthBufferAndFinalColor();
		void ToggleRotation();
		void ToggleNormalMap();
		void CycleShadingMode();

		void PixelShading(const std::vector<Vertex_Out>& v);
		
	private:
		float Remap(float value, float low1, float high1, float low2, float high2) const;

		SDL_Window* m_pWindow{};

		SDL_Surface* m_pFrontBuffer{ nullptr };
		SDL_Surface* m_pBackBuffer{ nullptr };
		uint32_t* m_pBackBufferPixels{};

		float* m_pDepthBufferPixels{};

		Camera m_Camera{};

		int m_Width{};
		int m_Height{};

		//Texture* m_pTexture{};

		Texture* m_pVehicleDiffuse{};
		Texture* m_pVehicleGloss{};
		Texture* m_pVehicleNormal{};
		Texture* m_pVehicleSpecular{};


		bool m_DisplayDepthBuffer{ };
		float m_RotationAngle{};
		float m_RotationSpeed{};
	};
}
