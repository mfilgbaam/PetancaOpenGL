// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <vector>
#include <cmath>

// Include GLEW
#include "dependente\glew\glew.h"

// Include GLFW
#include "dependente\glfw\glfw3.h"

// Include GLM
#include "dependente\glm\glm.hpp"
#include "dependente\glm\gtc\matrix_transform.hpp"
#include "dependente\glm\gtc\type_ptr.hpp"

#include "shader.hpp"

#include "Camera/camera.h"
#include "sphere/src/Sphere.h"
#include "Ball.h"

//variables
GLFWwindow* window;
const int width = 1024, height = 1024;


//Time
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

//Camera parameters
glm::vec3 cameraPos = glm::vec3(-190.0f, 50.0f, 0.0f);
glm::vec3 cameraDir = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::normalize(glm::cross(cameraUp, cameraDir));
Camera camera = Camera::Camera(cameraPos, cameraDir, cameraUp);

//Lightning
glm::vec3 lightPos(-190.0f, 50.0f, 0.0f);
glm::vec3 lightColor(0.5f);

//SPHERE
const float PI = acos(-1);

//Balls
Sphere ball = Sphere::Sphere();
std::list<Ball> BallList;
glm::vec3 jinitialpos = glm::vec3(3.0, 1.0, 0.0);
glm::vec3 binitialpos = glm::vec3(-5.0, 1.0, 0.0);
Ball *jack = new Ball(0, jinitialpos);
Ball *playball;

//Game
bool playing = false, pKey = true, end = false;
float lr_movement = 0.001f;
float terrainlimit = 2;
int ball_n = 1;

void window_callback(GLFWwindow* window, int new_width, int new_height)
{
	glViewport(0, 0, new_width, new_height);
}

void processInput(GLFWwindow* window)
{
	float cameraSpeed = 100.0f * deltaTime, minsec = 1, maxsec = 3;
	static float presstime;
	

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.translateFront(cameraSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.translateBack(cameraSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		camera.translateUp(cameraSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		if (camera.getCameraPosition().y >= 5) camera.translateDown(cameraSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.translateRight(cameraSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.translateLeft(cameraSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && !playing) {
		if (playball->GetPosition().z < (terrainlimit-0.1)) playball->AddMovement(lr_movement);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && !playing) {
		if (playball->GetPosition().z > -(terrainlimit-0.1)) playball->AddMovement(-lr_movement);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		camera.setCameraPosition(cameraPos);
	}
	//Trow the ball
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && pKey && !playing) {
		pKey = false;
		presstime = glfwGetTime();
	}
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE && !pKey && !playing) {
		pKey = true;
		float time = glfwGetTime() - presstime;
		if (time<= minsec) playball->AddMovement(glm::vec3(0.01f, 0.0f, 0.0f));
		else if (time >= maxsec) playball->AddMovement(glm::vec3(0.014f, 0.0f, 0.0f));
		else playball->AddMovement(glm::vec3(0.01f + (time-1)/2 * 0.004, 0.0f, 0.0f));
		playing = true;

	}
	//Reset the game
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		BallList.clear();
		BallList.push_back(Ball::Ball(0, jinitialpos));
		jack = &BallList.back();
		BallList.push_back(Ball::Ball(1, binitialpos));
		playball = &BallList.back();
		playing = false;
		end = false;
		ball_n = 1;

	}

}


glm::mat4 Adjust(glm::mat4 trans, Ball b) {
	trans = glm::scale(trans, glm::vec3(0.05f, 0.05f, 0.05f));
	trans = glm::translate(trans, glm::vec3(0.0f, -20.0f + b.GetHitbox()*100, 0.0f));
	trans = glm::scale(trans, glm::vec3(1.0f * b.GetHitbox()* 100, 1.0f * b.GetHitbox()* 100, 1.0f * b.GetHitbox()* 100));
	return trans;
}

//Calculate distances between objects
float Distance(glm::vec3 obj1, glm::vec3 obj2) {
	return pow((obj1.x - obj2.x), 2) + pow((obj1.y - obj2.y), 2) + pow((obj1.z - obj2.z), 2);
}

//Calculate nearest ball to the camera
bool Nearest_Ball(Ball b1, Ball b2) {
	return (Distance(b1.GetPosition(), camera.getCameraPosition()) > Distance(b2.GetPosition(), camera.getCameraPosition()));
}

//Chack if a ball is moving
bool Static(Ball b) {
	return (abs(b.GetMovement().x) < b.GetDeceleration() && abs(b.GetMovement().z) < b.GetDeceleration());
}
//Check if two balls collide
bool Collide(Ball b1, Ball b2) {
	return Distance(b1.GetPosition(), b2.GetPosition()) <= (b1.GetHitbox() + b2.GetHitbox());
}
//Manage the collisions of the balls
void ManageCollision(Ball *b1, Ball *b2) {

	if (Static(*b1) && Static(*b2)) return;

	glm::vec3 normalvector = glm::vec3(b1->GetPosition().x - b2->GetPosition().x, 0.0f, b1->GetPosition().z - b2->GetPosition().z);
	glm::vec3 normal = normalvector / (float)sqrt(pow(normalvector.x , 2) + pow(normalvector.z, 2));
	glm::vec3 tangent = glm::vec3(-normal.z, 0.0f, normal.x);

	float b1n = glm::dot(b1->GetMovement(), normal);
	float b1t = glm::dot(b1->GetMovement(), tangent);
	float b2n = glm::dot(b2->GetMovement(), normal);
	float b2t = glm::dot(b2->GetMovement(), tangent);

	float b1newnormal = (b1n * (b1->GetMass() - b2->GetMass()) + 2 * b1->GetMass() * b2n) / (b1->GetMass() + b2->GetMass());
	float b2newnormal = (b2n * (b2->GetMass() - b1->GetMass()) + 2 * b2->GetMass() * b1n) / (b1->GetMass() + b2->GetMass());

	

	glm::vec3 b1normal = b1newnormal * normal;
	glm::vec3 b1tangent = b1t * tangent;
	glm::vec3 b2normal = b2newnormal * normal;
	glm::vec3 b2tangent = b2t * tangent;

	glm::vec3 b1final = b1normal + b1tangent;
	glm::vec3 b2final = b2normal + b2tangent;


	b1->SetMovement(b1final);
	b2->SetMovement(b2final);

	
}
//Chech who is the winner and how many points got
void CountPoints() {
	Ball* first{}, * second{}, * third{};
	float firstd = 1000000, secondd = 10000000, thirdd = 1000000, distance;
	int points = 1, type;

	for (Ball& b : BallList) {
		distance = sqrt(Distance(jack->GetPosition(), b.GetPosition()));

		if (b.GetType() != 0) {
			if (distance < firstd) {
				first = &b;
				firstd = distance;
			}else if (distance < secondd) {
				second = &b;
				secondd = distance;
			}else if (distance < thirdd) {
				third = &b;
				thirdd = distance;
			}
		}
	}
	type = first->GetType();

	if (type == second->GetType()) {
		points++;
		if (type == third->GetType()) points++;
	}

	if (type == 1) printf("The winner is Blue with %d points\n", points);
	if (type == 2) printf("The winner is Red with %d points\n", points);
	end = true;
}
//Start next throw when all the balls stop moving
void Next_Round() {
	bool allstatic = true;
	for (Ball b : BallList) if (!Static(b)) allstatic = false;

	if (allstatic) {
		if (ball_n < 6) {
			BallList.push_back(Ball::Ball(ball_n++ % 2 + 1, binitialpos));
			playball = &BallList.back();
			playing = false;
		}
		else if (!end) {
			playing = true;
			CountPoints();
		}
	}
}


int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(width, height, "3D demo", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window.");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		glfwTerminate();
		return -1;
	}

	//specify the size of the rendering window
	glViewport(0, 0, width, height);

	// Dark blue background
	glClearColor(0.0f, 0.2f, 0.8f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");
	GLuint programID2 = LoadShaders("BallVertexShader.vertexshader", "BallFragmentShader.fragmentshader");


	float terrainvertices[] = {
		10.0f,  0.0f, 10.0f,
		10.0f, 0.0f, -10.0f,
		-10.0f, 0.0f, 10.0f,
		-10.0f,  0.0f, -10.0f,

		
	};


	unsigned int terrainindices[] = {  // note that we start from 0!
		0, 3, 2,
		0, 1, 3,
	};

	float vertices[] = {
		0.05f, 0.0f, 0.05f,
		0.05f, 0.0f, -0.05f,
		-0.05f, 0.0f, 0.05f,
		-0.05f, 0.0f, -0.05f,


	};


	unsigned int indices[] = {  // note that we start from 0!
		0, 3, 2,
		0, 1, 3,
	};



	GLuint vbo1, vao1, ibo1,
		vbo2, vao2, ibo2;
		

	glGenVertexArrays(1, &vao1);
	glGenBuffers(1, &vbo1);
	glGenBuffers(1, &ibo1);

	glBindVertexArray(vao1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(terrainvertices), terrainvertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(terrainindices), terrainindices, GL_STATIC_DRAW);

	//set attribute pointers
	glVertexAttribPointer(
		0,                  // attribute 0, must match the layout in the shader.
		3,                  // size of each attribute
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		3 * sizeof(float),                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(0);

	//Unbinding VAO
	glBindVertexArray(0);

	
	glGenVertexArrays(1, &vao2);
	glGenBuffers(1, &vbo2);
	glGenBuffers(1, &ibo2);

	glBindVertexArray(vao2);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	glBufferData(GL_ARRAY_BUFFER, ball.getVertexSize(), ball.getVertices(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ball.getIndexSize(), ball.getIndices(), GL_STATIC_DRAW);

	
	glVertexAttribPointer(
		0,                  // attribute 0, must match the layout in the shader.
		3,                  // size of each attribute
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		3 * sizeof(float),                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(0);

	//Unbinding VAO
	glBindVertexArray(0);

	glfwSetFramebufferSizeCallback(window, window_callback);


	BallList.push_back(*jack);
	jack = &BallList.back();
	BallList.push_back(Ball::Ball(1, binitialpos));
	playball = &BallList.back();

	//Change accordingly for camera
	glm::mat4 view;
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(90.0f), (float)width / (float)height, 0.1f, 10000.0f);

	
	

	// Check if the window was closed
	while (!glfwWindowShouldClose(window))
	{
		//calculate delta time 
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Swap buffers
		glfwSwapBuffers(window);

		// Check for events
		glfwPollEvents();

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		if (playing) Next_Round();

		processInput(window);

		lightPos = camera.getCameraPosition();

		view = glm::lookAt(camera.getCameraPosition(), camera.getCameraViewDirection(), camera.getCameraUp());

		//Brown Terrain
		
		glm::mat4 model(1.0f);
		model = glm::scale(model, glm::vec3(10.0f, 1.0f, 10.0f));

		
		glm::mat4 matrix = projection * view * model;

		unsigned int transformLoc = glGetUniformLocation(programID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matrix));

		glm::vec4 color = glm::vec4(0.2f, 0.4f, 0.1f, 1.0f);
		unsigned int transformLoc2 = glGetUniformLocation(programID, "color");
		glUniform4fv(transformLoc2, 1, glm::value_ptr(color));

		glBindVertexArray(vao1);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//Green Terrain

		glm::mat4 model2(1.0f);
		model2 = glm::scale(model2, glm::vec3(0.7f, 1.0f, 0.2f));


		glm::mat4 matrix2 = projection * view * model2;

		unsigned int transformLoc3 = glGetUniformLocation(programID, "transform");
		glUniformMatrix4fv(transformLoc3, 1, GL_FALSE, glm::value_ptr(matrix2));

		glm::vec4 color2 = glm::vec4(0.4f, 0.2f, 0.1f, 1.0f);
		unsigned int transformLoc4 = glGetUniformLocation(programID, "color");
		glUniform4fv(transformLoc4, 1, glm::value_ptr(color2));


		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);

		glUseProgram(programID2);

		unsigned int transformLoc5 = glGetUniformLocation(programID2, "lightColor");
		glUniform3fv(transformLoc5, 1, glm::value_ptr(lightColor));

		unsigned int transformLoc6 = glGetUniformLocation(programID2, "lightPos");
		glUniform3fv(transformLoc6, 1, glm::value_ptr(lightPos));

		unsigned int transformLoc7 = glGetUniformLocation(programID2, "viewPos");
		glUniform3fv(transformLoc7, 1, glm::value_ptr(camera.getCameraPosition()));

		//printf("%f %f %f\n", jack->GetPosition().x, jack->GetPosition().y, jack->GetPosition().z);

		//Sort the ball list so the nearest to the camera get rendered last
		BallList.sort(Nearest_Ball);

		for (Ball& b : BallList){
		
			for (Ball& b2 : BallList) if(!(b==b2) && Collide(b, b2) && Nearest_Ball(b,b2)) ManageCollision(&b, &b2);

		glm::mat4 model3 = b.Move();
		model3 = Adjust(model3, b);

		glm::vec4 normals = glm::vec4(glm::make_vec3(ball.getNormals()), 1.0f) * model3;

		glm::mat4 matrix3 = projection * view * model3;


		

		unsigned int transformLoc8 = glGetUniformLocation(programID2, "transform");
		glUniformMatrix4fv(transformLoc8, 1, GL_FALSE, glm::value_ptr(matrix3));

		glm::vec4 color3 = b.GetColor();
		unsigned int transformLoc9 = glGetUniformLocation(programID2, "color");
		glUniform4fv(transformLoc9, 1, glm::value_ptr(color3));

		unsigned int transformLoc10 = glGetUniformLocation(programID2, "normalCoords");
		glUniform4fv(transformLoc10, 1, ball.getNormals());

		unsigned int transformLoc11 = glGetUniformLocation(programID2, "model");
		glUniformMatrix4fv(transformLoc11, 1, GL_FALSE, glm::value_ptr(model3));

		unsigned int transformLoc12 = glGetUniformLocation(programID2, "Pos");
		glUniform3fv(transformLoc12, 1, glm::value_ptr(b.GetPosition()));


		glBindVertexArray(vao2);

		glDrawElements(GL_TRIANGLES, ball.getIndexCount(), GL_UNSIGNED_INT, 0);
		}



	}

	// Cleanup
	glDeleteBuffers(1, &vbo1);
	glDeleteBuffers(1, &vbo2);
	glDeleteBuffers(1, &ibo1);
	glDeleteBuffers(1, &ibo2);
	glDeleteVertexArrays(1, &vao1);
	glDeleteVertexArrays(1, &vao2);
	glDeleteProgram(programID);
	glDeleteProgram(programID2);
	for (Ball& b : BallList) b.~Ball();
	ball.~Sphere();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}