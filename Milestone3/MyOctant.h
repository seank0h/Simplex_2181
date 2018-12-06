
#ifndef __MYOCTANT_H_
#define __MYOCTANT_H_

#include "Simplex\Simplex.h"
#include "MyEntityManager.h"

namespace Simplex
{

	//System Class
	class MyOctant
	{
		int m_nData = 0; //Number of elements in the list of elements
		std::vector<int> m_lData; //list of elements

		MeshManager* m_pMeshMngr = nullptr; //Pointer to Mesh manager
		MyEntityManager* m_pEntityMngr = nullptr;//Entity Manager Singleton
		
		//Count of the Children
		uint currChildrenCount = 0;
		static uint currOctantCount;//Curr number of octants
		static uint MaxIdealLevel;
		static uint m_uIdealEntityCount;//Number of entities that should be in each octatn
		

		MyOctant* Parent=nullptr;//Store parent of the current octant
		MyOctant* m_pChild[8];//Will store the children of the current octant 
	
		
		float m_fSize = 0.0f;//Size of the octant boundinb box
		
		static uint m_nCount;
		//Store vectors of the octant 
		vector3 m_v3Center = vector3(0.0f);
		vector3 m_v3Min = vector3(0.0f);
		vector3 m_v3Max = vector3(0.0f);

		std::vector<uint> m_EntityList;

		
		std::vector<MyOctant*> m_lChild;//List of nodes that have entities
	public:
		MyOctant * m_pRoot = nullptr;//Root octant
		uint octantID = 0;
		uint currLevel = 0;
		static uint maxLevel;
		//First Octant, constructor to create octant that will hold all entities that the mesh manager holds
		MyOctant(uint a_nMaxLevel = 2, uint a_nIdealEntityCount = 5);

		//Constructor of the octants after the first octatn
		MyOctant(vector3 a_v3Center, float a_fSize);

		/*
		Usage: Constructor
		Arguments: ---
		Output: class object instance
		*/
		MyOctant(MyOctant const& other);

		/*
		Usage: Copy Assignment Operator
		Arguments: class object to copy
		Output: ---
		*/
		MyOctant& operator=(MyOctant const& other);

		/*
		Usage: Destructor
		Arguments: ---
		Output: ---
		*/
		~MyOctant(void);

		/*
		Usage: Changes object contents for other object's
		Arguments: other -> object to swap content from
		Output: ---
		*/
		void Swap(MyOctant& other);
		//Getting Size of octant
		float  GetSize(void);
		//Getting the min max and center of the octant
		vector3 GetCenterGlobal(void);

		vector3 GetMinGlobal(void);

		vector3 GetMaxGlobal(void);

		//Check if there is collision using AARB while feeding in the ID of the entities
		bool isColliding(uint a_uRBIndex);
		
		//Displaying the volume of octant specified by index and all of its children, inital call
		void Display(uint a_nIndex, vector3 a_v3Color = C_YELLOW);
		//Displays volume of octant for children
		void Display(vector3 a_v3Color = C_YELLOW);

		//Displaying the non empty leafs
		void DisplayLeafs(vector3 a_v3Color = C_YELLOW);

		//Clearing the entity list
		void ClearEntityList(void);

		//Method that divides up the octant
		void Subdivide(void);

		//Method to return child based on index
		MyOctant* GetChild(uint a_nChild);
		//Method to get parent octant
		MyOctant* GetParent(void);

		//Checking to see if the octant has children
		bool isLeaf();

		//Asks for the amount of entities in box
		bool ContainsMoreThan(uint a_nEntities);

		//Kills the children recursively
		void KillBranches(void);

		//Creating the octree 
		void ConstructTree(uint a_nMaxLevel = 3);

		//Sets the index for the entities in box
		void AssignIDtoEntity(void);

		//Gets all the octants current
		uint GetOctantCount(void);
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

		//Creates list of all leaves that contain entites
		void ConstructList(void);
	};//class

} //namespace Simplex

#endif //__MYOCTANT_H_

  /*
  USAGE:
  ARGUMENTS: ---
  OUTPUT: ---
  */