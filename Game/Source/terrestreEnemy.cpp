#include "terrestreEnemy.h"
#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "SceneIntro.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "ModuleFadeToBlack.h"
#include "Map.h"

TerrestreEnemy::TerrestreEnemy(bool startEnabled) : Entity(EntityType::TERRESTREENEMY)
{
	name.Create("TerrestreEnemy");

	//animations
	idleAnimation.PushBack({ 0,0,32,32 });

	walkLeftAnimation.PushBack({ 32 * 1, 32 * 0, 32, 32 });
	walkLeftAnimation.PushBack({ 32 * 2, 32 * 0, 32, 32 });
	walkLeftAnimation.PushBack({ 32 * 3, 32 * 0, 32, 32 });
	walkLeftAnimation.PushBack({ 32 * 4, 32 * 0, 32, 32 });
	walkLeftAnimation.PushBack({ 32 * 5, 32 * 0, 32, 32 });
	walkLeftAnimation.PushBack({ 32 * 6, 32 * 0, 32, 32 });
	walkLeftAnimation.PushBack({ 32 * 0, 32 * 0, 32, 32 });
	walkLeftAnimation.pingpong = false;
	walkLeftAnimation.loop = true;
	walkLeftAnimation.speed = 0.1f;

	walkRightAnimation.PushBack({ 32 * 1, 32 * 2, 32, 32 });
	walkRightAnimation.PushBack({ 32 * 2, 32 * 2, 32, 32 });
	walkRightAnimation.PushBack({ 32 * 3, 32 * 2, 32, 32 });
	walkRightAnimation.PushBack({ 32 * 4, 32 * 2, 32, 32 });
	walkRightAnimation.PushBack({ 32 * 5, 32 * 2, 32, 32 });
	walkRightAnimation.PushBack({ 32 * 6, 32 * 2, 32, 32 });
	walkRightAnimation.PushBack({ 32 * 0, 32 * 2, 32, 32 });
	walkRightAnimation.pingpong = false;
	walkRightAnimation.loop = true;
	walkRightAnimation.speed = 0.1f;

	attackLeftAnimation.PushBack({ 32 * 1, 32 * 1, 32, 32 });
	attackLeftAnimation.PushBack({ 32 * 2, 32 * 1, 32, 32 });
	attackLeftAnimation.PushBack({ 32 * 3, 32 * 1, 32, 32 });
	attackLeftAnimation.PushBack({ 32 * 4, 32 * 1, 32, 32 });
	walkRightAnimation.pingpong = false;
	walkRightAnimation.loop = true;
	walkRightAnimation.speed = 0.1f;


	attackLeftAnimation.PushBack({ 32 * 1, 32 * 3, 32, 32 });
	attackLeftAnimation.PushBack({ 32 * 2, 32 * 3, 32, 32 });
	attackLeftAnimation.PushBack({ 32 * 3, 32 * 3, 32, 32 });
	attackLeftAnimation.PushBack({ 32 * 4, 32 * 3, 32, 32 });
	walkRightAnimation.pingpong = false;
	walkRightAnimation.loop = true;
	walkRightAnimation.speed = 0.1f;

	deadAnimation.PushBack({ 32 * 6, 32 * 3, 32, 32 });
}

TerrestreEnemy::~TerrestreEnemy() {

}

bool TerrestreEnemy::Awake() {



	return true;
}

bool TerrestreEnemy::Start() {

	texture = app->tex->Load(texturePath);
	currentAnimation = &idleAnimation;

	alive = true;

	// L07 DONE 5: Add physics to the player - initialize physics body
	tebody = app->physics->CreateRectangle(initialPosition.x + 14, initialPosition.y + 6, 28,12, bodyType::DYNAMIC);
	tebody->listener = this;
	tebody->ctype = ColliderType::TERRESTREENEMY;
	app->map->enemies.Add(tebody);



	position.x = initialPosition.x;
	position.y = initialPosition.y;

	return true;
}

bool TerrestreEnemy::Update()
{
	
	return true;
}

bool TerrestreEnemy::CleanUp()
{
	LOG("Cleanup of the player");

	return true;
}


void TerrestreEnemy::OnCollision(PhysBody* physA, PhysBody* physB)
{

}