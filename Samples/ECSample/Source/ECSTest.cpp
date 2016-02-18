#include "Game/EntityManager.h"
#include "Game/TransformComponent.h"
#include "Game/TestComponent.h"
#include "Game/TestSystem.h"
#include "SDL/SDL.h"
#include "Core/Math.h"
#include "Renderer/Window.h"
#include "Renderer/GraphicsDevice.h"


int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO); // Initializing

	SDL_Event event;

	sge::Window window("The test to end all tests.", 0, 0, 800, 600);
	sge::GraphicsDevice device(window);
	device.init();

	// Create systems and managers

	sge::TestSystem* testSystem = new sge::TestSystem();
	sge::EntityManager* EManager = new sge::EntityManager();


	// Create an entity through the managercheck 

	sge::Entity* player1 = EManager->createEntity();
	sge::Entity* player2 = EManager->createEntity();


	// Give the entity a certain component

	EManager->setComponent(player1, new sge::TestComponent(player1));
	EManager->setComponent(player2, new sge::TestComponent(player2));


	// Give the testsystem some components to work with

	testSystem->addComponent(player1->getComponent<sge::TestComponent>());
	testSystem->addComponent(player2->getComponent<sge::TestComponent>());


	// sge::TestComponent* comp = player1->getComponent<sge::TestComponent>();

	sge::math::vec2(1.0f, 2.0f);


	// Looooop

	bool running = true;
	while (running)
	{
		device.clear(0.5f, 0.0f, 0.7f, 1.0f);

		testSystem->update(); // Updates the testsystem which updates testcomponents

		while (SDL_PollEvent(&event)) 
		{
			switch (event.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					running = false;
				}
			}
		}

		device.swap();
	}

	// Testing component deletion

	EManager->removeComponent<sge::TestComponent>(*player1);
	EManager->removeComponent<sge::TestComponent>(*player2);

	device.deinit();

	return 0;
}