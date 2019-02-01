
//#define GLM_FORCE_RADIANS
#include <iostream>

#include <AssetManager.h>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

//#define GL3_PROTOTYPES 1
#include <renderer.h>
#include <iostream>
#include <buffer.h>
//#include <triangle.h>
#include <gishandler.h>
#include <vector>
#include <camera.h>
#include <chrono>
#include <terrain.h>
#include <gbuffer.h>
#include <framerenderer.h>


//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
using namespace std;
using namespace chrono;


float Vertices[9] = {0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0};
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//    glViewport(0, 0, width*2, height*2);
//    fr.setScreenDims(SCREEN_WIDTH*2, SCREEN_HEIGHT*2);
//}

const char * ErrorString(GLenum error)
{
	if (error == GL_INVALID_ENUM)
	{
		return "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.";
	}

	else if (error == GL_INVALID_VALUE)
	{
		return "GL_INVALID_VALUE: A numeric argument is out of range.";
	}

	else if (error == GL_INVALID_OPERATION)
	{
		return "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.";
	}

	else if (error == GL_INVALID_FRAMEBUFFER_OPERATION)
	{
		return "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.";
	}

	else if (error == GL_OUT_OF_MEMORY)
	{
		return "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.";
	}
	else
	{
		return "None";
	}
};

//Starts up SDL, creates window, and initializes OpenGL
bool init();

//Initializes matrices and clear color
bool initGL();

//Input handler
void handleKeys( GLFWwindow* window, int key, int scancode, int action, int mods );


// void HandleEvents(SDL_Event e, float dt = 0);

//Per frame update
void update(float dt = 0);

//Renders quad to the screen
void render();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
// SDL_Window* gWindow = NULL;
GLFWwindow* window;

//OpenGL context
// SDL_GLContext gContext;

//Render flag
bool gRenderQuad = true;

camera Camera;

framerenderer fr;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    fr.setScreenDims(width, height);
}

string fname = "../DATA/grandcanyon_ar.tif";
terrain Terrain(fname);

GLuint VaoId;

high_resolution_clock::time_point current;
duration<double> time_span;

float lastFrame;
float deltaTime;


bool quit;

int main(int argc, char** argv)
{
	renderer Renderer = renderer();

	string appPath = argv[0];
	cout << argv[0] << endl;
	appPath.erase(appPath.end() - 3, appPath.end());
	// Lets set the application path for this guy
	AssetManager::SetAppPath(appPath);


//	current = high_resolution_clock::now();
	high_resolution_clock::time_point past = high_resolution_clock::now();

	//Start up SDL and create window
	if ( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		cout << "INITIALIZED" << endl;
		// Terrain.SetFile(AssetManager::GetAppPath() + "../../data/drycreek.tif");
		Terrain.setup();
		//Main loop flag
		quit = false;

		//Event handler
		// SDL_Event e;

		//Enable text input
		// SDL_StartTextInput();

		//While application is running

		//init time frames
		lastFrame = 0.0f;
		deltaTime = 0.0f;
		
		while ( !glfwWindowShouldClose(window) )
		{
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			current = high_resolution_clock::now();
			// // duration<double> time_span = duration_cast<duration<double>>(current - past);
			time_span = duration_cast<duration<double>>(current - past);
			// //Handle events on queue

			if (time_span.count() <= 1.0 / 30.0)
			{
				continue;
			}
			// past = current;
			//cout << time_span.count();
			// while ( SDL_PollEvent( &e ) != 0 )
			// {
			// 	HandleEvents(e, time_span.count());
			// }

//            std::cout << "Counts: " << time_span.count() << "\n";
			processInput(window);



			// Update first
			update();

			// Now render
			render();

			auto error = glGetError();
			if ( error != GL_NO_ERROR )
			{
				cout << "RENDER STAGE: Error initializing OpenGL! " << ErrorString( error )  << endl;

			}

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
			past = current;
		}

		//Disable text input
		// SDL_StopTextInput();
	}

	Renderer.cleanup();

	//Free resources and close SDL
	close();

	return 0;
}

bool init()
{

	//Initialization flag
	bool success = true;

	//Initialize SDL
	if ( !glfwInit() )
	{
		// printf( "GLFW could not initialize! SDL Error: %s\n", SDL_GetError() );
		printf( "GLFW could not initialize! GLFW Error: " );
		success = false;
	}
	else
	{
		//Use OpenGL 3.3 -- Make sure you have a graphics card that supports 3.3
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		//Create window
		window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GeoSim",NULL, NULL);
		if ( window == NULL )
		{
			// printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			printf( "Window could not be created!");
			success = false;
		}
		else
		{
			//Create context
			glfwMakeContextCurrent(window);

			//Registering callbacks
			glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
			glfwSetKeyCallback(window, handleKeys);

			// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			//Initialize OpenGL
			if ( !initGL() )
			{
				printf( "OUCH Unable to initialize OpenGL!\n" );
				success = false;
			}

			//cout << glGetString(GL_VERSION) << endl;
			cout << "GUFFER SUCCESS: " << GBuffer::Init(SCREEN_WIDTH, SCREEN_HEIGHT) << endl;
			fr.setup();
			fr.setScreenDims(SCREEN_WIDTH*2, SCREEN_HEIGHT*2);
		}
	}

	GLuint vao;
	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);
	return success;
}

bool initGL()
{
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		return false;
	}

	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(window,&bufferWidth,&bufferHeight);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glViewport(0,0,SCREEN_WIDTH*2,SCREEN_HEIGHT*2);
	//glViewport(0,0,bufferWidth*,bufferWidth*);

	//NOTE! For OSX with retina display, framebuffer size should be set twice as much as window size!
//	glViewport(0,0,SCR_WIDTH*2,SCR_HEIGHT*2);
	//configure global opengl state
	//glEnable(GL_DEPTH_TEST);
	//setting polygon mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	return true;
}

void update(float dt)
{
	//No per frame update needed
	Camera.update();
}

void render()
{
	fr.SetCameraPos(Camera.getPos());
	glm::mat4 view = Camera.getView();
	glm::mat4 projection = Camera.getProjection();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	// glClearColor( 0.f, 0.f, 0.5f, 0.f );
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	fr.render(view, projection);

	GBuffer::BindForWriting();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	// glClearColor( 0.f, 0.f, 0.5f, 0.f );
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	Terrain.render(view, projection);
	GBuffer::DefaultBuffer();
	//glDisable(GL_CULL_FACE);
	//glDisable(GL_DEPTH_TEST);
	return;
}

void close()
{
	// //Destroy window
	// SDL_DestroyWindow( gWindow );
	// gWindow = NULL;

	// //Quit SDL subsystems
	// SDL_Quit();
	glfwTerminate();
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}



void handleKeys( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		Camera.rotateX(1 * time_span.count());
	}
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		Camera.rotateX(-1 * time_span.count());
	}
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		Camera.strafe(-10 * time_span.count());
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		Camera.translate(-10 * time_span.count());
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		Camera.strafe(10 * time_span.count());
	}
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		Camera.translate(10 * time_span.count());
	}
	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	}
	if (key == GLFW_KEY_U && action == GLFW_PRESS)
	{
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}
	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		Camera.rotateY(-1 * time_span.count());
	}
	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		Camera.rotateY(1 * time_span.count());
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		Camera.flight(1 * time_span.count());
	}
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		Camera.flight(-1 * time_span.count());
	}
	if (key == GLFW_KEY_W && action == GLFW_RELEASE)
	{
		cout << "W RELEASED" << endl;
		Camera.resetVerticalSpeed();
	}
	if (key == GLFW_KEY_S && action == GLFW_RELEASE)
	{
		cout << "S RELEASED" << endl;
		Camera.resetVerticalSpeed();
	}
	if ((key == GLFW_KEY_A || key == GLFW_KEY_D) && action == GLFW_RELEASE)
	{
		Camera.resetHorizontalSpeed();
	}
	if ((key == GLFW_KEY_Q || key == GLFW_KEY_E) && action == GLFW_RELEASE)
	{
		Camera.resetHorizontalRotation();
	}
	if ((key == GLFW_KEY_Z || key == GLFW_KEY_X) && action == GLFW_RELEASE)
	{
		Camera.resetVerticalRotation();
	}
	if ((key == GLFW_KEY_R || key == GLFW_KEY_F) && action == GLFW_RELEASE)
	{
		Camera.resetFlightSpeed();
	}
}