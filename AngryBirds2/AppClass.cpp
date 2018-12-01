#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUpward(
		vector3(0.0f, 5.0f, 25.0f), //Position
		vector3(0.0f, 0.0f, 0.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

	
	m_pModel = new Model();
	m_pModel->Load("Minecraft\\Steve.obj");
	
	//m_pEntityMngr->AddEntity("Minecraft\\Steve.obj", "Steve_" + std::to_string(m_pEntityMngr->GetEntityCount()));

	m_pEntityMngr->AddEntity("Planets\\00_Sun.obj", "Sun");
	bird = m_pEntityMngr->GetEntity(0);
	m_pEntityMngr->UsePhysicsSolver();
	bird->SetMass(100.0f);
	m_pEntityMngr->CreateSmallCastle();
	//m_pEntityMngr->CreateMediumCastle();
	//m_pEntityMngr->CreateLargeCastle();
	

	m_pEntityMngr->SetMass(5.0f, 0);
	force = vector3(0.0f, 10.0f, -25.0f);

	//m_pMeshMngr->AddPlaneToRenderList
	//Ground
	//MyMesh m_pPlane = new MyMesh();

	//m_pMeshMngr->GeneratePlane(100.0f, vector3(2.0f));
	

	//m_pMeshMngr->AddPlaneToRenderList(IDENTITY_M4, C_WHITE);
	/*
	for (int i = 0; i < 100; i++)
	{
		m_pEntityMngr->AddEntity("Minecraft\\Cube.obj", "Cube_" + std::to_string(i));
		vector3 v3Position = vector3(glm::sphericalRand(12.0f));
		v3Position.y = 0.0f;
		matrix4 m4Position = glm::translate(v3Position);
		m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(2.0f)));
		m_pEntityMngr->UsePhysicsSolver();
		//m_pEntityMngr->SetMass(2);

		//m_pEntityMngr->SetMass(i+1);
	}
	*/
	cameraIndex = m_pCameraMngr->AddCamera(vector3(0,5.0,25.0f),vector3(0,0,0),AXIS_Y);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Update Entity Manager
	m_pEntityMngr->Update();
	birdLoc = bird->GetPosition();
	
	//Set the model matrix for the main object
	//m_pEntityMngr->SetModelMatrix(m_m4Steve, "Steve");

	//Add objects to render list
	if (cameraSwitch)
	{
		birdCam = m_pCameraMngr->GetCamera(cameraIndex);
		birdCam->SetPosition(vector3(birdLoc.x, birdLoc.y, birdLoc.z - 30.0f));
		birdCam->SetTarget(birdLoc);
		m_pCameraMngr->SetActiveCamera(cameraIndex);
	}
	else
	{
		m_pCameraMngr->SetActiveCamera(0);
	}
	m_pEntityMngr->AddEntityToRenderList(-1, true);
	//m_pEntityMngr->AddEntityToRenderList(-1, true);
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

	//draw gui,
	DrawGUI();

	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	SafeDelete(m_pModel);

	//Release MyEntityManager
	MyEntityManager::ReleaseInstance();

	//release GUI
	ShutdownGUI();
}