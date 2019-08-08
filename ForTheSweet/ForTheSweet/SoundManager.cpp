#include "stdafx.h"
#include "SoundManager.h"
SoundManager* SoundManager::instance = nullptr;
void SoundManager::Setup()
{

	//사운드 시스템생성
	System_Create(&g_pSystem);

	//채널수, 모드
	g_pSystem->init(10, FMOD_INIT_NORMAL, nullptr);
	g_pSystem->createSound("resource\\sound\\Soundtest.mp3", FMOD_LOOP_NORMAL, 0, &g_pSound[SOUND_1]);
	g_pSystem->createSound("resource\\sound\\effectsoundtest.mp3", FMOD_DEFAULT, 0, &g_pSound[SOUND_2]);
	g_pSystem->createSound("resource\\sound\\bgm.mp3", FMOD_LOOP_NORMAL, 0, &g_pSound[BACKGROUND]);
	g_pSystem->createSound("resource\\sound\\fevertime.mp3", FMOD_LOOP_NORMAL, 0, &g_pSound[FEVERTIME]);
	g_pSystem->createSound("resource\\sound\\hit.mp3", FMOD_DEFAULT, 0, &g_pSound[HIT]);
	//g_pSystem->createSound("resource\\sound\\hit2.mp3", FMOD_DEFAULT, 0, &g_pSound[HIT]);
	//g_pSystem->createSound("resource\\sound\\hit3.mp3", FMOD_DEFAULT, 0, &g_pSound[HIT]);
	g_pSystem->createSound("resource\\sound\\lightning.mp3", FMOD_DEFAULT, 0, &g_pSound[LIGHTNING]);	
	g_pSystem->createSound("resource\\sound\\jump.mp3", FMOD_DEFAULT, 0, &g_pSound[JUMPSOUND]);	
	g_pSystem->createSound("resource\\sound\\fog.mp3", FMOD_LOOP_NORMAL, 0, &g_pSound[FOG]);
	g_pSystem->createSound("resource\\sound\\win.mp3", FMOD_DEFAULT, 0, &g_pSound[WIN]);	
	g_pSystem->createSound("resource\\sound\\lose.mp3", FMOD_DEFAULT, 0, &g_pSound[LOSE]);	
}
void SoundManager::Release()
{
	g_pSystem->release();
	g_pSystem->close();
}

SoundManager* SoundManager::GetInstance()
{
	if (instance == nullptr)
		instance = new SoundManager();
	return instance;
}

void SoundManager::PlaySounds(SOUNDKIND eSound)
{	
	g_pSystem->update();
	g_pSystem->playSound(g_pSound[eSound], NULL, false, g_pChannel);
}

void SoundManager::PlayBackGroundSounds(SOUNDKIND eSound)
{
	g_pSystem->playSound(g_pSound[eSound], NULL, false, &background);
}

void SoundManager::StopBackGroundSounds()
{
	background->stop();
}

void SoundManager::FadeOutBackGroundSounds()
{
	unsigned long long dspclock;
	FMOD::System *sys;
	int rate;
	background->getSystemObject(&sys);                        // OPTIONAL : Get System object	
	sys->getSoftwareFormat(&rate, 0, 0);                // Get mixer rate	

	background->getDSPClock(0, &dspclock);                    // Get the reference clock, which is the parent channel group
	
	background->addFadePoint(dspclock, 1.0f);                 // Add a fade point at 'now' with full volume.
	
	background->addFadePoint(dspclock + (rate * 5), 0.f);    // Add a fade point 5 seconds later at 0 volume.

	background->setDelay(0, dspclock + (rate * 5), true);
}

void SoundManager::FadeINBackGroundSounds()
{
	unsigned long long dspclock;
	FMOD::System *sys;
	int rate;
	background->getSystemObject(&sys);                        // OPTIONAL : Get System object	
	sys->getSoftwareFormat(&rate, 0, 0);                // Get mixer rate	

	background->getDSPClock(0, &dspclock);                    // Get the reference clock, which is the parent channel group

	background->addFadePoint(dspclock, 0.0f);                 // Add a fade point at 'now' with full volume.

	background->addFadePoint(dspclock + (rate * 5), 1.f);    // Add a fade point 5 seconds later at 0 volume.

}
