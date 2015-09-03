// #include "SoundSystem.hpp"

// #include "fmod_studio.hpp"
// #include "fmod_errors.h"

// #include <gecom/Log.hpp>

// using namespace gecom;

// pxljm::SoundSystem::SoundSystem() {
// 	section_guard("SoundSystem");
// 	FMOD_RESULT result;

// 	result = FMOD::Studio::System::create(&m_system);
// 	if(result != FMOD_OK) {
// 		std::cout << "create() error: " << FMOD_ErrorString(result) << std::endl;
// 	}

// 	result = m_system->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);
// 	if(result != FMOD_OK) {
// 		std::cout << "initialize() error: " << FMOD_ErrorString(result) << std::endl;
// 	}

// 	result = m_system->loadBankFile("./res/sound/pxljm15/Build/Desktop/Master Bank.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &m_masterBank);
// 	if(result != FMOD_OK) {
// 		std::cout << "loadBankFile() error: " << FMOD_ErrorString(result) << std::endl;
// 	}
// 	result = m_system->loadBankFile("./res/sound/pxljm15/Build/Desktop/Master Bank.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &m_stringsBank);
// 	if(result != FMOD_OK) {
// 		std::cout << "loadBankFile() error: " << FMOD_ErrorString(result) << std::endl;
// 	}

// 	// result = m_system->getEvent("event:/Test", &m_testEvent);
// 	// if(result != FMOD_OK) {
// 	//   std::cout << "getEvent() error: " << FMOD_ErrorString(result) << std::endl;
// 	// }

// 	// need to get a list of available effects
// 	// we're making an assumption we're only using the main bank
// 	FMOD::Studio::EventDescription *events[MAXIMUM_EVENTS];
// 	int count;
// 	result = m_masterBank->getEventList(events, MAXIMUM_EVENTS, &count);
// 	if(result != FMOD_OK) {
// 		std::cout << "getEventList() error: " << FMOD_ErrorString(result) << std::endl;
// 	}


// 	Log::info() << "Loading " << count << " event(s) from the master bank";
// 	{
// 		section_guard("LoadEvents");
// 		for(int i = 0; i < count; i++) {
// 			char eventPath[256];
// 			int len;
// 			result = events[i]->getPath(eventPath, 256, &len);
// 			std::string eventPathString(eventPath);
// 			Log::info() << "Loaded new event: '" << eventPathString << "'";
// 			m_eventLibrary[eventPathString] = events[i];
// 		}
// 	}
// }

// pxljm::SoundSystem::~SoundSystem() {
// 	m_system->release();
// }

// void pxljm::SoundSystem::update() {
// 	m_system->update();
// }