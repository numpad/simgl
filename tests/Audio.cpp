#include "simgl.hpp"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <AL/al.h>
#include <AL/alc.h>
#include "sgla_listener.hpp"
#include "sgla_source.hpp"
#include "sgla_buffer.hpp"

#define SLEEP(f) usleep((useconds_t)(f * 1000.0f * 1000.0f));

static const ALCchar *list_devices(const ALCchar *devices) {
	const ALCchar *device = devices,
	              *next = devices + 1;
	
	printf("Devices:\n");
	int id = 0;
	while (device && *device != '\0' && next && *next != '\0') {
		printf("  %d: %s\n", id++, device);
		const size_t len = strlen(device);
		device += (len + 1);
		next += (len + 2);
	}
	printf("End Devices - %d total\n", id);
	
	return devices;
}

int main(int argc, char *argv[]) {
	/* device enumeration */
	const ALCchar *selected_device;
	if (alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT") == AL_TRUE) {
		selected_device = list_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));
	}

	/* open device */
	ALCdevice *device = alcOpenDevice(selected_device);
	if (!device) {
		puts("alcOpenDevice failed!");
	}
	
	/* context creation */
	ALCcontext *context = alcCreateContext(device, NULL);
	if (!alcMakeContextCurrent(context)) {
		puts("alcMakeContextCurrent failed!");
	}
	
	/* listener */
	sgla::listener listener;
	listener.update();

	/* source */
	sgla::source source;
	
	/* load sound */
	sgla::buffer buffer;
	buffer.loadVorbis("assets/audio/explode.ogg");

	/* bins source to buffer */
	source.setBuffer(buffer);

	/* wait until playing complete */
	char c = ' ';
	while (c != 'q') {

		switch (c) {
			case 'p':
				/* play sound */
				source.play();
				break;
			case '~':
				source.blockUntilFinish();
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9': {
				int num = c - '0' + 1;
				float sleeptime = num * 0.1f;
				SLEEP(sleeptime);
				break;
			}
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j': {
				int num = c - 'a' + 1;
				float pitch = num * 0.1f;
				source.setPitch(pitch);
				break;
			}
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J': {
				int num = c - 'A' + 1;
				float pitch = num * 0.1f + 1.0;
				source.setPitch(pitch);
				break;
			}
			case '>':
				listener.movePos(glm::vec3(-0.5f, 0.0f, 0.0f));
				listener.update();
				break;
			case '<':
				listener.movePos(glm::vec3( 0.5f, 0.0f, 0.0f));
				listener.update();
				break;
			case '\n':
				source.blockUntilFinish();
				source.setPitch(1.0f);
				listener.setPos(glm::vec3(0.0f));
				listener.setVel(glm::vec3(0.0f));
				listener.update();
				break;
			default:
				break;
		};
		c = getchar();
	}

	/* cleanup */
	device = alcGetContextsDevice(context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);

	return 0;
}

