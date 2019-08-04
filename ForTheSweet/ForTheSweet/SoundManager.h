#pragma once
#include "Fmod/inc/fmod.hpp"
#pragma comment(lib, "fmod_vc.lib")

using namespace FMOD;
enum SOUNDKIND {
	SOUND_1,
	SOUND_2,
	BACKGROUND,
	FEVERTIME,
	HIT,
	LIGHTING,
	SOUND_END
};
class SoundManager
{
private:
	SoundManager() {};
	SoundManager(const SoundManager& other);

	static SoundManager *instance;
	System* g_pSystem;
	Sound* g_pSound[SOUND_END];
	Channel* g_pChannel[10];
	Channel* background;
public:
	void Setup();
	void Release();
	static SoundManager* GetInstance();

	void Update() {
		g_pSystem->update();
	}
	void PlaySounds(SOUNDKIND eSound);
	void PlayBackGroundSounds(SOUNDKIND eSound);
	void StopBackGroundSounds(SOUNDKIND eSound);
	void FadeOutBackGroundSounds();
	void FadeINBackGroundSounds();

};
