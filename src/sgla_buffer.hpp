#ifndef SGLA_BUFFER_HPP
#define SGLA_BUFFER_HPP

#include <assert.h>
#include <stdlib.h>
#include <AL/al.h>
#include <AL/alc.h>

#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"

namespace sgla {

	class buffer {
		ALuint bufferid = 0;
		
		short *data = nullptr;
		int data_len = 0;
		int channels = 0;
		int sample_rate = 0;

	public:
		
		operator ALint() const;

		buffer();
		~buffer();
		
		void loadVorbis(const char *filename);
	};

}

#endif

