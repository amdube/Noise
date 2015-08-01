//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2015 Concordia University. All rights reserved.
//

#include "EventManager.h"
#include "Renderer.h"
#include "Sound.h"

#include <GLFW/glfw3.h>
#include "fmod.hpp"
#include "fmod_errors.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;


// Time
double EventManager::sLastFrameTime = glfwGetTime();
float  EventManager::sFrameTime = 0.0f;

// Mouse
double EventManager::sLastMousePositionX = 0.0f;
float  EventManager::sMouseDeltaX = 0.0f;
double EventManager::sLastMousePositionY = 0.0f;
float  EventManager::sMouseDeltaY = 0.0f;

// Window
GLFWwindow* EventManager::spWindow = nullptr;

// Sound
Sound sound;
FMOD_DSP_PARAMETER_FFT *fftParameter;
float val;
unsigned int len;
char s[256];
int windowsize = 2048;
float *specLeft = new float[windowsize / 2];
float *specRight = new float[windowsize / 2];
float *spec = new float[windowsize / 2];
float smoothedVolume = 0;
float currentVolume = 0;



void EventManager::Initialize()
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		exit(-1);
	}
		
	glfwWindowHint(GLFW_SAMPLES, 4);
    
#if defined(PLATFORM_OSX)
    // OS X would use legacy opengl by default, and wouldn't be able to load shaders
    // This is the proper way to setup GLFW to use modern OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
#else
    // Allow older laptops to run the framework, even though, our framework
    // is compatible with OpenGL 3.3 (which removed many deprecated features)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif
    
    
	// Open a window and create its OpenGL context
	glfwWindowHint(GLFW_RESIZABLE, 0);
	spWindow = glfwCreateWindow(1024, 768, "NOISE", nullptr, nullptr);

	if (spWindow == nullptr)
	{
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		exit(-1);
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(spWindow, GLFW_STICKY_KEYS, GL_TRUE);

	// Initial mouse position
	glfwPollEvents();
	double x, y;
	glfwGetCursorPos(spWindow, &x, &y);

	sLastMousePositionX = x;
	sLastMousePositionY = y;

	// Initial time
	sLastFrameTime = glfwGetTime();

	//initialize sound
	sound = Sound();
	sound.initialize();
	sound.playSong("../Assets/All You're Waiting For.mp3");
}

void EventManager::Shutdown()
{
	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	spWindow = nullptr;

	/*sound.shutdown();*/
}

void EventManager::Update()
{
	// Update inputs / events
	glfwPollEvents();

	// Update mouse position
	double x, y;
	glfwGetCursorPos(spWindow, &x, &y);
	sMouseDeltaX = static_cast<float>( x - sLastMousePositionX );
	sMouseDeltaY = static_cast<float>( y - sLastMousePositionY );
	sLastMousePositionX = x;
	sLastMousePositionY = y;

	// Update frame time
	double currentTime = glfwGetTime();
	sFrameTime = static_cast<float>(currentTime - sLastFrameTime);
	sLastFrameTime = currentTime;

	// update sound
	sound.update();

	sound.fft->setParameterInt(FMOD_DSP_FFT_WINDOWSIZE, windowsize);
	sound.fft->getParameterFloat(FMOD_DSP_FFT_DOMINANT_FREQ, &val, 0, 0);
	sound.fft->getParameterData(FMOD_DSP_FFT_SPECTRUMDATA, (void **)&fftParameter, &len, s, 256);
	float globalVolume = 0;
	for (int i = 0; i < windowsize / 2; i++){
		specLeft[i] = fftParameter->spectrum[0][i];
		specRight[i] = fftParameter->spectrum[1][i];
		spec[i] = specLeft[i] + specRight[i] / 2;
		globalVolume += spec[i];
	}
	globalVolume = (globalVolume / 1024) * 500;//do the average volume then multiply by 1000 because its too small
	smoothedVolume += (globalVolume - smoothedVolume) * 0.1;
	currentVolume = smoothedVolume;


	//THE FOLLOWING LINES WILL NORMALIZE THE SPECTRUM VALUES
	/*auto maxIterator = std::max_element(&spec[0], &spec[windowsize / 2]);
	float maxVol = *maxIterator;

	// Normalize
	if (maxVol != 0)
		std::transform(&spec[0], &spec[windowsize / 2], &spec[0], [maxVol](float dB) -> float { return dB / maxVol; });*/
}

float EventManager::GetFrameTime()
{
	return sFrameTime;
}

bool EventManager::ExitRequested()
{
	return glfwGetKey(spWindow, GLFW_KEY_ESCAPE ) == GLFW_PRESS || glfwWindowShouldClose(spWindow);
}

GLFWwindow* EventManager::GetWindow()
{
	return spWindow;
}

float EventManager::GetMouseMotionX()
{
	return sMouseDeltaX;
}

float EventManager::GetMouseMotionY()
{
	return sMouseDeltaY;
}
float EventManager::GetCurrentVolume(){
	return currentVolume;
}
float* EventManager::GetCurrentSpec(){
	return spec;
}
void EventManager::EnableMouseCursor()
{
	glfwSetInputMode(spWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void EventManager::DisableMouseCursor()
{
	glfwSetInputMode(spWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}