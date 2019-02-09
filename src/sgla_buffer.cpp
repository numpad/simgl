#include "sgla_buffer.hpp"

sgla::buffer::operator ALint() const {
	return this->bufferid;
}

sgla::buffer::buffer() {
	alGenBuffers(1, &this->bufferid);
}

sgla::buffer::~buffer() {
	alDeleteBuffers(1, &this->bufferid);
	free(this->data);
}

void sgla::buffer::loadVorbis(const char *filename) {
	/* load sound */
	this->data_len = stb_vorbis_decode_filename(filename, &this->channels, &this->sample_rate, &this->data);
	assert(this->data);
	
	/* fill buffer */
	alBufferData(this->bufferid, AL_FORMAT_MONO16, this->data, this->data_len * this->channels * sizeof(short), this->sample_rate);
}

