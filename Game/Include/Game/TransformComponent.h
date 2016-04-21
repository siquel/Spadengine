#pragma once
#include "Game/Component.h"
#include "Core/Math.h"

namespace sge
{

	class TransformComponent : public Component
	{
	public:
		TransformComponent() : Component(nullptr) {};
		TransformComponent(Entity* ent);
		TransformComponent(Entity* ent, const sge::math::vec3& position, const sge::math::vec3& scale, float rotation);
		~TransformComponent();

		// TODO do we need update??????
		void update() {}

		void setPosition(const math::vec3& p)
		{
			position = p;
		}

        void addPosition(const math::vec3& p)
        {
            position += p;
        }

        void addAngle(float a)
        {
            angle += a;
        }

		void setScale(const math::vec3& s)
		{
			scale = s;
		}

		void setRotationVector(const math::vec3& rv)
		{
			rotationVector = rv;
		}

		void setAngle(float a)
		{
			angle = a;
		}

		const math::vec3& getPosition()
		{
			return position;
		}

		const math::vec3& getScale()
		{
			return scale;
		}

		const math::vec3& getRotationVector()
		{
			return rotationVector;
		}

		float getAngle()
		{
			return angle;
		}

		const math::mat4 getMatrix()
		{
			return
				math::translate(math::mat4(1.0f), position) *
				math::rotate(math::mat4(1.0f), angle, rotationVector) *
				math::scale(math::mat4(1.0f), scale);
		}

	private:
		math::vec3 position;
		math::vec3 scale;
		math::vec3 rotationVector;
		float angle;
	};

}