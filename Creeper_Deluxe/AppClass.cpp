#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
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
	m_pEntityMngr->SetModelMatrix(glm::translate(vector3(0.0f, -yOffset, 0.0f)), "Ground"); //translate the ground down to have the top be (0,0,0)

	float xOffset = GroundHalfWidth.x;
	float zOffset = GroundHalfWidth.z;

	//Walls
	for (uint i = 1; i <= 4; i++)
	{
		//Create the entities
		m_pEntityMngr->AddEntity("Custom\\Wall.fbx", "Wall" + std::to_string(i));

		//Transformation matrix
		matrix4 m4Model;

		//Translate the walls to the edge of the ground (Horizontal)
		if (i % 2 == 0)
		{
			matrix4 translation;
			matrix4 rotation;

			if (i == 4)
			{
				//Set the transformation
				translation = glm::translate(vector3(-xOffset, 0.0f, 0.0f));
				rotation = glm::rotate(IDENTITY_M4, 90.0f, AXIS_Y);
			}
			else
			{
				//Set the transformation
				translation = glm::translate(vector3(xOffset, 0.0f, 0.0f));
				rotation = glm::rotate(IDENTITY_M4, -90.0f, AXIS_Y);
			}

			//Rotate the model's forward
			MyEntity* wall = m_pEntityMngr->GetEntity(m_pEntityMngr->GetEntityIndex("Wall" + std::to_string(i)));
			vector3 v3Forward = wall->GetForward();
			v3Forward = vector3(rotation * vector4(v3Forward, 0.0f));
			wall->SetForward(v3Forward);

			//Set the model matrix
			m4Model = translation * rotation;
			m_pEntityMngr->SetModelMatrix(m4Model, "Wall" + std::to_string(i));
		}

		//Translate the walls to the edge of the ground (Vertical)
		else
		{
			//Put Wall on opposite side
			if (i == 3)
			{
				//Set the model matrix
				matrix4 translation = glm::translate(vector3(0.0f, 0.0f, zOffset));
				matrix4 rotation = glm::rotate(IDENTITY_M4, 180.0f, AXIS_Y);
				m4Model = translation * rotation;

				//Rotate the model's forward
				MyEntity* wall = m_pEntityMngr->GetEntity(m_pEntityMngr->GetEntityIndex("Wall" + std::to_string(i)));
				vector3 v3Forward = wall->GetForward();
				v3Forward = vector3(rotation * vector4(v3Forward, 0.0f));
				wall->SetForward(v3Forward);
			}
			else
			{
				m4Model = glm::translate(vector3(0.0f, 0.0f, -zOffset));
			}

			m_pEntityMngr->SetModelMatrix(m4Model, "Wall" + std::to_string(i));
		}
	}

	//Player (Steve)
	m_pEntityMngr->AddEntity("Custom\\Steve.fbx", "Steve");
	m_pEntityMngr->GetModel("Steve")->SetModelMatrix(glm::translate(m_pCameraMngr->GetPosition()));
	
	//Creeper
	m_pEntityMngr->AddEntity("Custom\\Creeper.fbx", "Creeper");
	m_pEntityMngr->SetModelMatrix(glm::translate(vector3(-2.0f, 0.0f, 0.0f)), "Creeper"); //Start Creeper slightly to the left

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
	//m_pEntityMngr->GetModel("Steve")->SetModelMatrix(m_pCameraMngr->GetCamera()->GetViewMatrix() * glm::translate(m_pCameraMngr->GetPosition() - vector3(0.0f, 1.7f, 1.0f)));
	m_pEntityMngr->GetModel("Steve")->SetModelMatrix(glm::translate(m_pCameraMngr->GetPosition() - vector3(0.0f, 1.7f, 0.0f)) *
													 glm::inverse(glm::extractMatrixRotation(m_pCameraMngr->GetCamera()->GetViewMatrix())) * 
													 glm::rotate(IDENTITY_M4, 180.0f, AXIS_Y));
	//m_pEntityMngr->GetModel("Steve")->SetModelMatrix(m_pEntityMngr->GetModelMatrix(1) * glm::rotate(IDENTITY_M4, 0.0f, AXIS_Y));
	

		//->SetModelMatrix(m_pCameraMngr->GetCamera()->GetViewMatrix());
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