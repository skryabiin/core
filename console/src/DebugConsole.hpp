#ifndef CORE_DEBUG_CONSOLE_HPP
#define CORE_DEBUG_CONSOLE_HPP

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <fstream>
#include <vector>
#include <string>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 800
#define FONT_SIZE 15
#define FONT_BUFFER 2
#define FONT_PATH "./assets/fonts/coders_crux.ttf"
#define LOG_FILE_PATH "../../engine/bin/log.txt"

namespace core {

	class Message {
	public:

		Message();

		void init(std::string text, TTF_Font* font, SDL_Renderer* renderer);

		SDL_Color getColor(std::string text);

		std::string getText() const;

		void draw(SDL_Renderer* renderer, int pos, int &cursorY, bool didScroll);

		void destroy();
	private:

		std::string _text;
		SDL_Texture* _renderedText;
		SDL_Rect _target;
		SDL_Rect _source;
	};


	class TextInput {

	public:

		TextInput();

		TextInput(TTF_Font* font, SDL_Renderer* renderer);


		void start();

		void appendText(const char* text);

		void backspace();
		bool isStarted() const;

		std::string getText() const;
		void draw() const;

		void stop();

	private:

		void _renderText();

		bool _isStarted;
		SDL_Color _consoleDebugColor;
		TTF_Font* _font;
		SDL_Renderer* _renderer;
		SDL_Texture* _renderedText;
		SDL_Rect _target;
		SDL_Rect _source;
		std::string _text;



	};

	class DebugConsole {

	public:

		void run();

		void clearMessages();

		void purgeLog();

		std::string parseCommand(std::string input);

		void addMessage(std::string text);

	private:
		bool _gogogo; 
		int _cursorY;
		bool _didScroll;
		std::fstream _logFile;
		TextInput _textInput;
		TTF_Font* _ttfConsoleFont;
		SDL_Renderer* _sdlRenderer;
		SDL_SpinLock _readThreadSpinlock;
		std::vector<Message> _messages;
	};




} //end namespace core

#endif