#ifndef SGLA_SOURCE_HPP
#define SGLA_SOURCE_HPP

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <AL/al.h>
#include <AL/alc.h>

namespace sgla {

	class source {
		ALuint sourceid = 0;

	public:
		
		operator ALint() const;

		source();
		~source();
		
		/* reset to defaults */
		void reset();

		/* play */
		bool isPlaying();
		void play();
		void playBlocking();
		void blockUntilFinish();

		/* buffer */
		void setBuffer(ALint i);
		ALint getBuffer();

		/* pitch */
		void setPitch(float v);
		float getPitch();

		/* gain */
		void setGain(float v);
		float getGain();

		/* position */
		void setPos(glm::vec3 pos);
		glm::vec3 getPos();

		/* velocity */
		void setVel(glm::vec3 vel);
		glm::vec3 getVel();

		/* loop */
		void setLooping(bool b);
		bool isLooping();
	};

}


#endif

