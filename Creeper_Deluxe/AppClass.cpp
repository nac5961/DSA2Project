#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(
		vector3(0.0f, 1.0f, 13.0f), //Position
		vector3(0.0f, 1.0f, 12.0f),	//Target
		AXIS_Y);					//Up

	//Enable first person mode
	m_bFPC = true;

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
	
	//Creeper (Preload to avoid texture bug)
	m_pEntityMngr->AddEntity("Custom\\Creeper.fbx", "Creeper");
	m_pEntityMngr->SetModelMatrix(glm::translate(vector3(0.0f, 40.0f, 0.0f)), -1);
	m_pEntityMngr->GetEntity(-1)->MarkToDelete(); //delete immediately

	//Bullet (Preload to avoid texture bug)
	m_pEntityMngr->AddEntity("Custom\\Bullet.fbx", "Bullet");
	m_pEntityMngr->SetModelMatrix(glm::translate(vector3(0.0f, 40.0f, 0.0f)), -1);
	m_pEntityMngr->GetEntity(-1)->MarkToDelete(); //delete immediately

	//Mob Spawners
	for (int i = 1; i <= 4; i++)
	{
		float offset = 6.0f;
		float x = xOffset;
		float z = zOffset;

		if (i % 2 == 0)
		{
			//Position on opposite side
			if (i == 4)
			{
				z = -z;
				x = -x;
				offset = -offset;
			}

			//Create entity
			m_pEntityMngr->AddEntity("Custom\\Mob Spawner.fbx", "Mob Spawner" + std::to_string(i));

			//Get entity
			MyEntity* mobSpawner = m_pEntityMngr->GetEntity(-1);

			//Set position
			mobSpawner->SetPos(vector3(x - offset, 0.0f, -z + offset));

			//Set model matrix
			mobSpawner->SetModelMatrix(glm::translate(mobSpawner->GetPos()));
		}
		else
		{
			//Position on opposite side
			if (i == 3)
			{
				z = -z;
				x = -x;
				offset = -offset;
			}

			//Create entity
			m_pEntityMngr->AddEntity("Custom\\Mob Spawner.fbx", "Mob Spawner" + std::to_string(i));

			//Get entity
			MyEntity* mobSpawner = m_pEntityMngr->GetEntity(-1);

			//Set position
			mobSpawner->SetPos(vector3(x - offset, 0.0f, z - offset));

			//Set model matrix
			mobSpawner->SetModelMatrix(glm::translate(mobSpawner->GetPos()));
		}
	}
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//If the life count isn't updated
	if (m_iLives != m_pEntityMngr->GetNumLives())
	{
		//Update the life count
		m_iLives = m_pEntityMngr->GetNumLives();

		//Play player sound effect
		
	}

	if (!m_pEntityMngr->GetGameOver())
	{
		//Statics for creeper generation and delta time
		static short creeperCount = 0;
		static float fTime = 0;
		static uint uClock = m_pSystem->GenClock();
		float deltaTime = m_pSystem->GetDeltaTime(uClock);

		// Generates Creepers
		// Creates 5 every five sentences
		// For this version, they begin spawning at the beginning of the world and move creeperCount units forward (just so we can see and make sure it works right)
		if (creeperCount < 30) {
			fTime += deltaTime;
			std::cout << fTime << std::endl;

			if ((uint)fTime >= 5) {
				for (int i = 0; i < 5; i++) {
					m_pEntityMngr->AddEntity("Custom\\Creeper.fbx", "Creeper");
					MyEntity* creeper = m_pEntityMngr->GetEntity(-1);

					int index = (rand() % 4) + 1;
					float xOffset = (rand() / (float)RAND_MAX * (spawnRadius * 2)) - spawnRadius;
					float zOffset = (rand() / (float)RAND_MAX * (spawnRadius * 2)) - spawnRadius;
					MyEntity* spawn = nullptr;
					spawn = m_pEntityMngr->GetEntity(m_pEntityMngr->GetEntityIndex("Mob Spawner" + std::to_string(index)));

					int counter = 0;
					/*
					for (uint i = 0; i < m_pEntityMngr->GetEntityCount(); i++) {
					if (m_pEntityMngr->GetUniqueID(i).find("Mob Spawner") != std::string::npos) {
					if (counter == index) {
					spawn = m_pEntityMngr->GetEntity(i);
					break;
					}
					counter++;
					}
					}*/
					creeper->SetPos(spawn->GetPos() + vector3(xOffset, 0.0f, zOffset));
					creeper->SetModelMatrix(glm::translate(/*vector3(0.0f, 0.0f, ((float)creeperCount))*/  /*spawners[index].GetPos()*/  creeper->GetPos()));
					creeperCount++;
					printf("creeper made");
				}
				fTime = m_pSystem->GetDeltaTime(uClock);
			}
		}

		/* Set the updated model matrices for Steve and the Creepers here */
		for (uint i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
		{
			//Get the entity type
			char entity = m_pEntityMngr->GetUniqueID(i)[0];

			//Ignore walls, the ground, and mob spawners
			if (entity == 'W' || entity == 'G' || entity == 'M')
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
			if (entity == 'C')
			{
				//Get the creeper entity
				MyEntity* creeper = m_pEntityMngr->GetEntity(i);

				//Creeper is stunned
				if (creeper->GetWaitTime() > 0.0f)
				{
					//Get the stun/wait time
					float waitTime = creeper->GetWaitTime();

					//Decrease it by delta time
					waitTime -= deltaTime;

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
					//Get the player entity
					MyEntity* player = m_pEntityMngr->GetEntity(m_pEntityMngr->GetEntityIndex("Steve"));

					//Get the player's and the creeper's position
					vector3 playerPos = player->GetPos();
					vector3 creeperPos = creeper->GetPos();

					//Get vector from the creeper to the player
					vector3 creeperToPlayer = playerPos - creeperPos;

					//Normalize the vector
					float creeperVecMag = sqrtf(pow(creeperToPlayer.x, 2) + pow(creeperToPlayer.y, 2) + pow(creeperToPlayer.z, 2));

					creeperToPlayer.x = creeperToPlayer.x / creeperVecMag;
					creeperToPlayer.y = creeperToPlayer.y / creeperVecMag;
					creeperToPlayer.z = creeperToPlayer.z / creeperVecMag;

					//Set vector as the creeper's new forward
					creeper->SetForward(creeperToPlayer);

					//Add the creeper's forward to the creeper's position
					creeperPos += creeper->GetForward() * 0.05f;
					//vector3 creeperMove = (creeperPos + creeper->GetForward()) * 0.1f;
					creeper->SetPos(creeperPos);

					//Set the model matrix of the creeper to translate to its new position
					creeper->SetModelMatrix(glm::translate(creeper->GetPos()));
				}

				//Get creeper health
				int health = creeper->GetHealth();

				//Full Health (Green Bar)
				if (health == 3)
				{
					//Get the creeper model matrix
					matrix4 creeperMatrix = creeper->GetModelMatrix();

					//Set health bar model matrix
					matrix4 barTranslation = glm::translate(vector3(0.0f, 2.0f, 0.0f));
					matrix4 barScale = glm::scale(vector3(1.0f, 0.18f, 0.14f));
					matrix4 m4Health = creeperMatrix * barTranslation * barScale;

					//Draw green bar above creeper
					m_pMeshMngr->AddCubeToRenderList(m4Health, C_GREEN);
				}

				//Hit Once (Yellow Bar)
				else if (health == 2)
				{
					//Get the creeper model matrix
					matrix4 creeperMatrix = creeper->GetModelMatrix();

					//Set health bar model matrix
					matrix4 barTranslation = glm::translate(vector3(0.0f, 2.0f, 0.0f));
					matrix4 barScale = glm::scale(vector3(0.7f, 0.18f, 0.14f));
					matrix4 m4Health = creeperMatrix * barTranslation * barScale;

					//Draw yellow bar above creeper
					m_pMeshMngr->AddCubeToRenderList(m4Health, C_YELLOW);
				}

				//Hit Twice (Red Bar)
				else if (health == 1)
				{
					//Get the creeper model matrix
					matrix4 creeperMatrix = creeper->GetModelMatrix();

					//Set health bar model matrix
					matrix4 barTranslation = glm::translate(vector3(0.0f, 2.0f, 0.0f));
					matrix4 barScale = glm::scale(vector3(0.3f, 0.18f, 0.14f));
					matrix4 m4Health = creeperMatrix * barTranslation * barScale;

					//Draw red bar above creeper
					m_pMeshMngr->AddCubeToRenderList(m4Health, C_RED);
				}
			}

			//Player
			else if (entity == 'S')
			{
				//Get the player entity
				MyEntity* player = m_pEntityMngr->GetEntity(i);

				//Get the translation and rotation matrices
				matrix4 translation = glm::translate(player->GetPos());
				matrix4 rotation = ToMatrix4(player->GetRotation());
				matrix4 m4Model = translation * rotation;

				//Set the model matrix
				m_pEntityMngr->SetModelMatrix(m4Model, i);
			}

			//Bullet
			else if (entity == 'B')
			{
				//Get the bullet entity
				MyEntity* bullet = m_pEntityMngr->GetEntity(i);

				//Get the position and forward vectors
				vector3 bulletPos = bullet->GetPos();
				vector3 bulletForward = bullet->GetForward();

				//Move the bullet along the forward vector
				bulletPos += bulletForward * 0.5f;

				//Set the bullet's new position
				bullet->SetPos(bulletPos);

				//Set the model matrix
				m_pEntityMngr->SetModelMatrix(glm::translate(bulletPos), i);

				//Decrease the bullet's life time
				bullet->DecreaseLifeTime(deltaTime);

				//Check if the bullet needs to be deleted
				if (bullet->GetLifeTime() <= 0)
				{
					bullet->MarkToDelete();
				}
			}
		}

		//Update Entity Manager
		m_pEntityMngr->Update();

		//Is the first person camera active?
		CameraRotation();

		//Set the camera's position, target, and up vectors
		MyEntity* player = m_pEntityMngr->GetEntity(m_pEntityMngr->GetEntityIndex("Steve"));
		vector3 offset = player->GetPos() + vector3(0.0f, 1.6f, 0.0f);
		m_pCameraMngr->SetPositionTargetAndUp(offset, offset + player->GetForward(), player->GetUp());
	}

	//Add objects to render list
	if (m_bDebug)
	{
		m_pEntityMngr->AddEntityToRenderList(-1, true);
	}
	else
	{
		m_pEntityMngr->AddEntityToRenderList(-1, false);
	}
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