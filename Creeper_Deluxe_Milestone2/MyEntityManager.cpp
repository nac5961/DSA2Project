#include "MyEntityManager.h"
using namespace Simplex;
//  MyEntityManager
MyEntityManager* MyEntityManager::m_pInstance = nullptr;
void MyEntityManager::Init(void)
{
	m_uEntityCount = 0;
	m_entityList.clear();
}
void MyEntityManager::Release(void)
{
	for (uint uEntity = 0; uEntity < m_uEntityCount; ++uEntity)
	{
		MyEntity* pEntity = m_entityList[uEntity];
		SafeDelete(pEntity);
	}
	m_uEntityCount = 0;
	m_entityList.clear();
}
MyEntityManager* MyEntityManager::GetInstance()
{
	if(m_pInstance == nullptr)
	{
		m_pInstance = new MyEntityManager();
	}
	return m_pInstance;
}
void MyEntityManager::ReleaseInstance()
{
	if(m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
int Simplex::MyEntityManager::GetEntityIndex(String a_sUniqueID)
{
	//look one by one for the specified unique id
	for (uint uIndex = 0; uIndex < m_uEntityCount; ++uIndex)
	{
		if (a_sUniqueID == m_entityList[uIndex]->GetUniqueID())
			return uIndex;
	}
	//if not found return -1
	return -1;
}
//Accessors
Model* Simplex::MyEntityManager::GetModel(uint a_uIndex)
{
	//if the list is empty return blank
	if (m_entityList.size() == 0)
		return nullptr;

	// if out of bounds
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount - 1;

	return m_entityList[a_uIndex]->GetModel();
}
Model* Simplex::MyEntityManager::GetModel(String a_sUniqueID)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		return pTemp->GetModel();
	}
	return nullptr;
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(uint a_uIndex)
{
	//if the list is empty return blank
	if (m_entityList.size() == 0)
		return nullptr;

	// if out of bounds
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount - 1;

	return m_entityList[a_uIndex]->GetRigidBody();
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(String a_sUniqueID)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		return pTemp->GetRigidBody();
	}
	return nullptr;
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(uint a_uIndex)
{
	//if the list is empty return blank
	if (m_entityList.size() == 0)
		return IDENTITY_M4;

	// if out of bounds
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount - 1;

	return m_entityList[a_uIndex]->GetModelMatrix();
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(String a_sUniqueID)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		return pTemp->GetModelMatrix();
	}
	return IDENTITY_M4;
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, String a_sUniqueID)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		pTemp->SetModelMatrix(a_m4ToWorld);
	}
}
void Simplex::MyEntityManager::SetAxisVisibility(bool a_bVisibility, uint a_uIndex)
{
	//if the list is empty return blank
	if (m_entityList.size() == 0)
		return;

	// if out of bounds
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount - 1;

	return m_entityList[a_uIndex]->SetAxisVisible(a_bVisibility);
}
void Simplex::MyEntityManager::SetAxisVisibility(bool a_bVisibility, String a_sUniqueID)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		pTemp->SetAxisVisible(a_bVisibility);
	}
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, uint a_uIndex)
{
	//if the list is empty return blank
	if (m_entityList.size() == 0)
		return;

	// if out of bounds
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount -1;

	m_entityList[a_uIndex]->SetModelMatrix(a_m4ToWorld);
}
//The big 3
MyEntityManager::MyEntityManager(){Init();}
MyEntityManager::MyEntityManager(MyEntityManager const& other){ }
MyEntityManager& MyEntityManager::operator=(MyEntityManager const& other) { return *this; }
MyEntityManager::~MyEntityManager(){Release();};
// other methods
void Simplex::MyEntityManager::Update(void)
{	
	//check collisions
	for (uint i = 0; i < m_uEntityCount; i++)
	{
		//Don't perform any calculations if the game is over
		if (gameOver)
		{
			break;
		}

		//Get the entity type
		char entity = m_entityList[i]->GetUniqueID()[0];

		//Ignore checking collisions for the ground, walls, mob spawners, and bullets
		if (entity == 'G' || entity == 'W' || entity == 'M' || entity == 'B')
		{
			continue;
		}

		for (uint j = 0; j < m_uEntityCount; j++)
		{
			//Don't perform any more calculations if the game is over
			if (gameOver)
			{
				break;
			}

			//Get the entity type for the second entity
			char otherEntity = m_entityList[j]->GetUniqueID()[0];

			//Ignore collisions with self
			if (i == j)
			{
				continue;
			}

			//Collisions for creepers
			if (entity == 'C')
			{
				//Check collisions with walls
				if (otherEntity == 'W')
				{
					if (m_entityList[i]->IsColliding(m_entityList[j]))
					{
						//Get the entity for the wall and the creeper
						MyEntity* creeper = m_entityList[i];
						MyEntity* wall = m_entityList[j];

						//Get the position of the creeper
						vector3 creeperPos = creeper->GetPos();

						//Get the forward of the wall
						vector3 wallForward = wall->GetForward();

						//Move creeper back according to the wall's forward
						creeperPos += wallForward * 0.1f;

						//Set the creeper's new position
						creeper->SetPos(creeperPos);

						//Set the creeper's model matrix
						creeper->SetModelMatrix(glm::translate(creeperPos));
					}
				}

				//Check collisions with bullet
				else if (otherEntity == 'B')
				{
					if (m_entityList[i]->IsColliding(m_entityList[j]))
					{
						if (m_entityList[i]->GetCanDelete() == false && m_entityList[j]->GetCanDelete() == false)
						{
							//Mark the bullet and the creeper for deletion
							m_entityList[i]->MarkToDelete();
							m_entityList[j]->MarkToDelete();

							//Increase the player's kill count
							numKilled++;
						}
					}
				}
			}

			//Collisions for the player (Steve)
			else if (entity == 'S')
			{
				//Check collisions with walls
				if (otherEntity == 'W')
				{
					if (m_entityList[i]->IsColliding(m_entityList[j]))
					{
						//Get the entity for the wall and the player
						MyEntity* player = m_entityList[i];
						MyEntity* wall = m_entityList[j];

						//Get the position of the player
						vector3 playerPos = player->GetPos();

						//Get the forward of the wall
						vector3 wallForward = wall->GetForward();

						//Move player back according to the wall's forward
						playerPos += wallForward * 0.1f;

						//Set the player's new position
						player->SetPos(playerPos);

						//Set the player's model matrix
						matrix4 translate = glm::translate(playerPos);
						matrix4 rotation = ToMatrix4(player->GetRotation());
						matrix4 m4Model = translate * rotation;
						player->SetModelMatrix(m4Model);
					}
				}

				//Check collisions with creepers
				else if (otherEntity == 'C')
				{
					if (m_entityList[i]->IsColliding(m_entityList[j]))
					{
						//Get the creeper's entity
						MyEntity* creeper = m_entityList[j];

						//Only damage the player if the creeper isn't stunned
						//Note: creepers are stunned once they damage the player to avoid
						//dealing constant damage
						if (creeper->GetWaitTime() <= 0.0f)
						{
							//Decrease the player's # of lives
							numLives--;
						}

						//Check if the game is over
						if (numLives <= 0)
						{
							gameOver = true;
						}

						//Perform calculations if the game isn't over
						if (!gameOver)
						{
							//Stop the creeper from moving
							if (creeper->GetWaitTime() <= 0.0f)
							{
								creeper->ResetWaitTime();
							}
						}
					}
				}
			}
		}
	}
}
void Simplex::MyEntityManager::AddEntity(String a_sFileName, String a_sUniqueID)
{
	//Create a temporal entity to store the object
	MyEntity* pTemp = new MyEntity(a_sFileName, a_sUniqueID);
	//if I was able to generate it add it to the list
	if (pTemp->IsInitialized())
	{
		m_entityList.push_back(pTemp);
		m_uEntityCount = m_entityList.size();
	}
}
void Simplex::MyEntityManager::RemoveEntity(uint a_uIndex)
{
	//if the list is empty return
	if (m_entityList.size() == 0)
		return;

	// if out of bounds choose the last one
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount - 1;

	// if the entity is not the very last we swap it for the last one
	if (a_uIndex != m_uEntityCount - 1)
	{
		std::swap(m_entityList[a_uIndex], m_entityList[m_uEntityCount - 1]);
	}
	
	//and then pop the last one
	MyEntity* pTemp = m_entityList[m_uEntityCount - 1];
	SafeDelete(pTemp);
	m_entityList.pop_back();
	--m_uEntityCount;
	return;
}
void Simplex::MyEntityManager::RemoveEntity(String a_sUniqueID)
{
	int nIndex = GetEntityIndex(a_sUniqueID);
	RemoveEntity((uint)nIndex);
}
String Simplex::MyEntityManager::GetUniqueID(uint a_uIndex)
{
	//if the list is empty return blank
	if (m_entityList.size() == 0)
		return "";
	//if the index is larger than the number of entries we are asking for the last one
	if (a_uIndex >= m_entityList.size())
		a_uIndex = m_entityList.size() - 1;
	return m_entityList[a_uIndex]->GetUniqueID();
}
MyEntity* Simplex::MyEntityManager::GetEntity(uint a_uIndex)
{
	//if the list is empty return blank
	if (m_entityList.size() == 0)
		return nullptr;

	//if the index is larger than the number of entries we are asking for the last one
	if (a_uIndex >= m_entityList.size())
		a_uIndex = m_entityList.size() - 1;

	return m_entityList[a_uIndex];
}
void Simplex::MyEntityManager::AddEntityToRenderList(uint a_uIndex, bool a_bRigidBody)
{
	//if out of bounds will do it for all
	if (a_uIndex >= m_uEntityCount)
	{
		//add for each one in the entity list
		for (a_uIndex = 0; a_uIndex < m_uEntityCount; ++a_uIndex)
		{
			m_entityList[a_uIndex]->AddToRenderList(a_bRigidBody);
		}
	}
	else //do it for the specified one
	{
		m_entityList[a_uIndex]->AddToRenderList(a_bRigidBody);
	}
}
void Simplex::MyEntityManager::AddEntityToRenderList(String a_sUniqueID, bool a_bRigidBody)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		pTemp->AddToRenderList(a_bRigidBody);
	}
}