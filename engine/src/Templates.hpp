#ifndef CORE_TEMPLATES_H
#define CORE_TEMPLATES_H

#include <vector>
#include <string>
#include <cstdlib>
#include <memory>
#include <SDL.h>

namespace core {

	
	class nameable {

	public:

		std::string name() {
			return _name;
		}

		void setName(std::string name) {
			_name = name;
		}

	protected:
		std::string _name;
	};


	template <typename T>
	class singleton {

	public:

		static T& unsynchronized() {
			if (singleton<T>::_instance == nullptr) {
				singleton<T>::_instance = new T();
			}
			return *(singleton<T>::_instance);
		}

		static T& instance() {
			if (singleton<T>::_instance == nullptr) {
				SDL_AtomicLock(&singleton<T>::_lock);
				if (singleton<T>::_instance == nullptr) {
					singleton<T>::_instance = new T();
				}
				SDL_AtomicUnlock(&singleton<T>::_lock);
			}
			return *(singleton<T>::_instance);
		}

	protected:
		inline explicit singleton<T>() {};
		
	private: 
		explicit singleton<T>(singleton<T> const&) = delete;
		singleton<T>& operator=(singleton<T> const&) = delete;

		static T* _instance;
		static SDL_SpinLock _lock;
	};

	template<typename T>
	SDL_SpinLock singleton<T>::_lock = 0;

	template<typename T>
	T* singleton<T>::_instance = nullptr;


	template<typename T>
	inline T& unsynchronized() {
		return singleton<T>::unsynchronized();
	}


	template<typename T>

	inline T& single() {
		return singleton<T>::instance();
	}


	template <typename T>
	class renderable {

	public:

		void render() {
			static_cast<T*>(this)->renderImpl();
		}
	};


	template <typename T, typename Return_type, typename Context_type>
	class updateable {

	public:

		Return_type update(Context_type& context) {
			return static_cast<T*>(this)->updateImpl(context);
		}

	};


	class taggable {

	public:
		bool hasTag(std::string tag) {
			for (auto& t : _tags) {
				if (tag.compare(t)) return true;
			}
			return false;
		}

		void setTag(std::string tag) {
			if (hasTag(tag)) return;
			_tags.push_back(tag);
		}

	protected:

		std::vector<std::string> _tags;

	};


	template <typename T>
	class pausable{

	public:
		pausable() : _paused{ false }, _pauseCount{ 0 } {};

		void pause() {
			_pauseCount++;
			if (_paused) return;
			_didPause = true;
			static_cast<T*>(this)->pauseImpl();
			_paused = true;
		}

		void resume() {
			if (!_paused) return;
			_pauseCount--;
			if (_pauseCount > 0) return;
			_didResume = true;
			static_cast<T*>(this)->resumeImpl();
			_paused = false;
		}

		bool isPaused() {
			return _paused;
		}

		bool didPause() {
			auto out = _didPause;
			_didPause = false;
			return out;
		}

		bool didResume() {
			auto out = _didResume;
			_didResume = false;
			return out;

		}


	protected:
		int _pauseCount;
		bool _didPause;
		bool _didResume;
		bool _paused;

	};

	enum class InitStatus {

		CREATE_FALSE,
		CREATE_TRUE,
		CREATE_FAILED,
		INIT_TRUE,
		INIT_FAILED,
		RESET_FAILED,		
		DESTROY_FAILED

	};

	template <typename T, typename Create_type, typename Init_type, typename Reset_type, typename Destroy_type>
	class initializable {
	public:
		initializable() :
			_initStatus{ InitStatus::CREATE_FALSE }
		{}

		template<typename Create_type>
		InitStatus create(Create_type createConfig) {
			if (_initStatus == InitStatus::CREATE_FALSE || _initStatus == InitStatus::CREATE_FAILED) {
				_initStatus = (static_cast<T*>(this)->createImpl(createConfig)) ? InitStatus::CREATE_TRUE : InitStatus::CREATE_FAILED;
			}
			return _initStatus;
		}

		InitStatus create() {
			if (_initStatus == InitStatus::CREATE_FALSE || _initStatus == InitStatus::CREATE_FAILED) {
				_initStatus = (static_cast<T*>(this)->createImpl()) ? InitStatus::CREATE_TRUE : InitStatus::CREATE_FAILED;
			}
			return _initStatus;
		}

		template<typename Init_type>
		InitStatus initialize(Init_type initConfig) {
			if (_initStatus == InitStatus::CREATE_TRUE || _initStatus == InitStatus::INIT_FAILED) {
				_initStatus = (static_cast<T*>(this)->initializeImpl(initConfig)) ? InitStatus::INIT_TRUE : InitStatus::INIT_FAILED;
			}
			return _initStatus;
		}

		
		InitStatus initialize() {
			if (_initStatus == InitStatus::CREATE_TRUE || _initStatus == InitStatus::INIT_FAILED) {
				_initStatus = (static_cast<T*>(this)->initializeImpl()) ? InitStatus::INIT_TRUE : InitStatus::INIT_FAILED;
			}
			return _initStatus;
		}

		template<typename Reset_type>
		InitStatus reset(Reset_type resetConfig) {
			if (_initStatus == InitStatus::INIT_TRUE || _initStatus == InitStatus::RESET_FAILED) {
				_initStatus = (static_cast<T*>(this)->resetImpl(resetConfig)) ? InitStatus::CREATE_TRUE : InitStatus::RESET_FAILED;
			}
			return _initStatus;
		}

		InitStatus reset() {
			if (_initStatus == InitStatus::INIT_TRUE || _initStatus == InitStatus::RESET_FAILED) {
				_initStatus = (static_cast<T*>(this)->resetImpl()) ? InitStatus::CREATE_TRUE : InitStatus::RESET_FAILED;
			}
			return _initStatus;
		}
		
		template<typename Destroy_type>
		InitStatus destroy(Destroy_type destroyConfig) {
			if (_initStatus == InitStatus::CREATE_TRUE || _initStatus == InitStatus::DESTROY_FAILED) {
				_initStatus = (static_cast<T*>(this)->destroyImpl(destroyConfig)) ? InitStatus::CREATE_FALSE : InitStatus::DESTROY_FAILED;
			}
			return _initStatus;
		}

		InitStatus destroy() {
			if (_initStatus == InitStatus::CREATE_TRUE || _initStatus == InitStatus::DESTROY_FAILED) {
				_initStatus = (static_cast<T*>(this)->destroyImpl()) ? InitStatus::CREATE_FALSE : InitStatus::DESTROY_FAILED;
			}
			return _initStatus;
		}

		void setInitializedStatus(InitStatus status) {
			_initStatus = status;
		}

		InitStatus getInitializedStatus() {
			return _initStatus;
		}

	protected:

		InitStatus _initStatus;

    private:

	};

	template <typename T>
	class equal_comparable {
	public:
		equal_comparable<T>() : _id{ equal_comparable<T>::nextId() } {};

		equal_comparable<T>& operator=(equal_comparable<T> const& other) {
			_id = other._id;
			return *this;
		}

		long id() { return _id; }
		equal_comparable<T>(const equal_comparable<T>& other) : _id{ other._id } {};

		friend bool operator==(T const& a, T const& b) { return a._id == b._id; };

		friend bool operator!=(T const& a, T const& b) { return a._id != b._id; };


		static long nextId() {
			return ++equal_comparable<T>::_nextId;
		}

	protected:
		long _id;
		static long _nextId;
	};


	template <typename T>
	long equal_comparable<T>::_nextId = 0;


	template <typename T>
	class bindable {
	public:
		bindable() : _isBound{ false } {};

		bool bind() {
			_isBound = static_cast<T*>(this)->bindImpl();
			return _isBound;
		}

		bool unbind() {
			if (!_isBound) return true;
			_isBound = !(static_cast<T*>(this)->unbindImpl());
			return !_isBound;
		}

		bool isBound() {
			return _isBound;
		}

	protected:
		bool _isBound;

	};

	template <template <typename, typename> class Iterable_type, typename T, typename U>
	void findAndRemove(Iterable_type<T, U>& iterable, T& t) {
		for (unsigned i = 0; i < iterable.size(); i++) {
			if (iterable[i] == t) {
				iterable.erase(begin(iterable) + i);
				break;
			}
		}
	}

	template <template <typename, typename> class Iterable_type, typename T, typename U>
	void findAndRemove(Iterable_type<T*, U>& iterable, T* t) {
		for (unsigned i = 0; i < iterable.size(); i++) {
			if (*iterable[i] == *t) {
				iterable.erase(begin(iterable) + i);
				break;
			}
		}
	}


	template <typename Iterable_type, typename T>
	bool hasElement(Iterable_type& iterable, T t) {
		for (unsigned i = 0; i < iterable.size(); i++) {
			if (iterable[i] == t) {
				return true;
			}

		}
		return false;
	}

	template <typename Map_type, typename Key_type, typename Value_type>
	Value_type& safeFind(Map_type& map, Key_type& key) {
		auto it = map.find(key);
		if (it == end(map)) {
			Console::console() << "Key '" << key << "' not found in map.\n";
#ifdef DEBUG_CORE
			assert("Key not found in map" == "");
#endif
			return Value_type{};
		}
		else {
			return it->second;
		}
	}


#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

} //end namespace core





#endif
