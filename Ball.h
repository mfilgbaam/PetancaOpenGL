#pragma once

// Include GLM
#include "dependente\glm\glm.hpp"
#include "dependente\glm\gtc\matrix_transform.hpp"
#include "dependente\glm\gtc\type_ptr.hpp"
class Ball
{
private:
	glm::vec3 position;
	glm::mat4 trans;
	glm::vec4 color;
	glm::vec3 movement;
	float rotationX = 0;
	float rotationZ = 0;
	float hitbox;
	float mass;
	int type;
	float deceleration=0.00001;
	int test=1;
public:
	Ball(int,glm::vec3);
	~Ball();
	bool operator== (Ball &b2) { return this->position == b2.GetPosition(); }
	glm::vec3 GetPosition();
	void SetPosition(glm::vec3);
	float GetHitbox();
	float GetDeceleration() { return this->deceleration; }
	float GetMass() { return this->mass; }
	int GetType() { return this->type; }
	glm::vec4 GetColor();
	glm::mat4 GetTrans();
	void AddMovement(glm::vec3);
	void AddMovement(float);
	void SetMovement(glm::vec3 movement);
	glm::vec3 GetMovement();
	glm::mat4 Move();
	
};

