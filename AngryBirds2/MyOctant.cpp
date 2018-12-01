#include "MyOctant.h"
using namespace Simplex;
uint MyOctant::m_uIdealEntityCount = 5;
uint MyOctant::maxLevel = 5;
uint MyOctant::currOctantCount = 0;
#pragma region MaybeUse


//  MyOctant

//Initalize Variables
void MyOctant::Init(void)
{
	currChildrenCount = 0;

	m_fSize = 0.0f;

	octantID = currOctantCount;

	currLevel = 0;

	//Getting singletons
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	Parent = nullptr;

	m_pRoot = nullptr;

	m_v3Center = vector3(0.0f);
	m_v3Max = vector3(0.0f);
	m_v3Min = vector3(0.0f);

	for (size_t i = 0; i < 8; i++)
	{
		m_pChild[i] = nullptr;
	}
	
}

//Constructor to create the octane holding the ideal amount of entities
MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	//Initializing Variables
	Init();

	currOctantCount = 0;
	maxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
	octantID = currOctantCount;

	//FIrst octant made should be root
	m_pRoot = this;
	//Making sure no children
	m_lChild.clear();

	//List that will hold the minMAx needed for bounding box
	std::vector<vector3> boundingBox;

	//Number of entites
	uint entityCount = m_pEntityMngr->GetEntityCount();
	for (size_t i = 0; i < entityCount; i++)
	{
		//Creating pointers to all the entites
		MyEntity* pEntity = m_pEntityMngr->GetEntity(i);
		//Getting pointers to their rigid bodies
		MyRigidBody* pRigidBody = pEntity->GetRigidBody();

		//Gettubg the min and max for each entity to create the bounding box for each
		boundingBox.push_back(pRigidBody->GetMinGlobal());
		boundingBox.push_back(pRigidBody->GetMaxGlobal());
	}
	//Creating a rigidbody with the bounding box vertices
	MyRigidBody* pRigidBody = new MyRigidBody(boundingBox);

	vector3 halfWidth = pRigidBody->GetHalfWidth();

	float fMax = halfWidth.x;

	for (int i = 0; i < 3; i++)
	{
		if (fMax < halfWidth[i])
		{
			fMax = halfWidth[i];
		}
	}
	vector3 center = pRigidBody->GetCenterLocal();

	boundingBox.clear();

	SafeDelete(pRigidBody);

	//Setting the vectors for the bounding box of the octant
	m_fSize = fMax * 2.0f;
	m_v3Center = center;
	m_v3Min = m_v3Center - (vector3(fMax));
	m_v3Max = m_v3Center + (vector3(fMax));

	currOctantCount++;
	
	ConstructTree(maxLevel);
}
//Constructor to create children octants
MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	Init();

	m_v3Center = a_v3Center;
	m_fSize = a_fSize;

	m_v3Min = m_v3Center - (vector3(m_fSize) / 2.0f);
	m_v3Max = m_v3Center + (vector3(m_fSize) / 2.0f);

	currOctantCount++;
}
//Copy Constructor
MyOctant::MyOctant(MyOctant const& other)
{

	currChildrenCount = other.currChildrenCount;
	m_EntityList = other.m_EntityList;
	m_fSize = other.m_fSize;
	octantID = other.octantID;
	m_pRoot = other.m_pRoot;
	m_lChild = other.m_lChild;

	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;

	

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	currLevel = other.currLevel;
	Parent = other.Parent;

	for (size_t i = 0; i < 8; i++)
	{
		m_pChild[i] = other.m_pChild[i];
	}
}

MyOctant& MyOctant::operator=(MyOctant const& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyOctant temp(other);
		Swap(temp);
	}
	return *this;
}
MyOctant * MyOctant::GetChild(uint a_nChild)
{
	if (a_nChild > 7)
		return nullptr;
	return m_pChild[a_nChild];
}
MyOctant* MyOctant::GetParent()
{
	return Parent;
}
void MyOctant::Swap(MyOctant& other)
{
	std::swap(currChildrenCount, other.currChildrenCount);
	std::swap(m_EntityList, other.m_EntityList);
	std::swap(m_fSize, other.m_fSize);
	std::swap(octantID, other.octantID);
	std::swap(m_lChild, other.m_lChild);
	std::swap(m_pRoot, other.m_pRoot);
	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Max, other.m_v3Max);
	std::swap(m_v3Min, other.m_v3Min);
	

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	std::swap(Parent, other.Parent);
	std::swap(currLevel, other.currLevel);
	for (size_t i = 0; i < 8; i++)
	{
		std::swap(m_pChild[i], other.m_pChild[i]);
	}

}
void MyOctant::ClearEntityList(void)
{
	for (uint i = 0; i < currChildrenCount; i++)
	{
		m_pChild[i]->ClearEntityList();
	}
	//Clear root entity list
	m_EntityList.clear();
}

void MyOctant::Subdivide()
{
	//If level is last return
	if (currLevel >= maxLevel)
	{
		return;
	}
	//If already subdivided return
	if (currChildrenCount != 0)
	{
		return;
	}
	//Creating 8 children to subdivide 
	currChildrenCount = 8;

	//Variables to set bounding box for the children
	float fSizeChild = m_fSize / 4.0f;
	float fSizeDimen = fSizeChild * 2.0f;
	vector3 center;

	//Bottom Left Back
	center = m_v3Center;
	center.x -= fSizeChild;
	center.y -= fSizeChild;
	center.z -= fSizeChild;
	m_pChild[0] = new MyOctant(center,fSizeDimen);

	//Bottom Right Back
	center.x += fSizeDimen;
	m_pChild[1] = new MyOctant(center, fSizeDimen);

	//Bottom Right Front
	center.z += fSizeDimen;
	m_pChild[2] = new MyOctant(center, fSizeDimen);

	//Bottom Left Front
	center.x -= fSizeDimen;
	m_pChild[3] = new MyOctant(center, fSizeDimen);


	//Top Left Front
	center.y += fSizeDimen;
	m_pChild[4] = new MyOctant(center, fSizeDimen);

	//Top Left Back
	center.z -= fSizeDimen;
	m_pChild[5] = new MyOctant(center, fSizeDimen);

	//Top Right Back
	center.x += fSizeDimen;
	m_pChild[6] = new MyOctant(center, fSizeDimen);

	//Top Right Front
	center.z += fSizeDimen;
	m_pChild[7] = new MyOctant(center, fSizeDimen);
	
	for (uint i = 0; i < 8; i++)
	{
		//Setting the child's root to equal the main root
		m_pChild[i]->m_pRoot = m_pRoot;
		//Setting the child's parent to the octree that created it
		m_pChild[i]->Parent = this;
		//Increment a level
		m_pChild[i]->currLevel = currLevel + 1;
		//Only Subdivide if there is more entities in the octree than idea
		if (m_pChild[i]->ContainsMoreThan(m_uIdealEntityCount))
		{
			m_pChild[i]->Subdivide();
		}
		
	}
	
}

bool MyOctant::isLeaf()
{
	return false;
}

bool MyOctant::ContainsMoreThan(uint a_nEntities)
{
	uint collCount = 0;

	uint entityCount = m_pEntityMngr->GetEntityCount();

	for (size_t i = 0; i < entityCount; i++)
	{
		if (isColliding(i))
		{
			collCount++;
		}
		if (collCount > a_nEntities)
		{
			return true;
		}
	}
	return false;

}

void MyOctant::KillBranches(void)
{
	for (size_t i = 0; i < currChildrenCount; i++)
	{
		//Calls recursively to kill its children
		m_pChild[i]->KillBranches();
		//Kills itself
		delete m_pChild[i];
		m_pChild[i] = nullptr;
	}
	currChildrenCount == 0;
}

void MyOctant::ConstructTree(uint a_nMaxLevel)
{
	//Prevent construction of another tree in different levels than root
	if (currLevel != 0)
		return;

	maxLevel = a_nMaxLevel;

	
	currOctantCount = 1;

	//Delete Any leftover data
	m_EntityList.clear();

	KillBranches();

	m_lChild.clear();

	//Start the creation of more octants
	if (ContainsMoreThan(m_uIdealEntityCount))
	{
		Subdivide();
	}
	//Assign ID to the entities in the octant
	AssignIDtoEntity();
	std::cout << "Construct" << std::endl;
	ConstructList();
}

void MyOctant::AssignIDtoEntity(void)
{
	//Recursively assign ID to the children's children
	for (size_t i = 0; i < currChildrenCount; i++)
	{
		m_pChild[i]->AssignIDtoEntity();
	}
	//If leaf 
	if (currChildrenCount == 0)
	{
		//Getting Entity Count
		uint entityCount = m_pEntityMngr->GetEntityCount();
		for (size_t i = 0; i < entityCount; i++)
		{
			//Check collision between other entities
			if (isColliding(i))
			{
				//If True add the entity collided
				m_EntityList.push_back(i);
				//Increment dimension
				m_pEntityMngr->AddDimension(i, octantID);
			}
		}
	}


}

//When Deleting
void MyOctant::Release(void)
{
	//For Root Octane
	if (currLevel == 0)
	{
		KillBranches();
	}
	//Clear and reset
	m_EntityList.clear();
	m_fSize = 0.0f;
	currChildrenCount = 0.0f;
	m_lChild.clear();
	
}
//Destructor
MyOctant::~MyOctant() { Release(); };
//Accessors
//Get Methods
float MyOctant::GetSize(void)
{
	return m_fSize;
}
uint MyOctant::GetOctantCount(void)
{

	return currOctantCount;
}

vector3 MyOctant::GetCenterGlobal(void)
{
	return m_v3Center;
}
vector3 MyOctant::GetMinGlobal(void)
{
	return m_v3Max;
}
vector3 MyOctant::GetMaxGlobal(void)
{
	return m_v3Min;
}

//Collision Check to check whether or not to draw more octants based on collision
bool MyOctant::isColliding(uint a_uRBIndex)
{
	uint entityCount = m_pEntityMngr->GetEntityCount();
	if (a_uRBIndex > entityCount)//If ID of object is larger than any of the ID's of the entites in the octree no collision because that means that entity is not in the same space
		return false;
	//Using AARB Collision
	//Getting the entities vectors
	MyEntity* pEntity = m_pEntityMngr->GetEntity(a_uRBIndex);
	MyRigidBody* pRigidBody = pEntity->GetRigidBody();

	vector3 v3EntityMin = pRigidBody->GetMinGlobal();
	vector3 v3EntityMax = pRigidBody->GetMaxGlobal();

	//Checking Collision

	if (m_v3Max.x < v3EntityMin.x)
	{
		return false;
	}
	if (m_v3Min.x > v3EntityMax.x)
	{
		return false;
	}

	if (m_v3Max.y < v3EntityMin.y)
	{
		return false;
	}
	if (m_v3Min.y > v3EntityMax.y)
	{
		return false;
	}
	if (m_v3Max.z < v3EntityMin.z)
	{
		return false;
	}
	if (m_v3Min.z > v3EntityMax.z)
	{
		return false;
	}
	
	return true;
}
void MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
	if (octantID == a_nIndex)
	{
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center)*glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);

		return;
	}
	
	for (size_t i = 0; i < currChildrenCount; i++)
	{
		m_pChild[i]->Display(a_nIndex);
	}
}
void MyOctant::Display(vector3 a_v3Color)
{
	for (uint i = 0; i < currChildrenCount; i++)
	{
		m_pChild[i]->Display(a_v3Color);

	}
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4,m_v3Center)*glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);

}
void MyOctant::DisplayLeafs(vector3 a_v3Color)
{
	uint leafCount = m_lChild.size();

	for (size_t i = 0; i < leafCount; i++)
	{
		m_lChild[i]->DisplayLeafs(a_v3Color);
	}
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center)*glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}
void MyOctant::ConstructList(void)
{
	for (size_t i = 0; i < currChildrenCount; i++)
	{
		m_pChild[i]->ConstructList();
	}
	
	if(m_EntityList.size()>0)
	{
		m_pRoot->m_lChild.push_back(this);
	}
}

//--- Non Standard Singleton Methods

#pragma endregion
