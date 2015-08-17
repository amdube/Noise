//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
//
// Copyright (c) 2014 Concordia University. All rights reserved.
//

#pragma once

#include "Camera.h"

class FirstPersonCamera : public Camera
{
public:
	FirstPersonCamera(glm::vec3 position);
	FirstPersonCamera::FirstPersonCamera(glm::vec3 position, glm::vec3 lookat);
	virtual ~FirstPersonCamera();

	virtual void Update(float dt);

	virtual glm::mat4 GetViewMatrix() const;

protected:
	glm::vec3 getPosition() const;
	void setPosition(glm::vec3);
	void setSpeed(float speed){
		mSpeed = speed;
	}
	void setAngularSpeed(float angularSpeed){
		mAngularSpeed = angularSpeed;
	}

private:
	glm::vec3 mPosition;	
	float mHorizontalAngle; // horizontal angle
	float mVerticalAngle;   // vertical angle

	float mSpeed;			// World units per second
	float mAngularSpeed;    // Degrees per pixel

	bool shake;

	glm::vec3 mLookAt;
	glm::vec3 look;
};
