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

#include "CubeModel.h"
#include "SphereModel.h"
#include "LightModel.h"
#include "Stereo.h"
#include "Plane.h"
#include "Animation.h"
#include "Billboard.h"

#include <GLFW/glfw3.h>
#include "EventManager.h"

#include "TextureLoader.h"

#include "ParticleDescriptor.h"
#include "ParticleEmitter.h"
#include "ParticleSystem.h"

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
	mCamera.push_back(new FirstPersonCamera(vec3(15.0f, 15.0f, 30.0f)));
	mCamera.push_back(new StaticCamera(vec3(3.0f, 30.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)));
	mCamera.push_back(new StaticCamera(vec3(0.5f,  0.5f, 5.0f), vec3(0.0f, 0.5f, 0.0f), vec3(0.0f, 1.0f, 0.0f)));
	mCamera.push_back(new ThirdPersonCamera());
	mCamera.push_back(new TrackCamera());
	mCamera.push_back(new TrackCameraLookatCurve());


	mCurrentCamera = 0;

	// TODO: You can play with different textures by changing the billboardTest.bmp to another texture
#if defined(PLATFORM_OSX)
	//    int billboardTextureID = TextureLoader::LoadTexture("Textures/BillboardTest.bmp");
	//int billboardTextureID = TextureLoader::LoadTexture("Textures/Particle.png");
	int billboardTextureID = TextureLoader::LoadTexture("Textures/star.png");
#else
	//    int billboardTextureID = TextureLoader::LoadTexture("../Assets/Textures/BillboardTest.bmp");
	//int billboardTextureID = TextureLoader::LoadTexture("../Assets/Textures/Particle.png");
	int billboardTextureID= TextureLoader::LoadTexture("../Assets/Textures/star.png");
#endif
	assert(billboardTextureID != 0);

	mpBillboardList = new BillboardList(2048, billboardTextureID);

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
	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_8 ) == GLFW_PRESS)
	{
		Renderer::SetShader(SHADER_PHONG);
	}

	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_Q) == GLFW_PRESS)
	{
		TriggerBeat();
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


	// Update current Camera
	mCamera[mCurrentCamera]->Update(dt);

	// Update models
	for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
	{
		(*it)->Update(dt);
	}


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

	// Get a handle for our Transformation Matrices uniform
	GLuint WorldMatrixID = glGetUniformLocation(programID, "WorldTransform");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "ViewTransform");
	GLuint ProjMatrixID = glGetUniformLocation(programID, "ProjectionTransform");

	// Get a handle for Light Attributes uniform
	GLuint LightPositionID = glGetUniformLocation(programID, "WorldLightPosition");
	GLuint LightColorID = glGetUniformLocation(programID, "lightColor");
	GLuint LightAttenuationID = glGetUniformLocation(programID, "lightAttenuation");

	// Get a handle for Material Attributes uniform
	GLuint MaterialAmbientID = glGetUniformLocation(programID, "materialAmbient");
    GLuint MaterialDiffuseID = glGetUniformLocation(programID, "materialDiffuse");
    GLuint MaterialSpecularID = glGetUniformLocation(programID, "materialSpecular");
    GLuint MaterialExponentID = glGetUniformLocation(programID, "materialExponent");

	// Send our transformation to the currently bound shader, 
	// in the "World / View / Projection" uniform
	glUniformMatrix4fv(ViewMatrixID,  1, GL_FALSE, &GetCurrentCamera()->GetViewMatrix()[0][0]);
	glUniformMatrix4fv(ProjMatrixID,  1, GL_FALSE, &GetCurrentCamera()->GetProjectionMatrix()[0][0]);

	// Draw the Vertex Buffer
	// Note this draws a unit Sphere
	// The Model View Projection transforms are computed in the Vertex Shader

	// Set shader constants
    glUniform1f(MaterialAmbientID, ka);
    glUniform1f(MaterialDiffuseID, kd);
    glUniform1f(MaterialSpecularID, ks);
    glUniform1f(MaterialExponentID, n);
        
	glUniform4f(LightPositionID, lightPosition.x, lightPosition.y, lightPosition.z, lightPosition.w);
	glUniform3f(LightColorID, lightColor.r, lightColor.g, lightColor.b);
	glUniform3f(LightAttenuationID, lightKc, lightKl, lightKq);

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

	//Disable DepthMask so that everything always gets draw in fron of the skybox
	glDepthMask(GL_FALSE);

	//Draw the skybox seperatly 
	skyboxModel.Draw();

	glDepthMask (GL_TRUE);

	/*
	// Draw Path Lines
	
	// Set Shader for path lines
	unsigned int prevShader = Renderer::GetCurrentShader();
	Renderer::SetShader(SHADER_PATH_LINES);
	glUseProgram(Renderer::GetShaderProgramID());

	// Send the view projection constants to the shader
	VPMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewProjectionTransform");
	glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);

	for (vector<Animation*>::iterator it = mAnimation.begin(); it < mAnimation.end(); ++it)
	{
		mat4 VP = mCamera[mCurrentCamera]->GetViewProjectionMatrix();
		glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);

		(*it)->Draw();
	}*/

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
