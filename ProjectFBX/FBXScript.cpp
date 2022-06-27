#include <fbxsdk.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// inclus la definition de DWORD
#include <IntSafe.h>

extern "C"
{
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

#if defined(_WIN32) && defined(_MSC_VER)
#pragma comment(lib, "glfw3dll.lib")
#pragma comment(lib, "glew32s.lib")			// glew32.lib si pas GLEW_STATIC
#pragma comment(lib, "opengl32.lib")
#elif defined(__APPLE__)
#elif defined(__linux__)
#endif

#include <iostream>

FbxManager* m_fbxManager;
FbxScene* m_scene;

void Terminate()
{
	m_scene->Destroy();
	m_fbxManager->Destroy();
}
void Initialize()
{
	m_fbxManager = FbxManager::Create();
	FbxIOSettings* ioSettings = FbxIOSettings::Create(m_fbxManager, IOSROOT);
	m_fbxManager->SetIOSettings(ioSettings);
}

void LoadFBX() {
	m_scene = FbxScene::Create(m_fbxManager, "Ma Scene");
	FbxImporter* importer = FbxImporter::Create(m_fbxManager, "");
	bool status = importer->Initialize("cube01.fbx", -1, m_fbxManager -> GetIOSettings());
	status = importer->Import(m_scene);
	importer->Destroy();
}

static void error_callback(int error, const char* description)
{
	std::cout << "Error GFLW " << error << " : " << description << std::endl;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void) {

	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Projet FBX OpenGL", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// defini la fonction de rappel utilisee lors de l'appui d'une touche
	glfwSetKeyCallback(window, key_callback);

	Initialize();

	while (!glfwWindowShouldClose(window))
	{
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;

}
