#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	////Change this to your name and email
	//m_sProgrammer = "Alberto Bobadilla - labigm@rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));

	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(
		vector3(0.0f, 1.0f, 13.0f), //Position
		vector3(0.0f, 1.0f, 12.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

															  //Entity Manager
	m_pEntityMngr = MyEntityManager::GetInstance();

	//Ground
	//Create the entity
	m_pEntityMngr->AddEntity("Custom\\Ground.fbx", "Ground");

	//Get its halfwidth for positioning
	vector3 GroundHalfWidth = m_pEntityMngr->GetRigidBody("Ground")->GetHalfWidth();

	//Position the ground to make the very top have a y position of 0.
	float yOffset = GroundHalfWidth.y; //get distance from origin to the center of the ground
	m_pEntityMngr->SetModelMatrix(glm::translate(vector3(0.0f, -yOffset, 0.0f))); //translate the ground down to have the top be (0,0,0)

	float xOffset = GroundHalfWidth.x;
	float zOffset = GroundHalfWidth.z;

	//Walls
	for (uint i = 1; i <= 4; i++)
	{
		//Create the entities
		m_pEntityMngr->AddEntity("Custom\\Wall.fbx", "Wall" + i);

		//Translate the walls to the edge of the ground (Horizontal)
		if (i % 2 == 0)
		{
			//Put Wall on opposite side
			if (i == 4)
			{
				xOffset = -xOffset;
			}

			m_pEntityMngr->SetModelMatrix(glm::translate(vector3(xOffset, 0.0f, 0.0f)), "Wall" + i);
		}

		//Translate the walls to the edge of the ground (Vertical)
		else
		{
			//Put wall on opposite side
			if (i == 3)
			{
				zOffset = -zOffset;
			}

			matrix4 m4Model = glm::translate(vector3(0.0f, 0.0f, zOffset)); //translate
			m4Model = m4Model * glm::rotate(IDENTITY_M4, 90.0f, AXIS_Y); //rotate
			m_pEntityMngr->SetModelMatrix(m4Model, "Wall" + i);
		}
	}

	//Player (Steve)
	m_pEntityMngr->AddEntity("Custom\\Steve.fbx", "Steve");
	m_pEntityMngr->SetModelMatrix(glm::translate(vector3(2.0f, 0.0f, 0.0f))); //Start Steve slightly to the right

																			  //Creeper
	m_pEntityMngr->AddEntity("Custom\\Creeper.fbx", "Creeper");
	m_pEntityMngr->SetModelMatrix(glm::translate(vector3(-2.0f, 0.0f, 0.0f))); //Start Creeper slightly to the left

																			   //Mob Spawners
	m_pEntityMngr->AddEntity("Custom\\Mob Spawner.fbx", "Mob Spawner");
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	/* Set the updated model matrices for Steve and the Creepers here */
	//--
	//--
	//--

	// Generates Creepers
	// Creates 5 every five sentences
	// For this version, they begin spawning at the beginning of the world and move creeperCount units forward (just so we can see and make sure it works right)
	static short creeperCount = 0;
	static float time = 0;
	static uint clock = m_pSystem->GenClock();
	if (creeperCount < 30) {
		time += m_pSystem->GetDeltaTime(clock);

		if ((uint)time == 5) {
			for (int i = 0; i < 5; i++) {
				m_pEntityMngr->AddEntity("Custom\\Creeper.fbx", "Creeper");
				m_pEntityMngr->SetModelMatrix(glm::translate(vector3(0.0f, 0.0f, ((float)creeperCount))));
				creeperCount++;
			}
			time = m_pSystem->GetDeltaTime(clock);
		}
	}
	

	//Update Entity Manager
	m_pEntityMngr->Update();

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();
	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release the entity manager
	m_pEntityMngr->ReleaseInstance();

	//release GUI
	ShutdownGUI();
}