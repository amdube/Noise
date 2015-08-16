//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2015 Concordia University. All rights reserved.
//

#include "World.h"
#include "Renderer.h"
#include "ParsingHelper.h"
#include "Skybox.h"

#include "StaticCamera.h"
#include "FirstPersonCamera.h"
#include "ThirdPersonCamera.h"
#include "TrackCamera.h"
#include "TrackCameraLookatCurve.h"
#include "FirstPersonGravityCamera.h"

#include "Animation.h"
#include "CubeModel.h"
#include "SphereModel.h"
#include "LightModel.h"
#include "Stereo.h"
#include "Plane.h"
#include "ClippedCubeModel.h"
#include "Saw.h"
#include "CubeInitial.h"

#include <GLFW/glfw3.h>
#include "EventManager.h"

#include "TextureLoader.h"

#include "ParticleDescriptor.h"
#include "ParticleEmitter.h"
#include "ParticleSystem.h"
#include "Billboard.h"

using namespace std;
using namespace glm;

World* World::instance;

// Material Coefficients
const float ka = 0.2f;
const float kd = 0.8f;
const float ks = 0.2f;
const float n = 90.0f;

// Light Coefficients
const vec3 lightColor(1.0f, 1.0f, 1.0f);
const float lightKc = 0.0f;
const float lightKl = 0.0f;
const float lightKq = 1.0f;
const vec4 lightPosition(32.0f, 32.0f, 10.0f, 1.0f); // If w = 1.0f, we have a point light
//const vec4 lightPosition(5.0f, -5.0f, 5.0f, 0.0f); // If w = 0.0f, we have a directional light

World::World()
{
    instance = this;

	// Setup Camera
	mCamera.push_back(new TrackCameraLookatCurve());
	mCamera.push_back(new ThirdPersonCamera());
	mCamera.push_back(new TrackCamera());
	mCamera.push_back(new FirstPersonGravityCamera());
	mCamera.push_back(new FirstPersonCamera(vec3(0, 15.0f, 40), vec3(0,-15,-40)));


	mCurrentCamera = 0;

	// TODO: You can play with different textures by changing the billboardTest.bmp to another texture
#if defined(PLATFORM_OSX)
	//    int billboardTextureID = TextureLoader::LoadTexture("Textures/BillboardTest.bmp");
	//int billboardTextureID = TextureLoader::LoadTexture("Textures/Particle.png");
	int billboardTextureID = TextureLoader::LoadTexture("Textures/starhollow.png");
#else
	//    int billboardTextureID = TextureLoader::LoadTexture("../Assets/Textures/BillboardTest.bmp");
	//int billboardTextureID = TextureLoader::LoadTexture("../Assets/Textures/Particle.png");
	int billboardTextureID= TextureLoader::LoadTexture("../Assets/Textures/starhollow.png");
#endif
	assert(billboardTextureID != 0);

	mpBillboardList = new BillboardList(2048, billboardTextureID);
	 
	//Make a saw and initial cube
	saw = new Saw(vec3(0.1f, 4.0f, 4.0f));
    initialCube = new CubeInitial(vec3(4.0f, 4.0f, 4.0f));
    mSaw.push_back(saw);
    mCubeInitial.push_back(initialCube);
	//Nothing is clipped yet
	clipped = 0;

}




World::~World()
{
	// Models
	for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
	{
		delete *it;
	}

	mModel.clear();

	for (vector<Animation*>::iterator it = mAnimation.begin(); it < mAnimation.end(); ++it)
	{
		delete *it;
	}

	mAnimation.clear();

	for (vector<AnimationKey*>::iterator it = mAnimationKey.begin(); it < mAnimationKey.end(); ++it)
	{
		delete *it;
	}

	mAnimationKey.clear();

	// Camera
	for (vector<Camera*>::iterator it = mCamera.begin(); it < mCamera.end(); ++it)
	{
		delete *it;
	}
	mCamera.clear();

	delete mpBillboardList;

	// Saw
	for (vector<Saw*>::iterator it = mSaw.begin(); it < mSaw.end(); ++it)
	{
		delete *it;
	}

	mSaw.clear();

	// Cube initial
	for (vector<CubeInitial*>::iterator it = mCubeInitial.begin(); it < mCubeInitial.end(); ++it)
	{
		delete *it;
	}

	mCubeInitial.clear();

	// Clipped cube
	for (vector<ClippedCubeModel*>::iterator it = mClippedCubeModel.begin(); it < mClippedCubeModel.end(); ++it)
	{
		delete *it;
	}

	mClippedCubeModel.clear();

}

World* World::GetInstance()
{
    return instance;
}

const Camera* World::GetCurrentCamera() const
{
     return mCamera[mCurrentCamera];
}
void World::TriggerBeat(){
	for (vector<ParticleSystem*>::iterator it = mParticleSystemList.begin(); it != mParticleSystemList.end(); ++it)
	{
		(*it)->EmitParticles(1);
	}
	
}
void World::AddBillboard(Billboard* b)
{
    mpBillboardList->AddBillboard(b);
}

void World::RemoveBillboard(Billboard* b)
{
    mpBillboardList->RemoveBillboard(b);
}

void World::Update(float dt, float currentVolume, float* currentSpec)
{
	// User Inputs
	// 1 2 3 4 5 to change the Camera
	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_1 ) == GLFW_PRESS)
	{
		mCurrentCamera = 0;
	}
	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_2 ) == GLFW_PRESS)
	{
		if (mCamera.size() > 1)
		{
			mCurrentCamera = 1;
		}
	}
	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_3 ) == GLFW_PRESS)
	{
		if (mCamera.size() > 2)
		{
			mCurrentCamera = 2;
		}
	}
	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_4 ) == GLFW_PRESS)
	{
		if (mCamera.size() > 3)
		{
			mCurrentCamera = 3;
		}
	}
	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_5 ) == GLFW_PRESS)
	{
		if (mCamera.size() > 4)
		{
			mCurrentCamera = 4;
		}
	}
	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_6 ) == GLFW_PRESS)
	{
		if (mCamera.size() > 5)
		{
			mCurrentCamera = 5;
		}
	}


	if(glfwGetKey(EventManager::GetWindow(), GLFW_KEY_T) == GLFW_PRESS){
		Model * light = this->FindModel("\"Light1\"");
	}

	// Spacebar to change the shader
	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_0 ) == GLFW_PRESS)
	{
		Renderer::SetShader(SHADER_SOLID_COLOR);
	}
	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_9 ) == GLFW_PRESS)
	{
		Renderer::SetShader(SHADER_FLAT);
	}

	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_Q) == GLFW_PRESS)
	{
		TriggerBeat();
	}
	
	

	// X to clip the cube 
	// If there's already two clipped cubes, it will delete them and make new ones
	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_X ) == GLFW_PRESS) {
		// Plane rotation around origin
		// You can adjust the x,y,z of plane

		if(mClippedCubeModel.size() >= 1 ){
			//Erase current saw and clipped cubes and initial cubes.
			mClippedCubeModel.erase(mClippedCubeModel.begin(),mClippedCubeModel.end());
			mSaw.erase(mSaw.begin(),mSaw.end());
			mCubeInitial.erase(mCubeInitial.begin(),mCubeInitial.end());
			
			//Make new saw and initial cube
			saw = new Saw(vec3(0.1f, 4.0f, 4.0f));
			initialCube = new CubeInitial(vec3(4.0f, 4.0f, 4.0f));
			mSaw.push_back(saw);
			mCubeInitial.push_back(initialCube);
			clipped = 0;
		} else {
			//Erase current saw and clipped cubes and initial cubes.
			mClippedCubeModel.erase(mClippedCubeModel.begin(),mClippedCubeModel.end());
			mSaw.erase(mSaw.begin(),mSaw.end());
			mCubeInitial.erase(mCubeInitial.begin(),mCubeInitial.end());
			
			//Make new saw and initial cube
			saw = new Saw(vec3(0.1f, 4.0f, 4.0f));
			initialCube = new CubeInitial(vec3(4.0f, 4.0f, 4.0f));
			mSaw.push_back(saw);
			mCubeInitial.push_back(initialCube);
			clipped = 0;
		}
	}

	//Saw clips cubes
	distOfCubes = glm::distance(saw->GetPosition(), initialCube->GetPosition());
	clippingRadius = initialCube->GetSize().x;
    if (distOfCubes <= clippingRadius && !clipped){
		
		//Get size of initial cube
		vec3 initialCubeSize = initialCube->GetSize();
		//Get position of initial cube
		vec3 initialPosition = initialCube->GetPosition();
		//Get rotation and angle of initial cube
		vec3 initialRotationAxis = initialCube->GetRotationAxis();
		float initialRotationAngle = initialCube->GetRotationAngle();

        mCubeInitial.erase(mCubeInitial.begin(), mCubeInitial.end());
        vec4 clippingPlane(1.0f, 1.0f, 0.0f, 0.0f);
        mClippedCubeModel.push_back(new ClippedCubeModel(clippingPlane, initialCubeSize, initialPosition, initialRotationAxis, initialRotationAngle, false));
        mClippedCubeModel.push_back(new ClippedCubeModel(-clippingPlane, initialCubeSize, initialPosition, initialRotationAxis, initialRotationAngle, true));
        clipped =1;
    }

	//Remove saw and clipped cubes when they are far away
	distOfSaw = glm::distance(saw->GetPosition(), vec3(0,0,0));
	if (distOfSaw >= 60.0f){
		mSaw.erase(mSaw.begin(), mSaw.end());
		mClippedCubeModel.erase(mClippedCubeModel.begin(),mClippedCubeModel.end());
	}

    // Update animation and keys
    for (vector<Animation*>::iterator it = mAnimation.begin(); it < mAnimation.end(); ++it)
    {
        (*it)->Update(dt, currentVolume, currentSpec);
    }
    
    for (vector<AnimationKey*>::iterator it = mAnimationKey.begin(); it < mAnimationKey.end(); ++it)
    {
        (*it)->Update(dt);
    }
	skyboxModel.Update(dt);

	// Update current Camera
	mCamera[mCurrentCamera]->Update(dt);

	// Update models
	for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
	{
		(*it)->Update(dt);
	}

	//Update clipped cubes
	for (vector<ClippedCubeModel*>::iterator it = mClippedCubeModel.begin(); it < mClippedCubeModel.end(); ++it){
		(*it)->Update(dt);
	}

	 // Update saw
    //if( clipped == 0){
        for (vector<Saw*>::iterator it = mSaw.begin(); it < mSaw.end(); ++it)
        {
            (*it)->Update(dt);
        }
    //}
    
    // Update initial cube
   // if (clipped == 0){
        for (vector<CubeInitial*>::iterator it = mCubeInitial.begin(); it < mCubeInitial.end(); ++it)
        {
            (*it)->Update(dt);
        }
   // }


    // Update billboards
    for (vector<ParticleSystem*>::iterator it = mParticleSystemList.begin(); it != mParticleSystemList.end(); ++it)
    {
        (*it)->Update(dt);
    }
    
    mpBillboardList->Update(dt);

}
void World::Draw()
{
	Renderer::BeginFrame();

	// Set shader to use
	glUseProgram(Renderer::GetShaderProgramID());

	GLuint programID = Renderer::GetShaderProgramID();

	// This looks for the MVP Uniform variable in the Vertex Program
	GLuint VPMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewProjectionTransform");
	glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &mCamera[mCurrentCamera]->GetViewProjectionMatrix()[0][0]);

	GLuint LightLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "LightLocation");
	glUniform3fv(LightLocation, 1, &FindModel("\"Light1\"")->GetPosition()[0]);

	GLuint LightColor = glGetUniformLocation(Renderer::GetShaderProgramID(), "LightColor");
	glUniform3fv(LightColor, 1, &FindModel("\"Light1\"")->GetColor()[0]);

	// Draw models
	for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
	{
		(*it)->Draw();
	}

	 // Draw clipped models
    for (vector<ClippedCubeModel*>::iterator it = mClippedCubeModel.begin(); it < mClippedCubeModel.end(); ++it)
    {
        (*it)->Draw();
    }
    
    
    // Draw clipping model
    //if (clipped == 0){
        for (vector<Saw*>::iterator it = mSaw.begin(); it < mSaw.end(); ++it)
        {   
            (*it)->Draw();
        }
    //}
    
    // Draw initial
   // if (clipped == 0){
        for (vector<CubeInitial*>::iterator it = mCubeInitial.begin(); it < mCubeInitial.end(); ++it)
        {  
            (*it)->Draw();
        }
   // }

	//Disable DepthMask so that everything always gets draw in front of the skybox
	glDepthMask(GL_FALSE);

	//Draw the skybox seperatly 
	skyboxModel.Draw();

	//Enable the depthMask
	glDepthMask (GL_TRUE);
	
	for (vector<ClippedCubeModel*>::iterator it = mClippedCubeModel.begin(); it < mClippedCubeModel.end(); ++it)
	 { 
       (*it)->Draw();
	 }


	for (vector<AnimationKey*>::iterator it = mAnimationKey.begin(); it < mAnimationKey.end(); ++it)
	{
		(*it)->Draw();
	}

	Renderer::CheckForErrors();
    
    // Draw Billboards
    mpBillboardList->Draw();


	/*// Restore previous shader
	Renderer::SetShader((ShaderType) prevShader);*/

	Renderer::EndFrame();
}

void World::LoadScene(const char * scene_path)
{
	// Using case-insensitive strings and streams for easier parsing
	ci_ifstream input;
	input.open(scene_path, ios::in);

	// Invalid file
	if(input.fail() )
	{	 
		fprintf(stderr, "Error loading file: %s\n", scene_path);
		getchar();
		exit(-1);
	}

	ci_string item;
	while( std::getline( input, item, '[' ) )   
	{
        ci_istringstream iss( item );

		ci_string result;
		if( std::getline( iss, result, ']') )
		{
			if( result == "cube" )
			{
				// Box attributes
				CubeModel* cube = new CubeModel();
				cube->Load(iss);
				mModel.push_back(cube);

			}
            else if( result == "sphere" )
            {
                SphereModel* sphere = new SphereModel();
                sphere->Load(iss);
                mModel.push_back(sphere);

            }
			else if (result == "plane"){
				Plane* plane = new Plane(96);
				plane->Load(iss);
				mModel.push_back(plane);
			}
			else if ( result == "animationkey" )
			{
				AnimationKey* key = new AnimationKey();
				key->Load(iss);
				mAnimationKey.push_back(key);
			}
			else if (result == "animation")
			{
				Animation* anim = new Animation();
				anim->Load(iss);
				mAnimation.push_back(anim);
			}
			else if(result == "light")
			{
				LightModel* light = new LightModel();
				light->Load(iss);
				mModel.push_back(light);
			}
			else if(result == "stereo")
			{
				Stereo* stereo = new Stereo();
				stereo->Load(iss);
				mModel.push_back(stereo);
			}
		    else if( result == "skybox1" ) 
			{
				//Skybox attributes
				Skybox* skybox = new Skybox();
				skybox->Load(iss);
				//Set to skybox attribute
				skyboxModel = *skybox;
			} 
			else if ( result.empty() == false && result[0] == '#')
			{
				// this is a comment line
			}
			else
			{
				fprintf(stderr, "Error loading scene file... !");
				getchar();
				exit(-1);
			}
	    }
	}
	input.close();

	/* Set Animation vertex buffers
	for (vector<Animation*>::iterator it = mAnimation.begin(); it < mAnimation.end(); ++it)
	{
		// Draw model
		(*it)->CreateVertexBuffer();
	}*/
}

std::vector<Model*>* World::getAllModels(){
	return &mModel;
}


Animation* World::FindAnimation(ci_string animName)
{
    for(std::vector<Animation*>::iterator it = mAnimation.begin(); it < mAnimation.end(); ++it)
    {
        if((*it)->GetName() == animName)
        {
            return *it;
        }
    }
    return nullptr;
}

Model* World::FindModel(ci_string modelName)
{
    for(std::vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
    {
        if((*it)->GetName() == modelName)
        {
            return *it;
        }
    }
    return nullptr;
}

AnimationKey* World::FindAnimationKey(ci_string keyName)
{
    for(std::vector<AnimationKey*>::iterator it = mAnimationKey.begin(); it < mAnimationKey.end(); ++it)
    {
        if((*it)->GetName() == keyName)
        {
            return *it;
        }
    }
    return nullptr;
}

void World::AddParticleSystem(ParticleSystem* particleSystem)
{
	mParticleSystemList.push_back(particleSystem);
}

void World::RemoveParticleSystem(ParticleSystem* particleSystem)
{
	vector<ParticleSystem*>::iterator it = std::find(mParticleSystemList.begin(), mParticleSystemList.end(), particleSystem);
	mParticleSystemList.erase(it);
}
