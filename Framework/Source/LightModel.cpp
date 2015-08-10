//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2015 Concordia University. All rights reserved.
//

#include "LightModel.h"
#include "Renderer.h"
#include "EventManager.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>


using namespace glm;

LightModel::LightModel(vec3 size) : Model()
{
	mColor = vec3(1.0f, 1.0f, 1.0f);
	vec3 vertexBuffer[] = {vec3(0.0f, 0.0f ,0.0f)};

    numOfVertices = 1;

    glGenVertexArrays(1, &mVertexArrayID);
    glGenBuffers(1, &mVertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBuffer), vertexBuffer, GL_STATIC_DRAW);
}

LightModel::~LightModel()
{
    glDeleteBuffers(1, &mVertexBufferID);
    glDeleteVertexArrays(1, &mVertexArrayID);
}


void LightModel::Update(float dt)
{
    Model::Update(dt);
	static bool r_pressed = 0;
	if(glfwGetKey(EventManager::GetWindow(), GLFW_KEY_R) == GLFW_PRESS && !r_pressed){
		r_pressed = 1;
		setColor(glm::vec3(1.0f,0.0f,0.0f));
	}
	if(glfwGetKey(EventManager::GetWindow(), GLFW_KEY_R) == GLFW_RELEASE){
		r_pressed = 0;
	}
}

void LightModel::Draw()
{
}

bool LightModel::ParseLine(const std::vector<ci_string> &token)
{
    if (token.empty())
    {
        return true;
    }
    else
    {
        return Model::ParseLine(token);
    }
}