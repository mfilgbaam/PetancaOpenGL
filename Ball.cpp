#include "Ball.h"

Ball::Ball(int type,glm::vec3 position) {
	this->type = type;
	this->position = position;
	this->rotationX = 0;
	this->rotationZ = 0;
	if (type == 0) {
		this->color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		this->hitbox = 0.01;
		this->mass = 0.9;
	}
	if (type == 1) {
		this->color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		this->hitbox = 0.02;
		this->mass = 1.0;
	}
	if (type == 2) {
		this->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		this->hitbox = 0.02;
		this->mass = 1.0;
	}
	this->trans = glm::translate(glm::mat4(1.0f), this->position);
	this->position.y *= this->hitbox;
}
Ball::~Ball() {

}
glm::vec3 Ball::GetPosition() {
	return this->position;
}
void Ball::SetPosition(glm::vec3 position) {
	this->position = position;
}
float Ball::GetHitbox() {
	return hitbox;
}
glm::vec4 Ball::GetColor() {
	return color;
}
glm::mat4 Ball::GetTrans() {
	return trans;
}
void Ball::AddMovement(glm::vec3 movement) {
	this->movement += movement;
}
void Ball::AddMovement(float movement) {
	glm::vec3 new_movement = glm::vec3(0.0f, sin(glm::radians(this->rotationX)) * movement, cos(glm::radians(this->rotationX)) * movement);
	trans = glm::translate(trans, new_movement);
	this->position.z += movement;
}
glm::vec3 Ball::GetMovement() {
	return this->movement;
}
void Ball::SetMovement(glm::vec3 movement) {
	this->movement = movement;
}
glm::mat4 Ball::Move() {

	if (abs(this->movement.x) > this->deceleration) this->movement.x -= this->deceleration * (this->movement.x/ abs(this->movement.x));
	else  this->movement.x = 0; 

	if (abs(this->movement.z) > this->deceleration) this->movement.z -= this->deceleration * (this->movement.z / abs(this->movement.z));
	else  this->movement.z = 0;

	this->rotationZ += this->movement.x;
	this->trans = glm::rotate(this->trans, this->movement.x, glm::vec3(0.0, 0.0, 1.0));
	this->rotationX += this->movement.z;
	this->trans = glm::rotate(this->trans, this->movement.z, glm::vec3(1.0, 0.0, 0.0));

	glm::vec3 new_movement = glm::vec3(cos(glm::radians(this->rotationZ)) * this->movement.x, -sin(glm::radians(this->rotationZ)) * this->movement.x, 0.0f);
	new_movement += glm::vec3(0.0f, sin(glm::radians(this->rotationX)) * this->movement.z, cos(glm::radians(this->rotationX)) * this->movement.z);

	trans = glm::translate(trans, new_movement);

	this->position.x += this->movement.x;
	this->position.z += this->movement.z;

	return trans;
}


