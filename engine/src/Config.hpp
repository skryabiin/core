#ifndef CORE_CONFIG_HPP
#define CORE_CONFIG_HPP

#include <string>

namespace core {

	struct cameraConfig {
		float scaleX;
		float scaleY;
		float centerWorldPointX;
		float centerWorldPointY;
	};

struct rendererConfig {
		bool vsync;
        int windowHeight;
        int windowWidth;
        int colorDepth;		
		int maxFps;
};


struct inputConfig {
	

	int keyScancodeLeft;
	int keyScancodeRight;
	int keyScancodeJump;
	int keyScancodeUp;
	int keyScancodeDown;
	int keyScancodeCycleColorPrev;
	int keyScancodeCycleColorNext;

	int buttonLeft;
	int buttonRight;
	int buttonJump;
	int buttonUp;
	int buttonDown;
	int buttonCycleColorPrev;
	int buttonCycleColorNext;

	bool keyboardActive;
	bool gamepadActive;
	//the standard move speed
	float moveSpeed;

	//the amount of time it takes to reach full movement speed, and to reach no movement when stopped
	//not implemented yet
	float moveAccelDuration;
	float moveDecelDuration;

	float jumpSpeed;
};

struct gameConfig {
    std::string title;
	int keyScancodePause;
	int buttonPause;
};

struct consoleConfig {

	consoleConfig() : debug{ false } {};
	bool debug;
	int keyToggle;
	int buttonToggle;

};

struct Config {

	cameraConfig camera;
    rendererConfig renderer;
    gameConfig game;
	consoleConfig console;
	inputConfig input;

    };


} //end namespace core


#endif // CONFIG_H_INCLUDED
