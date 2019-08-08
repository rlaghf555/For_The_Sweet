#pragma once
constexpr float GRAVITY = 9.81f;
constexpr float MAP_BOTTOM_HEIGHT = 10.1f;

constexpr float CH_HALF_HEIGHT = 17.5f;
constexpr float CH_CAPSULE_HEIGHT = 15.f;
constexpr float CH_CAPSULE_RADIUS = 10.f;

constexpr int MAX_WEAPON_TYPE = 5;
constexpr int MAX_WEAPON_NUM = 30;
constexpr int RESPAWN_WEAPON_NUM = 10;

constexpr int MAX_ANIM = 29;
constexpr int MAX_HP = 100;

// ROOM
constexpr int MAX_ROOM_NAME_LEN = 10;
constexpr int MAX_ROOM_USER = 8;

constexpr int ROOM_MODE_TEAM = 0;
constexpr int ROOM_MODE_INDIVIDUAL = 1;
constexpr int ROOM_MOVE_KING = 2;

constexpr int ROOM_TIMER_START = 1000;

// Weapon
constexpr int Weapon_Lollipop = 0;
constexpr int Weapon_chupachupse = 1;
constexpr int Weapon_pepero = 2;
constexpr int Weapon_chocolate = 3;
constexpr int Weapon_cupcake = 4;

// Pepero
constexpr float Pepero_Width = 1.f;
constexpr float Pepero_Length = 18.f;
constexpr float Pepero_Vel = 150.f;
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


// Trigger_Type
constexpr int Player_Trigger = 0;
constexpr int Lollipop_Trigger = 1;
constexpr int Candy_Trigger = 2;
constexpr int Pepero_Trigger = 3;

// KnockBack Vel
constexpr float KnockBack_Vel = 40.f;