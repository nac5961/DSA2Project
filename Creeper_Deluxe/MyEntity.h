/*----------------------------------------------
Programmer: Alberto Bobadilla (labigm@gmail.com)
Date: 2017/07
----------------------------------------------*/
#ifndef __MYENTITY_H_
#define __MYENTITY_H_

#include "Simplex\Simplex.h"

namespace Simplex
{

//System Class
class MyEntity
{
	bool canDelete = false; //for bullets and creepers

	float lifeTime = 4.0f; //for bullets
	float waitTime = 0.0f; //for creepers
	int health = 3; //for creepers

	//for all entities
	vector3 pos = ZERO_V3;
	vector3 forward = AXIS_Z;
	vector3 up = AXIS_Y;
	vector3 right = AXIS_X;
	quaternion rotation;

	bool m_bInMemory = false; //loaded flag
	bool m_bSetAxis = false; //render axis flag
	String m_sUniqueID = ""; //Unique identifier name

	Model* m_pModel = nullptr; //Model associated with this Entity
	RigidBody* m_pRigidBody = nullptr; //Rigid Body associated with this Entity

	matrix4 m_m4ToWorld = IDENTITY_M4; //Model matrix associated with this Entity
	MeshManager* m_pMeshMngr = nullptr; //For rendering shapes

	static std::map<String, MyEntity*> m_IDMap; //a map of the unique ID's

public:
	/* Getters and Setters */
	void MarkToDelete() { canDelete = true; }
	bool GetCanDelete() { return canDelete; }

	void DecreaseLifeTime(float a_fTime) { lifeTime -= a_fTime; }
	float GetLifeTime() { return lifeTime; }

	void ResetWaitTime() { waitTime = 3.0f; }
	void SetWaitTime(float a_fTime) { waitTime = a_fTime; }
	float GetWaitTime() { return waitTime; }

	void SetHealth(int a_iHealth) { health = a_iHealth; }
	int GetHealth() { return health; }

	void SetRotation(quaternion a_qRotation) { rotation = a_qRotation; }
	quaternion GetRotation() { return rotation; }

	void SetPos(vector3 a_v3Pos) { pos = a_v3Pos; }
	vector3 GetPos() { return pos; }

	void SetRight(vector3 a_v3Pos) { right = a_v3Pos; }
	vector3 GetRight() { return right; }

	void SetUp(vector3 a_v3Pos) { up = a_v3Pos; }
	vector3 GetUp() { return up; }

	void SetForward(vector3 a_v3Pos) { forward = a_v3Pos; }
	vector3 GetForward() { return forward; }

	/*
	Usage: Constructor that specifies the name attached to the Entity
	Arguments:
	-	String a_sFileName -> Name of the model to load
	-	String a_sUniqueID -> Name wanted as identifier, if not available will generate one
	Output: class object instance
	*/
	MyEntity(String a_sFileName, String a_sUniqueID = "NA");
	/*
	Usage: Copy Constructor
	Arguments: class object to copy
	Output: class object instance
	*/
	MyEntity(MyEntity const& other);
	/*
	Usage: Copy Assignment Operator
	Arguments: class object to copy
	Output: ---
	*/
	MyEntity& operator=(MyEntity const& other);
	/*
	Usage: Destructor
	Arguments: ---
	Output: ---
	*/
	~MyEntity(void);
	/*
	Usage: Changes object contents for other object's
	Arguments: other -> object to swap content from
	Output: ---
	*/
	void Swap(MyEntity& other);
	/*
	USAGE: Gets the model matrix associated with this entity
	ARGUMENTS: ---
	OUTPUT: model to world matrix
	*/
	matrix4 GetModelMatrix(void);
	/*
	USAGE: Sets the model matrix associated with this entity
	ARGUMENTS: matrix4 a_m4ToWorld -> model matrix to set
	OUTPUT: ---
	*/
	void SetModelMatrix(matrix4 a_m4ToWorld);
	/*
	USAGE: Gets the model associated with this entity
	ARGUMENTS: ---
	OUTPUT: Model
	*/
	Model* GetModel(void);
	/*
	USAGE: Gets the Rigid Body associated with this entity
	ARGUMENTS: ---
	OUTPUT: Rigid Body
	*/
	RigidBody* GetRigidBody(void);
	/*
	USAGE: Will reply to the question, is the Entity Initialized?
	ARGUMENTS: ---
	OUTPUT: initialized?
	*/
	bool IsInitialized(void);
	/*
	USAGE: Adds the entity to the render list
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void AddToRenderList(bool a_bDrawRigidBody = false);
	/*
	USAGE: Tells if this entity is colliding with the incoming one
	ARGUMENTS: MyEntity* const other -> inspected entity
	OUTPUT: are they colliding?
	*/
	bool IsColliding(MyEntity* const other);
	/*
	USAGE: Gets the Entity specified by unique ID, nullptr if not exists
	ARGUMENTS: String a_sUniqueID -> unique ID if the queried entity
	OUTPUT: Entity specified by unique ID, nullptr if not exists
	*/
	static MyEntity* GetEntity(String a_sUniqueID);
	/*
	USAGE: Will generate a unique id based on the name provided
	ARGUMENTS: String& a_sUniqueID -> desired name
	OUTPUT: will output though the argument
	*/
	void GenUniqueID(String& a_sUniqueID);
	/*
	USAGE: Gets the Unique ID name of this model
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	String GetUniqueID(void);
	/*
	USAGE: Sets the visibility of the axis of this Entity
	ARGUMENTS: bool a_bSetAxis = true -> axis visible?
	OUTPUT: ---
	*/
	void SetAxisVisible(bool a_bSetAxis = true);

private:
	/*
	Usage: Deallocates member fields
	Arguments: ---
	Output: ---
	*/
	void Release(void);
	/*
	Usage: Allocates member fields
	Arguments: ---
	Output: ---
	*/
	void Init(void);
};//class

} //namespace Simplex

#endif //__MYENTITY_H_

/*
USAGE:
ARGUMENTS: ---
OUTPUT: ---
*/