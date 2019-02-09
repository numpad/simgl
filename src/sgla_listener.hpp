#ifndef SGLA_LISTENER_HPP
#define SGLA_LISTENER_HPP

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <AL/al.h>
#include <AL/alc.h>

namespace sgla {

	class listener {
		float gain = 1.0f;
		glm::vec3 pos = glm::vec3(0.0f);
		glm::vec3 vel = glm::vec3(0.0f);
		glm::mat2x3 orientation = glm::mat2x3(0.0f);

	public:
		
		listener(glm::vec3 pos = glm::vec3(0.0f));

		/* update listener data */
		void update();
	

		/* gain */
		void setGain(float v);
		float getGain();
		
		/* position */
		void setPos(glm::vec3 pos);
		glm::vec3 getPos();
		
		/* velocity */
		void setVel(glm::vec3 vel);
		glm::vec3 getVel();
		
		/* update position and velocity */
		void movePos(glm::vec3 relmove);
		void movePosTo(glm::vec3 to);
		void moveFromPosTo(glm::vec3 from, glm::vec3 to);

		/* orientation */
		void setViewTarget(glm::vec3 pos);
		void setViewDirection(glm::vec3 dir);
		glm::vec3 getOrientation();
		
	};

}

#endif

