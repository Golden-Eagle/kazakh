// #ifndef PXLJM_SOUND_HEADER
// #define PXLJM_SOUND_HEADER

// #include "ComponentSystem.hpp"

// #include <map>
// #include "fmod_studio.hpp"

// namespace pxljm {
// 	class SoundSystem : public ComponentSystem {
// 		static const unsigned MAXIMUM_EVENTS = 256;
// 		FMOD::Studio::System* m_system    = NULL;
// 		FMOD::Studio::Bank* m_masterBank  = NULL;
// 		FMOD::Studio::Bank* m_stringsBank = NULL;

// 		std::map<std::string, FMOD::Studio::EventDescription*> m_eventLibrary;
// 	public:
// 		SoundSystem();
// 		virtual ~SoundSystem();

// 		void update();
// 	};
// }

// #endif