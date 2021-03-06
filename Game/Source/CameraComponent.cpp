#include "Game/CameraComponent.h"
#include "Game/TransformComponent.h"
#include "Game/Entity.h"
#include "Core/Assert.h"

#include <SDL2/SDL.h>
#include <iostream>

namespace sge
{
	CameraComponent::CameraComponent(Entity* ent) : 
		Component(ent),
        viewport({ 0, 0, 0, 0 }),
        viewProj(0.0f),
        proj(0.0f),
        transform(nullptr)
	{
		transform = getParent()->getComponent<TransformComponent>();

		SGE_ASSERT(transform);
	}

    void CameraComponent::setPerspective(float fov, float aspectRatio, float near, float far)
    {
        proj = math::perspective(math::radians(fov), aspectRatio, near, far);
    }

    void CameraComponent::setOrtho(float left, float right, float bottom, float top, float near, float far)
    {
        proj = math::ortho(left, right, bottom, top, near, far);
    }

    void CameraComponent::setViewport(int x, int y, unsigned int width, unsigned int height)
    {
        viewport.x = x;
        viewport.y = y;
        viewport.width = width;
        viewport.height = height;
    }

    void CameraComponent::setViewport(const Viewport& viewport)
    {
        setViewport(viewport.x, viewport.y, viewport.width, viewport.height);
    }

	void CameraComponent::update()
	{
        updateView();
	}

    void CameraComponent::updateView()
    {
        viewProj = proj * sge::math::lookAt(
            transform->getPosition(), 
            transform->getPosition() + transform->getFront(), 
            transform->getUp());
    }
}