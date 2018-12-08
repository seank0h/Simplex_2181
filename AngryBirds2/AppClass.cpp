#include "AppClass.h"
#include "MyOctant.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	//set up the ortho camera to look at the castle+ball from the x axis
	//Set the position and target of the camera
	m_pCameraMngr->SetCameraMode(CAM_ORTHO_X,0);					//Up
	m_pCameraMngr->SetPosition(vector3(50.0f, 0.0f, 0.0f), 0);
	m_pCameraMngr->SetUpward(AXIS_Y, 0);

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

	
	
	
	//Setting up our projectile
	m_pEntityMngr->AddEntity("Planets\\AngryBird.obj", "Sun");
	bird = m_pEntityMngr->GetEntity(0);
	m_pEntityMngr->UsePhysicsSolver();
	bird->SetPosition(vector3(9.0f, 0.0f, 0.0f));
	
	//Initializing beginning level
	m_pEntityMngr->CreateSmallCastle();
	//Setting mass for all the blocks
	m_pEntityMngr->SetMass(5.0f, 0);
	//Setting mass for bird separately
	bird->SetMass(10.0f);
	//Initializing switch variables
	optimizeSwitch = false;
	freeSwitch = false;
	displaySwitch = true;
	//Initial Force Value for the launch
	force = vector3(0.0f, 8.0f, 35.0f);

	//Adding Cameras
	//Projectile View
	//Camera Index:1
	m_pCameraMngr->AddCamera(vector3(0,5.0,25.0f),vector3(0,0,0),AXIS_Y);
	//Free View
	//Camera Index:2 
	m_pCameraMngr->AddCamera(vector3(2, 10.0, 0.0f), vector3(0, 0, 0), AXIS_Y);

	m_pEntityMngr->Update();

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
	
	
	//Switching Cameras
	//Bird Following
	if (cameraSwitch&&freeSwitch==false)
	{
		birdCam = m_pCameraMngr->GetCamera(1);
		birdCam->SetPosition(vector3(birdLoc.x, birdLoc.y, birdLoc.z - 30.0f));
		birdCam->SetTarget(birdLoc);
		m_pCameraMngr->SetActiveCamera(1);
	}
	//Free camera
	else if (freeSwitch&&cameraSwitch==false)
	{
		m_pCameraMngr->SetActiveCamera(2);
	}
	//Ortho X
	else
	{
		m_pCameraMngr->SetActiveCamera(0);
	}
	


	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
	//m_pEntityMngr->AddEntityToRenderList(-1, true);
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	//Making root display only when user wants it so
	if (root != nullptr && displaySwitch == true)
	{
		root->Display();
	}

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
	SafeDelete(birdCam);
	SafeDelete(orthoXCam);
	SafeDelete(root);
	//SafeDelete(orthoZCam);
	//Release MyEntityManager
	MyEntityManager::ReleaseInstance();
	m_pCameraMngr->ReleaseInstance();
	//release GUI
	ShutdownGUI();
}