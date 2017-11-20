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
	m_pEntityMngr->SetModelMatrix(glm::translate(vector3(2.0f, 0.0f, 0.0f)), "Steve"); //Start Steve slightly to the right

	//Creeper
	m_pEntityMngr->AddEntity("Custom\\Creeper.fbx", "Creeper");
	m_pEntityMngr->SetModelMatrix(glm::translate(vector3(-2.0f, 0.0f, 0.0f)), "Creeper"); //Start Creeper slightly to the left

	//Mob Spawners
	m_pEntityMngr->AddEntity("Custom\\Mob Spawner.fbx", "Mob Spawner");

	std::string oll = "Wall1";
	if (oll.find("Wall2") != std::string::npos)
	{
		std::cout << "I found it";
	}
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
	for (uint i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		//Ignore walls, the ground, and mob spawners
		if (m_pEntityMngr->GetUniqueID(i).find("Wall") != std::string::npos || m_pEntityMngr->GetUniqueID(i).find("Ground") != std::string::npos
			|| m_pEntityMngr->GetUniqueID(i).find("Mob Spawner") != std::string::npos)
		{
			continue;
		}

		//Delete the entity if it's ready to be deleted (Creeper and Bullet)
		if (m_pEntityMngr->GetEntity(i)->GetCanDelete())
		{
			//Remove the entity from the list
			m_pEntityMngr->RemoveEntity(i);

			//Set i back 1 to represent the accurate index
			i--;

			//Go to the next iteration
			continue;
		}

		//Creeper
		if (m_pEntityMngr->GetUniqueID(i).find("Creeper") != std::string::npos)
		{
			//Get the creeper and player entity
			MyEntity* creeper = m_pEntityMngr->GetEntity(i);
			MyEntity* player = m_pEntityMngr->GetEntity(m_pEntityMngr->GetEntityIndex("Steve"));

			//Get the player's and the creeper's position
			vector3 creeperPos = creeper->GetPos();
			vector3 playerPos = player->GetPos();

			//Creeper is stunned
			if (creeper->GetWaitTime() > 0.0f)
			{
				//Get the stun/wait time
				float waitTime = creeper->GetWaitTime();

				//Decrease it by delta time
				//waitTime -=

				//Clamp the wait time
				if (waitTime <= 0.0f)
				{
					waitTime = 0.0f;
				}

				//Update the wait time
				creeper->SetWaitTime(waitTime);
			}

			//Creeper not stunned
			else
			{
				//Get vector from the creeper to the player

				//Normalize the vector

				//Set vector as the creeper's new forward

				//Add the creeper's forward to the creeper's position
			}

			//Set the model matrix of the creeper to translate to its new position
		}

		//Player
		else if (m_pEntityMngr->GetUniqueID(i).find("Steve") != std::string::npos)
		{
			//Get the player entity
			MyEntity* player = m_pEntityMngr->GetEntity(i);

			//Get the translation and rotation matrices
			//matrix4 translation = glm::translate(player->GetPos());
			//matrix4 rotation = ToMatrix4();
			//matrix4 m4Model = translation * rotation;

			//Get translation matrix
			matrix4 m4Model = glm::translate(player->GetPos());

			//Set the model matrix
			m_pEntityMngr->SetModelMatrix(m4Model, i);
		}

		//Bullet
		else if (m_pEntityMngr->GetUniqueID(i).find("Bullet") != std::string::npos)
		{
			//Get the bullet entity
			MyEntity* bullet = m_pEntityMngr->GetEntity(i);

			//Get the position and forward vectors
			vector3 bulletPos = bullet->GetPos();
			vector3 bulletForward = bullet->GetForward();

			//Move the bullet along the forward vector
			bulletPos += bulletForward * 0.1f;

			//Set the bullet's new position
			bullet->SetPos(bulletPos);

			//Set the model matrix
			m_pEntityMngr->SetModelMatrix(glm::translate(bulletPos), i);

			//Decrease the bullet's life time
			
			//Check if the bullet needs to be deleted
			if (bullet->GetLifeTime() <= 0)
			{
				bullet->MarkToDelete();
			}
		}

	}
	//m_pEntityMngr->SetModelMatrix(glm::translate(m_v3Creeper) * ToMatrix4(m_qArcBall), "Creeper");
	//MyEntity* player = m_pEntityMngr->GetEntity(m_pEntityMngr->GetEntityIndex("Creeper"));
	//m_pEntityMngr->SetModelMatrix(glm::translate(player->GetPos()), "Creeper");
	//--
	//--
	//--

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