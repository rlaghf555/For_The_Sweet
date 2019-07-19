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
	g_pSystem->playSound(g_pSound[eSound], NULL, false, &g_pChannel[0]);
}

void SoundManager::StopBackGroundSounds()
{
	g_pChannel[0]->stop();
}
