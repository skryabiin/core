// DebugConsole.cpp : Defines the entry point for the console application.
//
// Echoes all input to stdout. This will be redirected by the redirect
// sample. Compile and build child.c as a Win32 Console application and
// put it in the same directory as the redirect sample.
//
#include "DebugConsole.hpp"

namespace core {

	TextInput::TextInput() {
		_isStarted = false;
	}

	TextInput::TextInput(TTF_Font* font, SDL_Renderer* renderer) {

		_isStarted = false;
		_consoleDebugColor.a = 0xFF;
		_consoleDebugColor.b = 0x00;
		_consoleDebugColor.g = 0xFF;
		_consoleDebugColor.r = 0x00;
		_font = font;
		_renderer = renderer;
		_text = "";
	}


	void TextInput::start() {

		_isStarted = true;
		_text = "";
		_renderText();
	}

	void TextInput::appendText(const char* text) {
		if (!_isStarted) return;
		_text = _text.append(text);
		_renderText();

	}

	void TextInput::backspace() {
		if (_text.length() == 0) return;
		if (_text.length() == 1) {
			_text = "";
			_renderText();
			return;
		}
		_text = _text.substr(0, _text.length() - 1);
		_renderText();
	}

	bool TextInput::isStarted() const {
		return _isStarted;
	}

	std::string TextInput::getText() const {
		return _text;
	}

	void TextInput::draw() const {
		if (!_isStarted) return;
		SDL_RenderCopy(_renderer, _renderedText, &_source, &_target);
	}

	void TextInput::stop() {
		_text = "";
		SDL_DestroyTexture(_renderedText);
		_isStarted = false;
	}

	void TextInput::_renderText() {
		std::string text = ">" + _text + "_";
		auto tempSurf = TTF_RenderText_Solid(_font, text.c_str(), _consoleDebugColor);
		_target.w = tempSurf->w;
		_target.h = tempSurf->h;
		_target.x = 5;
		_target.y = SCREEN_HEIGHT - 20;
		_source.x = 0;
		_source.y = 0;
		_source.w = tempSurf->w;
		_source.h = tempSurf->h;
		_renderedText = SDL_CreateTextureFromSurface(_renderer, tempSurf);
		SDL_FreeSurface(tempSurf);
		tempSurf = nullptr;
	}



		Message::Message() {
			_renderedText = nullptr;
		}

		void Message::init(std::string text, TTF_Font* font, SDL_Renderer* renderer) {
			_text = text;
			auto color = getColor(text);
			auto tempSurf = TTF_RenderText_Solid(font, _text.c_str(), color);
			_target.w = tempSurf->w;
			_target.h = tempSurf->h;
			_target.x = 5;
			_source.x = 0;
			_source.y = 0;
			_source.w = tempSurf->w;
			_source.h = tempSurf->h;
			_renderedText = SDL_CreateTextureFromSurface(renderer, tempSurf);

			SDL_FreeSurface(tempSurf);
			tempSurf = nullptr;

		}

		SDL_Color Message::getColor(std::string text) {

			auto severity = text.substr(0, 5);
			if (!severity.compare("[fata")) {
				auto consoleFatalColor = SDL_Color{};
				consoleFatalColor.a = 0xFF;
				consoleFatalColor.b = 0xFF;
				consoleFatalColor.g = 0xFF;
				consoleFatalColor.r = 0xFF;
				return consoleFatalColor;
			}
			else if (!severity.compare("[warn")) {

				auto consoleWarnColor = SDL_Color{};
				consoleWarnColor.a = 0xFF;
				consoleWarnColor.b = 0x00;
				consoleWarnColor.g = 0xAA;
				consoleWarnColor.r = 0xAA;
				return consoleWarnColor;
			}
			else if (!severity.compare("[noti")) {
				auto consoleNoticeColor = SDL_Color{};
				consoleNoticeColor.a = 0xFF;
				consoleNoticeColor.b = 0xFF;
				consoleNoticeColor.g = 0xAA;
				consoleNoticeColor.r = 0xAA;
				return consoleNoticeColor;
			}
			else if (!severity.compare("[erro")) {
				auto consoleErrorColor = SDL_Color{};
				consoleErrorColor.a = 0xFF;
				consoleErrorColor.b = 0x00;
				consoleErrorColor.g = 0x00;
				consoleErrorColor.r = 0xFF;
				return consoleErrorColor;
			}
			else if (!severity.compare("[dbug")) {
				auto consoleErrorColor = SDL_Color{};
				consoleErrorColor.a = 0xFF;
				consoleErrorColor.b = 0xCC;
				consoleErrorColor.g = 0xCC;
				consoleErrorColor.r = 0xCC;
			}
			else {
				auto consoleDebugColor = SDL_Color{};

				consoleDebugColor.a = 0xFF;
				consoleDebugColor.b = 0x00;
				consoleDebugColor.g = 0xFF;
				consoleDebugColor.r = 0x00;

				return consoleDebugColor;
			}
		}

		std::string Message::getText() const {
			return _text;
		}

		void Message::draw(SDL_Renderer* renderer, int pos, int &_cursorY, bool _didScroll) {
			_target.y = (FONT_SIZE + FONT_BUFFER) *(pos - _cursorY) + 2;
			if (!_didScroll && (_target.y + _target.h) > SCREEN_HEIGHT - FONT_SIZE) {
				_cursorY++;
				_target.y = (FONT_SIZE + FONT_BUFFER) *(pos - _cursorY) + 2;
			}
			SDL_RenderCopy(renderer, _renderedText, &_source, &_target);
		}

		void Message::destroy() {
			SDL_DestroyTexture(_renderedText);

		}




	void DebugConsole::clearMessages() {
		for (auto it = std::begin(_messages); it != std::end(_messages); ++it) {
			it->destroy();
		}
		_messages.clear();
		_cursorY = 0;
		_didScroll = false;
	}

	
	void DebugConsole::run() {

		SDL_Init(SDL_INIT_VIDEO);
		TTF_Init();

		auto sdlWindow = SDL_CreateWindow("Core Debugger", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

		_sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED);
		SDL_SetRenderDrawColor(_sdlRenderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_SetRenderDrawBlendMode(_sdlRenderer, SDL_BLENDMODE_BLEND);
		auto consoleTextcolor = SDL_Color{};

		consoleTextcolor.a = 0xFF;
		consoleTextcolor.b = 0x00;
		consoleTextcolor.g = 0xFF;
		consoleTextcolor.r = 0x00;

		_ttfConsoleFont = TTF_OpenFont(FONT_PATH, FONT_SIZE);

		_gogogo = true;


		SDL_Event e{};

		_cursorY = 0;
		_didScroll = false;
		

		_textInput = TextInput{ _ttfConsoleFont, _sdlRenderer };

		while (_gogogo)
		{

			if (!_logFile.is_open()) {
				_logFile.clear();
				SDL_Delay(100);
				_logFile.open(LOG_FILE_PATH, std::ios::in);
			}
			else {
				SDL_Delay(20);
				std::string thisLine;

				std::string thisMessage = "";

				while (std::getline(_logFile, thisLine)) {
					if (thisLine[0] == '[' && thisMessage.compare("")) {
						addMessage(thisMessage);
						thisMessage = thisLine;
					}
					else {
						thisMessage = thisMessage + thisLine;
						addMessage(thisMessage);
						thisMessage = "";
					}

					if (_logFile.eof()) {
						addMessage(thisMessage);
						thisMessage = "";
					}
				}
				_logFile.clear();
			}

			SDL_RenderClear(_sdlRenderer);

			int depth = 0;

			for (auto it = std::begin(_messages); it != std::end(_messages); it++) {

				it->draw(_sdlRenderer, depth, _cursorY, _didScroll);
				depth++;
			}
			_textInput.draw();
			SDL_RenderPresent(_sdlRenderer);

			while (SDL_PollEvent(&e) != 0)
			{
				switch (e.type) {
				case SDL_TEXTINPUT:
					if (_textInput.isStarted()) {
						_textInput.appendText(e.text.text);
					}
					break;
				case SDL_MOUSEWHEEL:
					_cursorY -= e.wheel.y;
					_cursorY = (_cursorY > 0) ? _cursorY : 0;
					if (_cursorY > 0) {
						_didScroll = true;
					}
					break;
				case SDL_KEYDOWN:
					if (_textInput.isStarted()) {
						std::string result;
						switch (e.key.keysym.sym) {
						case SDLK_ESCAPE:
							_textInput.stop();
							SDL_StopTextInput();
							break;
						case SDLK_RETURN:
							result = parseCommand(_textInput.getText());
							if (result.compare("")) {								
								addMessage(result);
							}
							_textInput.stop();
							SDL_StopTextInput();

							break;
						case SDLK_BACKSPACE:
							_textInput.backspace();
							break;
						}
					}
					else {
						switch (e.key.keysym.sym) {
						case SDLK_RETURN:
							_didScroll = false;
							_textInput.start();
							SDL_StartTextInput();
							break;
						case SDLK_DELETE:
							if (_logFile.is_open()) {
								_logFile.close();
							}
							_logFile.open(LOG_FILE_PATH, std::ios::out | std::ios::trunc);
							_logFile.close();
						case SDLK_BACKSPACE:
							clearMessages();
							_cursorY = 0;
							_didScroll = false;
							break;
						case SDLK_SPACE:
							_didScroll = false;
							break;
						}
					}
					break;
				case SDL_QUIT:
					_gogogo = false;
					break;
				}
			}
		}

		if (_logFile.is_open()) {
			_logFile.close();
		}

		clearMessages();

		TTF_CloseFont(_ttfConsoleFont);
		TTF_Quit();

		SDL_DestroyRenderer(_sdlRenderer);
		SDL_DestroyWindow(sdlWindow);

		SDL_Quit();

	}

	void DebugConsole::addMessage(std::string text) {

		text = (!text.compare("")) ? " " : text;
		_messages.emplace_back();
		_messages.back().init(text, _ttfConsoleFont, _sdlRenderer);

	}

	void DebugConsole::purgeLog() {
		if (_logFile.is_open()) {
			_logFile.close();
		}
		_logFile.open(LOG_FILE_PATH, std::ios::out | std::ios::trunc);
		_logFile.close();
		_logFile.clear();

	}

	std::string DebugConsole::parseCommand(std::string input) {

		if (!input.compare("clear")) {
			clearMessages();
			return "--Screen Cleared.";
		} else if (!input.compare("purge")) {
			purgeLog();
			clearMessages();			
			return "--Log Purged.";
		}
		else if (!input.compare("quit")) {
			_gogogo = false;
			return "--Quitting.";
		}
		else if (!input.compare("filestatus")) {
			std::string isopen = (_logFile.is_open()) ? "open." : "closed.";
			_logFile.close();
			return "--Log file is " + isopen;

		}
		else {
			return "--Unknown Command: " + input;
		}

	}

}