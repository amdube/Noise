//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
//
// Copyright (c) 2014 Concordia University. All rights reserved.
//

#include "FirstPersonCamera.h"
#include "EventManager.h"
#include "Renderer.h"
#include <ctime>   // for time
#include <GLM/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>



using namespace glm;

FirstPersonCamera::FirstPersonCamera(glm::vec3 position) :  Camera(), mPosition(position), mLookAt(0.0f, 0.0f, -1.0f), mHorizontalAngle(90.0f), mVerticalAngle(0.0f), mSpeed(7.0f), mAngularSpeed(2.5f)
{
	mLookAt = vec3(0.0f, 0.0f, 1.0f);
	shake = false;
}

FirstPersonCamera::FirstPersonCamera(glm::vec3 position, glm::vec3 lookat) :  Camera(), mPosition(position), mLookAt(0.0f, 0.0f, -1.0f), mHorizontalAngle(90.0f), mVerticalAngle(0.0f), mSpeed(7.0f), mAngularSpeed(2.5f)
{
	mLookAt = lookat;
	shake = false;
}


FirstPersonCamera::~FirstPersonCamera()
{
}

void FirstPersonCamera::Update(float dt)
{
	// Prevent from having the camera move only when the cursor is within the windows
	EventManager::DisableMouseCursor();


	// The Camera moves based on the User inputs
	// - You can access the mouse motion with EventManager::GetMouseMotionXY()
	// - For mapping A S D W, you can look in World.cpp for an example of accessing key states
	// - Don't forget to use dt to control the speed of the camera motion

	// Mouse motion to get the variation in angle
	mHorizontalAngle -= EventManager::GetMouseMotionX() * mAngularSpeed * dt;
	mVerticalAngle   -= EventManager::GetMouseMotionY() * mAngularSpeed * dt;

	// Clamp vertical angle to [-85, 85] degrees
	mVerticalAngle = std::max(-85.0f, std::min(85.0f, mVerticalAngle));
	if (mHorizontalAngle > 360)
	{
		mHorizontalAngle -= 360;
	}
	else if (mHorizontalAngle < -360)
	{
		mHorizontalAngle += 360;
	}

	float theta = radians(mHorizontalAngle);
	float phi = radians(mVerticalAngle);

	mLookAt = vec3(cosf(phi)*cosf(theta), sinf(phi), -cosf(phi)*sinf(theta));
	look = mLookAt;


	//implementation of shaky cam
	static bool six_pressed = 1;
	if(glfwGetKey(EventManager::GetWindow(), GLFW_KEY_6) == GLFW_PRESS && !six_pressed){
		six_pressed = 1;
		shake = !shake;
	}
	if(glfwGetKey(EventManager::GetWindow(), GLFW_KEY_6 ) == GLFW_RELEASE){
		six_pressed = 0;
	}
	if(shake){
		vec3 originalVelocityVector = look;
		vec3 randomVector(1.0f, 1.0f, 1.0f);
		vec3 firstRotationAxis = glm::cross(randomVector, look);

		float step1Angle = EventManager::GetRandomFloat(0.0f, 1 * EventManager::GetCurrentVolume());
		mat4 step1Rotation = glm::rotate(mat4(1.0f), step1Angle, firstRotationAxis);//rotation matrix along x axis
		look = vec3(step1Rotation * vec4(look, 0));

		float step2Angle = EventManager::GetRandomFloat(0.0f, 360.0f);
		mat4 step2Rotation = glm::rotate(mat4(1.0f), step2Angle, originalVelocityVector);//rotation matrix along original velocity vector
		look = vec3(step2Rotation * vec4(look, 0));
	}
	
	vec3 sideVector = glm::cross(mLookAt, vec3(0.0f, 1.0f, 0.0f));
	vec3 upVector = vec3(0.0f,1.0f,0.0f);
	glm::normalize(sideVector);

	// A S D W for motion along the camera basis vectors
	// Also check if the camera has moved, using hasMoved, which is used to calculate the view Matrix
	// for the Skybox

	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_W ) == GLFW_PRESS)
	{
		vec3 direction = normalize(glm::vec3(mLookAt.x, 0.0f, mLookAt.z));
		mPosition += direction * dt * mSpeed;
		hasMoved = true;
	}

	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_S ) == GLFW_PRESS)
	{
		vec3 direction = normalize(glm::vec3(mLookAt.x, 0.0f, mLookAt.z));
		mPosition -= direction * dt * mSpeed;
		hasMoved = true;
	}

	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_D ) == GLFW_PRESS)
	{
		mPosition += sideVector * dt * mSpeed;
		hasMoved = true;
	}

	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_A ) == GLFW_PRESS)
	{
		mPosition -= sideVector * dt * mSpeed;
		hasMoved = true;
	}

	//space and shift for up and down

	if(glfwGetKey(EventManager::GetWindow(), GLFW_KEY_SPACE) == GLFW_PRESS){
		mPosition += upVector * dt * mSpeed;
		hasMoved = true;
	}
	if(glfwGetKey(EventManager::GetWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
		mPosition -= upVector * dt * mSpeed;
		hasMoved = true;
	}

}


float GetRandomFloat(float min,float max){
	
	long seed = 1;
	long  a  = 16807;
	long  m = 2147483647;
	long  q  = 127773;
	long r =2836;
	long int lo, hi, test;
    hi = seed / q;
    lo = seed % q;
    test = a * lo - r * hi;
    if(test > 0)
      seed = test;
    else
      seed = test + m;
    double value = ((double)seed) / ((double)m);
	float fvalue = (value+0.0f);
	return min + value*(max - min);
}
glm::mat4 FirstPersonCamera::GetViewMatrix() const
{
	return glm::lookAt(	mPosition, mPosition + look, vec3(0.0f, 1.0f, 0.0f) );
}

glm::vec3 FirstPersonCamera::getPosition() const {
	return mPosition;
}
void FirstPersonCamera::setPosition(glm::vec3 position) {
	mPosition = position;
}