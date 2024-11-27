#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Maths.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle):
			origin{_origin},
			fovAngle{_fovAngle}
		{
		}


		Vector3 origin{};
		float fovAngle{90.f};
		float fov{ tanf((fovAngle * TO_RADIANS) / 2.f) };

		Vector3 forward{Vector3::UnitZ};
		Vector3 up{Vector3::UnitY};
		Vector3 right{Vector3::UnitX};

		float totalPitch{};
		float totalYaw{};

		Matrix invViewMatrix{};
		Matrix viewMatrix{};

		// FOV variables
		const float maxFov{ 150.f };
		const float minFov{ 20.f };
		const float angle{ 5.f };

		// Speed variables
		const float speed{ 4.f };

		// Movement variables
		float movement{ 4.f };
		float mouseMovement{ 0.8f };

		void Initialize(float _fovAngle = 90.f, Vector3 _origin = {0.f,0.f,0.f})
		{
			fovAngle = _fovAngle;
			fov = tanf((fovAngle * TO_RADIANS) / 2.f);

			origin = _origin;
		}

		void CalculateViewMatrix()
		{
			//TODO W1
			//ONB => invViewMatrix
			//Inverse(ONB) => ViewMatrix

			//ViewMatrix => Matrix::CreateLookAtLH(...) [not implemented yet]
			//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixlookatlh
			
			right = Vector3::Cross(Vector3(0, 1, 0), forward).Normalized();
			up = Vector3::Cross(forward, right).Normalized();

			invViewMatrix = {
				right,
				up,
				forward,
				origin
			};

			viewMatrix = invViewMatrix.Inverse();

		}

		void CalculateProjectionMatrix()
		{
			//TODO W3

			//ProjectionMatrix => Matrix::CreatePerspectiveFovLH(...) [not implemented yet]
			//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixperspectivefovlh
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			//Camera Update Logic
			//...

			//Update Matrices
			CalculateViewMatrix();
			CalculateProjectionMatrix(); //Try to optimize this - should only be called once or when fov/aspectRatio changes

			//Matrix
			Matrix finalRotation{};

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);


			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			//todo: W2
			//throw std::runtime_error("Not Implemented Yet");
			if (pKeyboardState[SDL_SCANCODE_LSHIFT]) {
				movement += speed;
			}

			//Change FOV angle with Left and Right arrows
			if (pKeyboardState[SDL_SCANCODE_LEFT]) {
				if (fovAngle > minFov) {
					fovAngle -= angle;
				}
			}
			if (pKeyboardState[SDL_SCANCODE_RIGHT]) {
				if (fovAngle < maxFov) {
					fovAngle += angle;
				}
			}

			if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT) && mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
				if (mouseY > 0) {
					origin -= up * movement * deltaTime;
				}
				else if (mouseY < 0) {
					origin += up * movement * deltaTime;
				}
			}
			else if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) {
				if (mouseY != 0 || mouseX != 0) {
					if (mouseY > 0) {
						origin -= forward * speed * deltaTime;
					}
					else if (mouseY < 0) {
						origin += forward * speed * deltaTime;
					}
					totalYaw += mouseX * deltaTime;
				}
			}
			else if (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
				if (mouseX != 0 || mouseY != 0) {
					totalPitch += mouseY * deltaTime;
					totalYaw += mouseX * deltaTime;
				}
			}

			finalRotation = Matrix::CreateRotationX(totalPitch) * Matrix::CreateRotationY(totalYaw);
			forward = finalRotation.TransformVector(Vector3::UnitZ);
			forward.Normalize();

			//Change position of camera with WASD
			// 
			// Dont move origin.z by itself because if you move look around and try to move around
			// the camera will only follow the z-axis and not the way the camera is pointing

			if (pKeyboardState[SDL_SCANCODE_W]) {
				origin += forward * speed * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_S]) {
				origin -= forward * speed * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_A]) {
				Vector3 right = Vector3::Cross(forward, up);
				origin += right * speed * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_D]) {
				Vector3 right = Vector3::Cross(forward, up);
				origin -= right * speed * deltaTime;
			}
		}
	};
}
