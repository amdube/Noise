//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2015 Concordia University. All rights reserved.
//

#pragma once

#include "ParsingHelper.h"

#include <vector>

#include <GLM/glm.hpp>

class Animation;
class ParticleSystem;

class Model
{
public:
	Model();
	virtual ~Model();

	virtual void Update(float dt) = 0;
	virtual void Draw() = 0;

	void Load(ci_istringstream& iss);

	virtual glm::mat4 GetWorldMatrix() const;

	void SetPosition(glm::vec3 position);
	void SetScaling(glm::vec3 scaling);
	void SetRotation(glm::vec3 axis, float angleDegrees);
	void setColor(glm::vec3 color){
		mColor = color;
	}

	virtual glm::vec3 GetPosition() const;
	virtual glm::vec3 GetColor() const;
	glm::vec3 GetScaling() const		{ return mScaling; }
	glm::vec3 GetStretchVec() const		{ return mStretchVec; }
	glm::vec3 GetRotationAxis() const	{ return mRotationAxis; }
	int	GetNumerator() const			{ return rangeNumerator; }
	int	GetDenominator() const			{ return rangeDenominator; }
	float     GetRotationAngle() const	{ return mRotationAngleInDegrees; }
    ci_string GetName()                 { return mName; }

protected:
	virtual bool ParseLine(const std::vector<ci_string> &token) = 0;

	ci_string mName; // The model name is mainly for debugging
	glm::vec3 mPosition;
	glm::vec3 mColor;
	glm::vec3 mScaling;
	glm::vec3 mRotationAxis;
	glm::vec3 mStretchVec;
	int rangeNumerator;
	int rangeDenominator;
	float     mRotationAngleInDegrees;

    // Makes the model follow a list of Animation Keys so it's world transform changes over time
    Animation* mAnimation;
	ParticleSystem* mParticleSystem;

	friend class Animation;
};
