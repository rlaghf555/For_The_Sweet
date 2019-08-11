#pragma once

// 속도
constexpr float NORMAL_SPEED = 30.f;

constexpr float GRAVITY = 9.81f;
constexpr float MAP_BOTTOM_HEIGHT = 10.1f;

constexpr float CH_HALF_HEIGHT = 17.5f;
constexpr float CH_CAPSULE_HEIGHT = 15.f;
constexpr float CH_CAPSULE_RADIUS = 10.f;

constexpr int MAX_WEAPON_TYPE = 5;
constexpr int MAX_WEAPON_NUM = 20;
constexpr int RESPAWN_WEAPON_NUM = 10;

constexpr int MAX_ANIM = 33;
constexpr int MAX_HP = 100;

constexpr float FALL_HEIGHT = -100.f;

// Timer
constexpr int MAX_TIMER = 150;			// 원래 300
constexpr int MINUTE = 30;				// 원래 60

// Referee
constexpr int Rfr_Start_Num = 11;
constexpr int Rfr_Init = 23;
constexpr int Rfr_Start = 20;
constexpr int Rfr_End = 10;
constexpr int Rfr_Weapon = 0;

// Lightning
constexpr int Lightning_Type = 100;
constexpr float Lightning_Width = 20.f;
constexpr float Lightning_Height = 60.f;

// ROOM
constexpr int MAX_ROOM_NAME_LEN = 10;
constexpr int MAX_ROOM_USER = 8;

constexpr int ROOM_MODE_TEAM = 0;
constexpr int ROOM_MODE_SOLO = 1;
constexpr int ROOM_MODE_KING = 2;

constexpr int ROOM_TIMER_START = 1000;

constexpr int ROOM_ST_WAITING = 0;
constexpr int ROOM_ST_LOADING = 1;
constexpr int ROOM_ST_LOADING_COMPLETE = 2;
constexpr int ROOM_ST_PLAYING = 3;
constexpr int ROOM_ST_END = 4;

// MAP
constexpr int MAP_Wehas = 0;
constexpr int MAP_Oreo = 1;
constexpr int MAP_Cake = 2;

// MAP OBJECT
constexpr int MAP_1_BASE = 0;
constexpr int MAP_2_BASE = 1;
constexpr int MAP_2_CHOCOHAR = 2;
constexpr int MAP_3_BRIDGE = 3;
constexpr int MAP_3_FLOOR_1 = 4;
constexpr int MAP_3_FLOOR_2 = 5;
constexpr int MAP_3_FLOOR_3 = 6;
constexpr int MAP_3_IN = 7;
constexpr int MAP_3_IN_STAIR_1 = 8;
constexpr int MAP_3_IN_STAIR_2 = 9;
constexpr int MAP_3_STAIR = 10;
constexpr int MAP_MACARON = 11;

// Weapon
constexpr int Weapon_Lollipop = 0;
constexpr int Weapon_chupachupse = 1;
constexpr int Weapon_pepero = 2;
constexpr int Weapon_chocolate = 3;
constexpr int Weapon_cupcake = 4;
constexpr int Weapon_King = 5;

// Pepero
constexpr float Pepero_Width = 1.f;
constexpr float Pepero_Length = 18.f;
constexpr float Pepero_Vel = 170.f;
constexpr float Pepero_Trigger_Len = 27.f;
constexpr float Pepero_Pos_Gap = 23.f;

// Lollipop
constexpr float Lollipop_Pos_Gap = 10.f;
constexpr float Lollipop_Radius = 43.f;

// Chupa
constexpr float Candy_Radius = 25.f;

// Chocolate
constexpr float Chocolate_Width = 10.f;
constexpr float Chocolate_Height = 16.f;
constexpr float Chocolate_Depth = 2.f;
constexpr float Chocolate_Len = 15.f;
constexpr float Chocolate_Scale = 1.6f;


// Trigger_Type
constexpr int Player_Trigger = 0;
constexpr int Lollipop_Trigger = 1;
constexpr int Candy_Trigger = 2;
constexpr int Pepero_Trigger = 3;
constexpr int Light_Trigger = 4;

// KnockBack Vel
constexpr float KnockBack_Vel = 40.f;