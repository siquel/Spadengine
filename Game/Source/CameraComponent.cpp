#include "Game/CameraComponent.h"
#include "Game/TransformComponent.h"
#include "Game/Entity.h"
#include "Core/Assert.h"

#include <SDL2/SDL.h>
#include <iostream>

namespace sge
{
	CameraComponent::CameraComponent(Entity* ent) : 
		Component(ent)
	{
		transform = getParent()->getComponent<TransformComponent>();

		SGE_ASSERT(transform);
	}

	CameraComponent::~CameraComponent()
	{

	}

	void CameraComponent::setupCamera(float FOV, float aspectRatio, float near, float far, bool enableMouse, sge::math::vec3 POS, sge::math::vec3 front, sge::math::vec3 up)
	{
		this->transform->setPosition(POS);
		this->cameraFront = front;
		this->transform->setRotationVector(cameraFront);
		this->cameraUp = up;

		P = sge::math::perspective(sge::math::radians(FOV), aspectRatio, near, far);
		V = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		VP = P * V;
		mouseEnabled = enableMouse;

		if (mouseEnabled)
		{
			SDL_SetRelativeMouseMode(SDL_TRUE);
		}

		sensitivity = 0.15f;
	}

	void CameraComponent::update()
	{
		if (mouseEnabled)
		{
			//Spade::getInstance().mouseInput->getRelativeMouseState(&mouseXpos, &mouseYpos);

			mousseX += mouseXpos;
			mousseY += mouseYpos;

			float xoffset = mousseX - lastX;
			float yoffset = lastY - mousseY;
			lastX = static_cast<float>(mousseX);
			lastY = static_cast<float>(mousseY);

			xoffset *= sensitivity;
			yoffset *= sensitivity;

			yaw += xoffset;
			pitch += yoffset;

			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;

			sge::math::vec3 front;
			front.x = sge::math::cos(sge::math::radians(pitch)) * sge::math::cos(sge::math::radians(yaw));
			front.y = sge::math::sin(sge::math::radians(pitch));
			front.z = sge::math::cos(sge::math::radians(pitch)) * sge::math::sin(sge::math::radians(yaw));
			cameraFront = sge::math::normalize(front);
		}
		transform->setRotationVector(cameraFront);

		setPosition(transform->getPosition());
		if (!mouseEnabled)
		{
			setFrontByVector(transform->getRotationVector());
		}		

		V = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		VP = P * V;
	}

	void CameraComponent::render(GraphicsDevice* device)
	{

	}

	sge::math::mat4 CameraComponent::getVp()
	{
		return VP;
	}

	void CameraComponent::enableMouse()
	{
		SDL_WarpMouseGlobal(enableX, enableY);
		mouseEnabled = true;
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}

	void CameraComponent::disableMouse()
	{
		std::cout << enableX << " " << enableY << std::endl;
		//enableX = Spade::getInstance().mouseInput->getMouseXPosition();
		//enableY = Spade::getInstance().mouseInput->getMouseYPosition();
		mouseEnabled = false;
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}

	void CameraComponent::setFrontByVector(sge::math::vec3 front)
	{
		cameraFront = front;
	}

	void CameraComponent::setPosition(sge::math::vec3 POS)
	{
		cameraPos = POS;
	}

	void CameraComponent::setSensitivity(float sensitivity)
	{
		this->sensitivity = sensitivity;
	}

	sge::math::vec3 CameraComponent::getFront()
	{
		return cameraFront;
	}
}