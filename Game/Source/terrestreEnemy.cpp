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

	initialPosition.x = parameters.attribute("x").as_int();
	initialPosition.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool TerrestreEnemy::Start() {

	texture = app->tex->Load(texturePath);
	currentAnimation = &idleAnimation;

	// L07 DONE 5: Add physics to the player - initialize physics body

	tebody = app->physics->CreateRectangle(initialPosition.x + 14, initialPosition.y + 6, 28,12, bodyType::DYNAMIC);
	tebody->listener = this;
	tebody->ctype = ColliderType::TERRESTREENEMY;
	tebody->body->SetFixedRotation(1);
	app->map->enemies.Add(tebody);

	currentAnimation = &idleAnimation;

	alive = true;
	
	state = STATE::NORMALPATH;

	position.x = initialPosition.x;
	position.y = initialPosition.y;

	return true;
}

bool TerrestreEnemy::Update()
{
	//delete the enemies that has been killed
	if (pendingToSetInactive != nullptr && pendingToSetInactive->body->GetLinearVelocity().y == 0)
	{
		pendingToSetInactive->body->SetActive(false);
		pendingToSetInactive = nullptr;
	}

	PhysBody* pbody = app->scene->player->getPbody();
	b2Vec2 vel;


	if (alive)
	{
		if (pbody->body->GetPosition().x < tebody->body->GetPosition().x)
		{
			vel = tebody->body->GetLinearVelocity() + b2Vec2(0, -GRAVITY_Y * 0.05);
			float32 speed = 5.0f;
			//move to left
			b2Vec2 force = { -speed, 0 };
			tebody->body->ApplyForceToCenter(force, true);
			if (vel.x < -3)
			{
				vel.x = -3;
			}
			currentAnimation = &walkLeftAnimation;
		}
		else if (pbody->body->GetPosition().x > tebody->body->GetPosition().x)
		{
			vel = tebody->body->GetLinearVelocity() + b2Vec2(0, -GRAVITY_Y * 0.05);
			float32 speed = 5.0f;
			//move to left
			b2Vec2 force = { speed, 0 };
			tebody->body->ApplyForceToCenter(force, true);
			if (vel.x > 3)
			{
				vel.x = 3;
			}
			currentAnimation = &walkRightAnimation;
		}
	}
	else if (!alive)
	{
		currentAnimation = &deadAnimation;
		vel = b2Vec2(0, -GRAVITY_Y);
		position.y = METERS_TO_PIXELS(tebody->body->GetTransform().p.y) - 16;
	}
	
	tebody->body->SetLinearVelocity(vel);
	position.x = METERS_TO_PIXELS(tebody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(tebody->body->GetTransform().p.y) - 16;

	//blit sprite at the end
	currentAnimation->Update();
	SDL_Rect rect = currentAnimation->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x, position.y - 9, &rect);







	return true;
}

bool TerrestreEnemy::CleanUp()
{
	LOG("Cleanup of the player");

	return true;
}


void TerrestreEnemy::OnCollision(PhysBody* physA, PhysBody* physB)
{
	if (physA->ctype == ColliderType::TERRESTREENEMY)
	{
		switch (physB->ctype)
		{
		case ColliderType::ITEM:
			LOG("TERRESTRE ENEMY Collision ITEM");
			break;
		case ColliderType::PLATFORM:
			LOG("TERRESTRE ENEMY Collision PLATFORM");
			break;
		case ColliderType::WALL:
			LOG("TERRESTRE ENEMY Collision Wall");
			break;
		case ColliderType::WATER:
			LOG("TERRESTRE ENEMY Collision Water");
			if (!app->scene->godMode)
			{
				alive = false;
			}
			break;
		case ColliderType::VICTORY:
			LOG("TERRESTRE ENEMY Collision Victory");
			break;
		case ColliderType::PLAYER:
			LOG("TERRESTRE ENEMY Collision Player");
			if (app->scene->player->jumping && !app->scene->godMode)
			{
				alive = false;
				pendingToSetInactive = physA;
				//this->Disable();
			}
		case ColliderType::UNKNOWN:
			LOG("TERRESTRE ENEMY Collision UNKNOWN");
			break;
		}
	}
}