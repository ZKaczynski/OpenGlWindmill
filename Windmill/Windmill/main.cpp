#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// glm
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

// stb_image
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


#include "shprogram.h"
#include "cylinder.h"
#include "house.h"



const GLuint WIDTH = 1920, HEIGHT = 1080;
const unsigned int NUMBER_OF_SAILS = 8;
const unsigned int NUMBER_OF_SLICES_MECH = 4;
const unsigned int NUMBER_OF_SLICES_COG1 = 30;
const unsigned int NUMBER_OF_BUFFERS = 6;
const unsigned int NUMBER_OF_PIPES = 16;

const unsigned int SIZE_OF_VILLAGE = 80;

GLfloat village_position[SIZE_OF_VILLAGE][4];

const GLfloat SAILS_LENGH = 0.7f;
const GLfloat SAILS_WIDTH = 0.1f;

const GLfloat GRASS = 25.0f;
GLfloat WIND_SPEED = 0.5f;
const GLfloat GREY = 0.86f;


// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

void generate_village() {
	srand(glfwGetTime());
	for (int i = 0; i < SIZE_OF_VILLAGE; i++) {
		village_position[i][0] = (float)((rand() % 10) - 10);
		village_position[i][1] = (float)((rand() % 10) - 10);
		village_position[i][2] = (float)((rand() % 2) + 0.5f);
		village_position[i][3] = (float)((rand() % 2) + 0.5f);
	}

}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout <<"Key code:"<< key << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	float cameraSpeed = 15.5 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		cameraPos += cameraSpeed * cameraFront;

	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		WIND_SPEED += 0.05f;

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		WIND_SPEED -= 0.05f;
}

GLuint LoadMipmapTexture(GLuint texId, const char* fname)
{
	int width, height,numChannels;
	unsigned char* image = stbi_load(fname, &width, &height, &numChannels,0);
	if (image == nullptr)
		throw std::exception("Failed to load texture file");

	GLuint texture;
	glGenTextures(1, &texture);

	glActiveTexture(texId);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

int main()
{
	if (glfwInit() != GL_TRUE)
	{
		std::cerr << "GLFW initialization failed" << std::endl;
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	try
	{
		GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Windmill", nullptr, nullptr);
		if (window == nullptr)
			throw std::exception("GLFW window not created");
		glfwMakeContextCurrent(window);
		glfwSetKeyCallback(window, key_callback);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);


		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
			throw std::exception("GLEW Initialization failed");

		glViewport(0, 0, WIDTH, HEIGHT);
		glEnable(GL_DEPTH_TEST);

		// Build, compile and link shader program
		ShaderProgram theProgram("Windmill.vert", "Windmill.frag");

		// Set up vertex data 

		GLfloat cylinder_vertices[8 * 2 * (NUMBER_OF_SLICES_MECH + 1)];
		GLuint cylinder_indieces[4 * 3 * NUMBER_OF_SLICES_MECH];
		create_cylider_vertices(cylinder_vertices, NUMBER_OF_SLICES_MECH);
		create_cylinder_indieces(cylinder_indieces, NUMBER_OF_SLICES_MECH);

		GLfloat first_cog_vertices[8 * 2 * (NUMBER_OF_SLICES_COG1 + 1)];
		GLuint first_cog_indieces[4 * 3 * NUMBER_OF_SLICES_COG1];
		create_cylider_vertices(first_cog_vertices, NUMBER_OF_SLICES_COG1);
		create_cylinder_indieces(first_cog_indieces, NUMBER_OF_SLICES_COG1);


		GLfloat plain_vertices[]{
			0.5f, 0.0f, 0.5f,	0.13f, 0.54f, 0.13f,	0.0f, 0.0f,	//0
			0.5f, 0.0f, -0.5f,	0.13f, 0.54f, 0.13f,	0.0f, GRASS*1.0f,	//1
			-0.5f, 0.0f, -0.5f, 0.13f, 0.54f, 0.13f, GRASS*1.0f, GRASS*1.0f,	//2
			-0.5f, 0.0f, 0.5f, 0.13f, 0.54f, 0.13f, GRASS*1.0f, 0.0f,	//3
		};
		GLuint grass_indieces[]{
			0, 1, 2,
			0, 2, 3,
		};


		GLfloat sky_vertices[]{
			0.5f, 0.0f, 0.5f,	1.0f, 1.0f, 1.0f, 0.0f, 0.0f,	//0
			0.5f, 0.0f, -0.5f,	1.0f, 1.0f, 1.0f, 0.0f, 1.0f,	//1
			-0.5f, 0.0f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,	//2
			-0.5f, 0.0f, 0.5f,	1.0f, 1.0f, 1.0f, 1.0f, 0.0f,	//3
		};
		GLuint sky_indieces[]{
			0, 1, 2,
				0, 2, 3,
		};


		GLfloat vertices[] = {
			// coordinates			// color			// texture
			0.0f,  0.0f,  0.03f,	0.51f, 0.32f, 0.0f,	1.0f,  0.0f,	//0
			-SAILS_WIDTH, SAILS_LENGH, 0.03f, 0.71f, 0.61f, 0.3f, 0.0f, 0.0f,	//1
			SAILS_WIDTH, SAILS_LENGH, 0.03f, 0.5f, 0.6f, 0.0f, 0.0f, 1.0f,	//2

			0.0f, 0.0f, -0.03f,		0.5f, 0.6f, 0.0f,	0.0f, 1.0f,		//3
			-SAILS_WIDTH, SAILS_LENGH, -0.03f, 0.51f, 0.32f, 0.0f, 1.0f, 0.0f,	//4
			SAILS_WIDTH, SAILS_LENGH, -0.03f, 0.71f, 0.61f, 0.3f, 0.0f, 0.0f,		//5

		};

		GLuint indices[] = {
			0,1,2,
			3,4,5,
			1,2,4,
			4,5,2,
			0,1,3,
			3,4,1,
			0,2,3,
			3,5,2,
		};


		unsigned int VBOs[NUMBER_OF_BUFFERS], VAOs[NUMBER_OF_BUFFERS], EBOs[NUMBER_OF_BUFFERS];
		glGenVertexArrays(NUMBER_OF_BUFFERS, VAOs);
		glGenBuffers(NUMBER_OF_BUFFERS, VBOs);
		glGenBuffers(NUMBER_OF_BUFFERS, EBOs);

		// plain  setup
		// --------------------
		glBindVertexArray(VAOs[0]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(plain_vertices), plain_vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(grass_indieces), grass_indieces, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);


		// vertex geometry data
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// vertex color data
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		// vertex texture coordinates
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);


		// sails setup
		// ---------------------
		glBindVertexArray(VAOs[1]);	// note that we bind to a different VAO now
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);	// and a different VBO
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);


		// vertex geometry data
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// vertex color data
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		// vertex texture coordinates
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

		glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)

		// MILL  setup
		// --------------------
		glBindVertexArray(VAOs[2]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(mill_vertices), mill_vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mill_indieces), mill_indieces, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);


		// vertex geometry data
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// vertex color data
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		// vertex texture coordinates
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);


		// CYLINDER  setup
		// --------------------
		glBindVertexArray(VAOs[3]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cylinder_vertices), cylinder_vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[3]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cylinder_indieces), cylinder_indieces, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);


		// vertex geometry data
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// vertex color data
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		// vertex texture coordinates
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		// Set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		// Set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		// FIRST COG  setup
		// --------------------
		glBindVertexArray(VAOs[4]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[4]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(first_cog_vertices), first_cog_vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[4]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(first_cog_indieces), first_cog_indieces, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);


		// vertex geometry data
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// vertex color data
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		// vertex texture coordinates
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		// SKY  setup
		// --------------------
		glBindVertexArray(VAOs[5]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[5]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(sky_vertices), sky_vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[5]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sky_indieces), sky_indieces, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);


		// vertex geometry data
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// vertex color data
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		// vertex texture coordinates
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);


		// Set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		// Set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// prepare textures

		GLuint texture0 = LoadMipmapTexture(GL_TEXTURE0, "wood.png");
		GLuint texture1 = LoadMipmapTexture(GL_TEXTURE1, "grass.png");
		GLuint texture2 = LoadMipmapTexture(GL_TEXTURE2, "wooden_wall.png");
		GLuint texture3 = LoadMipmapTexture(GL_TEXTURE3, "stone.png");

		GLuint texture4 = LoadMipmapTexture(GL_TEXTURE4, "skyup.png");
		GLuint texture5 = LoadMipmapTexture(GL_TEXTURE5, "skyleft.png");
		GLuint texture6 = LoadMipmapTexture(GL_TEXTURE6, "skyright.png");
		GLuint texture7 = LoadMipmapTexture(GL_TEXTURE7, "skyfront.png");
		GLuint texture8 = LoadMipmapTexture(GL_TEXTURE8, "skybottom.png");

		generate_village();

		// main event loop
		while (!glfwWindowShouldClose(window)) {
			//Get time beetween frames
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
			glfwPollEvents();

			// Clear the colorbuffer
			glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			// Bind Textures using texture units
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture1);
			glUniform1i(glGetUniformLocation(theProgram.get_programID(), "Texture0"), 0);

			theProgram.Use();

			glm::mat4 view;
			glm::mat4 projection;

			view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
			GLint viewLoc = glGetUniformLocation(theProgram.get_programID(), "view");
			projection = glm::perspective(glm::radians(fov), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 200.0f);
			GLint projLoc = glGetUniformLocation(theProgram.get_programID(), "projection");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));



			glBindVertexArray(VAOs[0]);


			glm::mat4 plain;
			plain = glm::scale(plain, glm::vec3(100.0f, 1.0f, 100.0f));
			plain = glm::translate(plain, glm::vec3(0, -1.01, 0));
			GLuint transformLoc = glGetUniformLocation(theProgram.get_programID(), "transform");
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(plain));
			glDrawElements(GL_TRIANGLES, _countof(grass_indieces), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);



			glBindTexture(GL_TEXTURE_2D, texture4);

			glBindVertexArray(VAOs[5]);

			glm::mat4 sky;
			sky = glm::scale(sky, glm::vec3(100.0f, 1.0f, 100.0f));
			sky = glm::translate(sky, glm::vec3(0, 50.0, 0));

			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(sky));
			glDrawElements(GL_TRIANGLES, _countof(sky_indieces), GL_UNSIGNED_INT, 0);



			glBindTexture(GL_TEXTURE_2D, texture5);


			glm::mat4 sky2;
			sky2 = glm::rotate(sky2, -glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
			sky2 = glm::scale(sky2, glm::vec3(100.0f, 1.0f, 100.0f));


			sky2 = glm::translate(sky2, glm::vec3(0, -50.0, 0));
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(sky2));
			glDrawElements(GL_TRIANGLES, _countof(sky_indieces), GL_UNSIGNED_INT, 0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture6);
			glUniform1i(glGetUniformLocation(theProgram.get_programID(), "Texture0"), 0);


			glm::mat4 sky3;
			sky3 = glm::rotate(sky3, -glm::radians(-90.0f), glm::vec3(0.0, 0.0, 1.0));
			sky3 = glm::scale(sky3, glm::vec3(-100.0f, 1.0f, 100.0f));


			sky3 = glm::translate(sky3, glm::vec3(0, -50.0, 0));
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(sky3));
			glDrawElements(GL_TRIANGLES, _countof(sky_indieces), GL_UNSIGNED_INT, 0);


			glBindTexture(GL_TEXTURE_2D, texture7);

			glm::mat4 sky4;
			sky4 = glm::rotate(sky4, -glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
			sky4 = glm::scale(sky4, glm::vec3(-100.0f, 1.0f, 100.0f));


			sky4 = glm::translate(sky4, glm::vec3(0, -50.0, 0));
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(sky4));
			glDrawElements(GL_TRIANGLES, _countof(sky_indieces), GL_UNSIGNED_INT, 0);

			glBindTexture(GL_TEXTURE_2D, texture8);

			glm::mat4 sky5;
			sky5 = glm::rotate(sky5, -glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
			sky5 = glm::scale(sky5, glm::vec3(-100.0f, 1.0f, 100.0f));


			sky5 = glm::translate(sky5, glm::vec3(0, 50.0, 0));
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(sky5));
			glDrawElements(GL_TRIANGLES, _countof(sky_indieces), GL_UNSIGNED_INT, 0);


			glBindVertexArray(0);

			glBindTexture(GL_TEXTURE_2D, texture2);


			glm::mat4 trans;
			static GLfloat rot_angle = 0.0f;
			trans = glm::rotate(trans, -glm::radians(rot_angle), glm::vec3(0.0, 0.0, 1.0));
			rot_angle += WIND_SPEED;
			if (rot_angle >= 360.0f)
				rot_angle -= 360.0f;
			glBindVertexArray(VAOs[1]);

			for (unsigned int i = 0; i < NUMBER_OF_SAILS; i++) {
				glm::mat4 model;
				GLfloat angle = (GLfloat)i * 360.0f / (GLfloat)NUMBER_OF_SAILS;

				model = glm::rotate(model, -glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));

				model = trans * model;
				glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));
				glDrawElements(GL_TRIANGLES, _countof(indices), GL_UNSIGNED_INT, 0);

			}


			glBindVertexArray(0);

			glBindVertexArray(VAOs[2]);
			glm::mat4 house_transform;
			house_transform = glm::translate(house_transform, glm::vec3(0, -0.3, -0.55f));
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(house_transform));
			glDrawElements(GL_TRIANGLES, _countof(mill_indieces), GL_UNSIGNED_INT, 0);


			for (int i = 0; i < SIZE_OF_VILLAGE; i++) {

				glm::mat4 house_transform;
				GLfloat angle = (GLfloat)(i * 360.0f / (GLfloat)SIZE_OF_VILLAGE);
				house_transform = glm::translate(house_transform, glm::vec3(0.0f, -0.6f, 0.0f));
				house_transform = glm::rotate(house_transform, -glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
				house_transform = glm::scale(house_transform, glm::vec3(1.0f, 0.5f, 1.5f));

				house_transform = glm::translate(house_transform, glm::vec3(village_position[i][0], 0.0f, village_position[i][1]));

				glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(house_transform));
				glDrawElements(GL_TRIANGLES, _countof(mill_indieces), GL_UNSIGNED_INT, 0);

			}


			glBindVertexArray(VAOs[3]);
			glm::mat4 cylinder_transform;
			cylinder_transform = glm::scale(cylinder_transform, glm::vec3(0.06f, 0.06f, 0.62));
			cylinder_transform = glm::translate(cylinder_transform, glm::vec3(0, 0, -0.69));
			cylinder_transform = trans * cylinder_transform;
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(cylinder_transform));
			glDrawElements(GL_TRIANGLES, _countof(cylinder_indieces), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);



			glBindVertexArray(VAOs[4]);
			glm::mat4 cog_transform;
			cog_transform = glm::scale(cog_transform, glm::vec3(0.16f, 0.16f, 0.05));
			cog_transform = glm::translate(cog_transform, glm::vec3(0, 0, -9.5f));
			cog_transform = trans * cog_transform;
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(cog_transform));
			glDrawElements(GL_TRIANGLES, _countof(first_cog_indieces), GL_UNSIGNED_INT, 0);

			for (int i = 0; i < NUMBER_OF_PIPES; i++) {
				glm::mat4 pipe_transform;
				GLfloat angle = (GLfloat)i * 360.0f / (GLfloat)NUMBER_OF_PIPES;
				pipe_transform = glm::rotate(pipe_transform, -glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
				pipe_transform = glm::scale(pipe_transform, glm::vec3(0.01f, 0.01f, 0.02));
				pipe_transform = glm::translate(pipe_transform, glm::vec3(14.9, 0.0, -26.5f));
				pipe_transform = trans * pipe_transform;
				glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(pipe_transform));
				glDrawElements(GL_TRIANGLES, _countof(first_cog_indieces), GL_UNSIGNED_INT, 0);
			}


			glm::mat4 trans2;
			static GLfloat rot_angle2 = 0.0f;
			trans2 = glm::rotate(trans2, -glm::radians(rot_angle2), glm::vec3(0.0, 0.0, 1.0));

			rot_angle2 += WIND_SPEED;
			if (rot_angle2 >= 360.0f)
				rot_angle2 -= 360.0f;

			for (int i = 0; i < NUMBER_OF_PIPES; i++) {
				glm::mat4 pipe_transform;
				GLfloat angle = (GLfloat)(i * 360.0f / (GLfloat)NUMBER_OF_PIPES) - 7.5;
				pipe_transform = glm::translate(pipe_transform, glm::vec3(0.0, -0.15, -0.69f));
				pipe_transform = glm::rotate(pipe_transform, -glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				pipe_transform = glm::rotate(pipe_transform, -glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
				pipe_transform = glm::rotate(pipe_transform, -glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				pipe_transform = glm::scale(pipe_transform, glm::vec3(0.01f, 0.01f, 0.02));
				pipe_transform = pipe_transform * trans2;
				pipe_transform = glm::translate(pipe_transform, glm::vec3(14.9, 0.0, 0.0f));
				glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(pipe_transform));
				glDrawElements(GL_TRIANGLES, _countof(first_cog_indieces), GL_UNSIGNED_INT, 0);
			}


			glm::mat4 cog_transform2;
			cog_transform2 = glm::scale(cog_transform2, glm::vec3(0.16f, 0.05f, 0.16));

			cog_transform2 = glm::translate(cog_transform2, glm::vec3(0.0f, -4.15f, -4.3f));
			cog_transform2 = glm::rotate(cog_transform2, -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			cog_transform2 = cog_transform2 * trans2;

			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(cog_transform2));
			glDrawElements(GL_TRIANGLES, _countof(first_cog_indieces), GL_UNSIGNED_INT, 0);

			glm::mat4 cog_transform3;
			cog_transform3 = glm::scale(cog_transform3, glm::vec3(0.04f, 0.40f, 0.04));
			cog_transform3 = glm::translate(cog_transform3, glm::vec3(0.0f, -1.51f, -17.2f));
			cog_transform3 = glm::rotate(cog_transform3, -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			cog_transform3 = cog_transform3 * trans2;
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(cog_transform3));
			glDrawElements(GL_TRIANGLES, _countof(first_cog_indieces), GL_UNSIGNED_INT, 0);

			//---------MILL STONE-----------------------------------------------------------
			glm::mat4 stone_transform;
			stone_transform = glm::scale(stone_transform, glm::vec3(0.20f, 0.05f, 0.20));
			stone_transform = glm::translate(stone_transform, glm::vec3(0.0f, -19.0f, -3.5f));
			stone_transform = glm::rotate(stone_transform, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			stone_transform = stone_transform * trans2;
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(stone_transform));
			glBindTexture(GL_TEXTURE_2D, texture3);
			glDrawElements(GL_TRIANGLES, _countof(first_cog_indieces), GL_UNSIGNED_INT, 0);
			//-----------------------------------------------------------------------------

			glBindVertexArray(0);
			// Swap the screen buffers
			glfwSwapBuffers(window);

		}
		glDeleteVertexArrays(NUMBER_OF_BUFFERS, VAOs);
		glDeleteBuffers(NUMBER_OF_BUFFERS, VBOs);
		glDeleteBuffers(NUMBER_OF_BUFFERS, EBOs);
	}
	catch (std::exception &ex)
	{
		std::cerr << ex.what() << std::endl;
		int i;
		std::cin >> i;
	}
	glfwTerminate();

	return 0;
}
