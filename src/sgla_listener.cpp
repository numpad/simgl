#include "sgla_listener.hpp"


sgla::listener::listener(glm::vec3 pos)
	: pos(pos)
{
	this->orientation[0] = glm::vec3(0.0f, 0.0f, 1.0f);
	this->orientation[1] = glm::vec3(0.0f, 1.0f, 0.0f);
}

void sgla::listener::update() {
	alListenerf(AL_GAIN, this->gain);
	alListenerfv(AL_POSITION, glm::value_ptr(this->pos));
	alListenerfv(AL_VELOCITY, glm::value_ptr(this->vel));
	alListenerfv(AL_ORIENTATION, glm::value_ptr(this->orientation));
}

/* gain */
void sgla::listener::setGain(float v) {
	this->gain = v;
}

float sgla::listener::getGain() {
	return this->gain;
}

/* position */
void sgla::listener::setPos(glm::vec3 pos) {
	this->pos = pos;
}

glm::vec3 sgla::listener::getPos() {
	return this->pos;
}

/* velocity */
void sgla::listener::setVel(glm::vec3 vel) {
	this->vel = vel;
}

glm::vec3 sgla::listener::getVel() {
	return this->vel;
}

/* update position and velocity */
void sgla::listener::movePos(glm::vec3 relmove) {
	glm::vec3 right = glm::cross(this->orientation[0], this->orientation[1]);
	glm::vec3 up = this->orientation[1];
	glm::vec3 front = this->orientation[0];
	
	glm::vec3 p = this->pos;
	p += (glm::normalize(right) * relmove.x)
	  +  (glm::normalize(up)    * relmove.y)
	  +  (glm::normalize(front) * relmove.z);
	
	this->movePosTo(p);
}

void sgla::listener::movePosTo(glm::vec3 to) {
	this->moveFromPosTo(this->pos, to);
}

void sgla::listener::moveFromPosTo(glm::vec3 from, glm::vec3 to) {
	glm::vec3 dist = to - from;
	this->setVel(dist);
	this->setPos(to);
}

/* orientation */
void sgla::listener::setViewTarget(glm::vec3 pos) {
	this->orientation[0] = glm::normalize(pos - this->pos);
}

void sgla::listener::setViewDirection(glm::vec3 dir) {
	this->orientation[0] = glm::normalize(dir);
}

glm::vec3 sgla::listener::getOrientation() {
	return this->orientation[0];
}

