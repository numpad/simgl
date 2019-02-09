#include "sgla_source.hpp"

sgla::source::operator ALint() const {
	return this->sourceid;
}

sgla::source::source() {
	alGenSources(1, &this->sourceid);
	this->reset();
}

sgla::source::~source() {
	alDeleteSources(1, &this->sourceid);
}

/* reset to defaults */
void sgla::source::reset() {
	alSourcef(this->sourceid, AL_PITCH, 1.0f);
	alSourcef(this->sourceid, AL_GAIN, 1.0f);
	alSource3f(this->sourceid, AL_POSITION, 0.0f, 0.0f, 0.0f);
	alSource3f(this->sourceid, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
	alSourcei(this->sourceid, AL_LOOPING, AL_FALSE);
}


/* play */
bool sgla::source::isPlaying() {
	ALint source_state;
	alGetSourcei(this->sourceid, AL_SOURCE_STATE, &source_state);
	return source_state == AL_PLAYING ? true : false;
}

void sgla::source::play() {
	alSourcePlay(this->sourceid);
}

void sgla::source::playBlocking() {
	this->play();
	this->blockUntilFinish();
}

void sgla::source::blockUntilFinish() {
	while (this->isPlaying());
}

/* buffer */
void sgla::source::setBuffer(ALint i) {
	alSourcei(this->sourceid, AL_BUFFER, i);
}

ALint sgla::source::getBuffer() {
	ALint buf;
	alGetSourcei(this->sourceid, AL_BUFFER, &buf);
	return buf;
}

/* pitch */
void sgla::source::setPitch(float v) {
	alSourcef(this->sourceid, AL_PITCH, v);
}

float sgla::source::getPitch() {
	float pitch;
	alGetSourcef(this->sourceid, AL_PITCH, &pitch);
	return pitch;
}


/* gain */
void sgla::source::setGain(float v) {
	alSourcef(this->sourceid, AL_GAIN, v);
}

float sgla::source::getGain() {
	float gain;
	alGetSourcef(this->sourceid, AL_GAIN, &gain);
	return gain;
}


/* position */
void sgla::source::setPos(glm::vec3 pos) {
	alSourcefv(this->sourceid, AL_POSITION, glm::value_ptr(pos));
}

glm::vec3 sgla::source::getPos() {
	glm::vec3 pos;
	alGetSourcefv(this->sourceid, AL_POSITION, glm::value_ptr(pos));
	return pos;
}


/* velocity */
void sgla::source::setVel(glm::vec3 vel) {
	alSourcefv(this->sourceid, AL_VELOCITY, glm::value_ptr(vel));
}

glm::vec3 sgla::source::getVel() {
	glm::vec3 vel;
	alGetSourcefv(this->sourceid, AL_VELOCITY, glm::value_ptr(vel));
	return vel;
}


/* loop */
void sgla::source::setLooping(bool b) {
	alSourcei(this->sourceid, AL_LOOPING, b ? AL_TRUE : AL_FALSE);
}

bool sgla::source::isLooping() {
	ALint loop;
	alGetSourcei(this->sourceid, AL_LOOPING, &loop);
	return loop == AL_TRUE ? true : false;
}


