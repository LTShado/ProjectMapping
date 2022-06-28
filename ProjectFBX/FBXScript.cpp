#include <fbxsdk.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <windows.h>
#include <glm/glm.hpp>
// inclus la definition de DWORD
#include <IntSafe.h>

#include <Vector>


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



void AddMesh(FbxNode* node, FbxNode* parent)
{

	std::vector<glm::vec3> positions[1];
	int controlPointIndex = 0;

	int nbControlPoint;

	std::cout << "mesh" << std::endl;

	FbxMesh* mesh = node->GetMesh();

	nbControlPoint = mesh->GetControlPointsCount();

	positions->resize(nbControlPoint);

	for (int i = 0; i < nbControlPoint; i++) {
		FbxVector4 position = mesh->GetControlPointAt(i);
		positions->emplace_back(glm::vec3((float)position[0], (float)position[1],
			(float)position[2]));
	}
	std::vector<bool> normals[1];
	std::vector<int> vertexes[1];
	int nbVertex = 0;
	int nbPoly = 0;
	nbPoly = mesh->GetPolygonCount();
	nbVertex = mesh->GetPolygonVertexCount();
	normals->resize(nbVertex);
	vertexes->resize(nbVertex);
	for (int i = 0; i < nbPoly; i++) {
		int p = mesh->GetPolygonSize(i);
		for (int j = 0; j < p; j++) {
			vertexes->push_back(mesh->GetPolygonVertex(i, j));
			FbxVector4 normal;
			normals->push_back(mesh->GetPolygonVertexNormal(i, j, normal));

		}
	}



}

void Display(GLFWwindow* window)
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	glClearColor(1.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// active le tri des faces
	glEnable(GL_DEPTH_TEST);
	// on peut egalement modifier la fonction de test
	//glDepthFunc(GL_LEQUAL); // par ex: en inferieur ou egal
	// active la suppression des faces arrieres
	glEnable(GL_CULL_FACE);

	// Defini le viewport en pleine fenetre
	glViewport(0, 0, width, height);
}

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
	bool status = importer->Initialize("../ironman/ironman.fbx", -1, m_fbxManager->GetIOSettings());
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

void ProcessNode(FbxNode* node, FbxNode* parent) {
	//
	// insérer TRAITEMENT DU NODE, cf plus bas
	//

	FbxNodeAttribute* att = node->GetNodeAttribute();

	if (att != NULL)
	{
		FbxNodeAttribute::EType type = att->GetAttributeType();

		switch (type)
		{
		case FbxNodeAttribute::eMesh:
			AddMesh(node, parent);
			break;

		case FbxNodeAttribute::eSkeleton:
			// illustratif, nous traiterons du cas des squelettes 
			// dans une fonction spécifique
			//AddJoint(node, parent);
			break;
			//…
		}
	}
	



	int childCount = node->GetChildCount();
	for (int i = 0; i < childCount; i++) 
	{
		FbxNode* child = node->GetChild(i);
		ProcessNode(child, node);
	}
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

	LoadFBX();

	FbxNode* root_node = m_scene->GetRootNode();

	ProcessNode(root_node, NULL);


	while (!glfwWindowShouldClose(window))
	{
		glfwSwapBuffers(window);
		Display(window);
		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;

}
