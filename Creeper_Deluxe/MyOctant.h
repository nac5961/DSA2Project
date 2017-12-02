#ifndef _MYOCTANT_H_
#define _MYOCTANT_H_

#include "MyEntityManager.h"
//#include "Simplex\Physics\Octant.h"

namespace Simplex
{

	class MyOctant
	{

	private:
		static uint m_uOctantCount; //total number of octants in the scene
		static uint m_uMaxLevel; //number of octree subdivisions

		uint m_uID = 0; //id of octant
		uint m_uLevel = 0; //the current level of the octant
		uint m_uChildren = 0; //number of children an octant has (either 0 or 8)

		float m_fSize = 0.0f; //size of the octant

		MeshManager* m_pMeshManager = nullptr;
		MyEntityManager* m_pEntityManager = nullptr;

		vector3 m_v3Center = ZERO_V3;
		vector3 m_v3Min = ZERO_V3;
		vector3 m_v3Max = ZERO_V3;

		std::vector<uint> m_EntityList; //list of entities in this octant

		MyOctant* m_pRoot = nullptr; //root node
		MyOctant* m_pParent = nullptr; //parent node
		MyOctant* m_pChildren[8]; //child nodes

	public:
		//Main Constructor
		//a_uLevel -> max subdivision level
		MyOctant(uint a_uMaxLevel);

		//Sub Constructor
		//a_v3Center -> center of octant
		//a_fSize -> size of octant
		MyOctant(MyOctant* a_pParent, vector3 a_v3Center, float a_fSize, uint a_uMaxLevel);

		//Destructor
		//Deletes all octants in the octree
		~MyOctant();

		//Getter
		//Returns size of octant
		float GetSize();

		//Getter
		//Returns center of octant in global space
		vector3 GetCenterGlobal();

		//Getter
		//Returns min of octant in global space
		vector3 GetMinGlobal();

		//Getter
		//Returns max of octant in global space
		vector3 GetMaxGlobal();

		//Checks to see if the specified entity is colliding with the octant
		bool IsColliding(uint a_uIndex);

		//Displays the entire octree
		void Display();

		//Displays the octant at the specified index
		void Display(uint a_uIndex);

		//Clears entity list for all nodes
		void ClearEntityList();

		//Returns the child octant specified (0-7)
		MyOctant* GetChild(uint a_uIndex);

		//Returns the parent octant of this octant
		MyOctant* GetParent();

		//Returns if this octant is a leaf node (no children)
		bool IsLeaf();

		//Returns total number of octants created
		uint GetOctantCount();

		//Constructs a tree according to the max level
		void ConstructTree(uint a_uLevel);

		//Gives an entity in the scene an octant ID
		void AssignIDtoEntity();

		//Deletes all octants in the octree
		//Used in the destructor to avoid memory leaks
		void KillBranches();
	};

}




#endif // !_MYOCTANT_H_
