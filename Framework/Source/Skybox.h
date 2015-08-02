/*
COMP 371 - Fianl Project
Author: Francois Stelluti

This header defines the skybox class, which will help create a skybox for the scene
*/

#pragma once

#include "Model.h"
#include <GL/glew.h>

class Skybox : public Model {

	private:
		unsigned int mSkyBoxVertexArrayID;
		unsigned int mSkyBoxVertexBufferID;

	public:
		Skybox();
		~Skybox();

		virtual void draw();
	    void cubeMap(const char* front, const char* back, const char* top, const char* bottom,
			const char* left, const char* right, GLuint* cubeTexture);	//Creates the cube texture
		bool cubeMapSideLoader(GLuint texture, GLenum side, const char* fileName); //Loads the texture for each side of the cube

};