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
	name.Create("terrestreEnemy");
}

TerrestreEnemy::~TerrestreEnemy() {

}

bool TerrestreEnemy::Awake() {

	initialPosition.x = parameters.attribute("x").as_int();
	initialPosition.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	//idle anim
	if (idleAnimation.GetTotalFrames() == 0)
	{
		for (pugi::xml_node nodePlayer = parameters.child("idleAnim").child("pushback");
			nodePlayer; nodePlayer = nodePlayer.next_sibling("pushback"))
		{
			idleAnimation.PushBack({	nodePlayer.attribute("x").as_int(),
										nodePlayer.attribute("y").as_int(),
										nodePlayer.attribute("w").as_int(),
										nodePlayer.attribute("h").as_int() });
		}
	}
	//walk left anim
	if (walkLeftAnimation.GetTotalFrames() == 0)
	{
		for (pugi::xml_node nodePlayer = parameters.child("walkLeftAnim").child("pushback");
			nodePlayer; nodePlayer = nodePlayer.next_sibling("pushback"))
		{
			walkLeftAnimation.PushBack({	nodePlayer.attribute("x").as_int(),
											nodePlayer.attribute("y").as_int(),
											nodePlayer.attribute("w").as_int(),
											nodePlayer.attribute("h").as_int() });
		}
		walkLeftAnimation.speed = parameters.child("walkLeftAnim").attribute("speed").as_float();
		walkLeftAnimation.loop = parameters.child("walkLeftAnim").attribute("loop").as_bool();
	}
	//walk right anim
	if (walkRightAnimation.GetTotalFrames() == 0)
	{
		for (pugi::xml_node nodePlayer = parameters.child("walkRightAnim").child("pushback");
			nodePlayer; nodePlayer = nodePlayer.next_sibling("pushback"))
		{
			walkRightAnimation.PushBack({	nodePlayer.attribute("x").as_int(),
											nodePlayer.attribute("y").as_int(),
											nodePlayer.attribute("w").as_int(),
											nodePlayer.attribute("h").as_int() });
		}
		walkRightAnimation.speed = parameters.child("walkRightAnim").attribute("speed").as_float();
		walkRightAnimation.loop = parameters.child("walkRightAnim").attribute("loop").as_bool();
	}
	//attack left anim
	if (attackLeftAnimation.GetTotalFrames() == 0)
	{
		for (pugi::xml_node nodePlayer = parameters.child("attackLeftAnim").child("pushback");
			nodePlayer; nodePlayer = nodePlayer.next_sibling("pushback"))
		{
			attackLeftAnimation.PushBack({	nodePlayer.attribute("x").as_int(),
											nodePlayer.attribute("y").as_int(),
											nodePlayer.attribute("w").as_int(),
											nodePlayer.attribute("h").as_int() });
		}
		attackLeftAnimation.speed = parameters.child("attackLeftAnim").attribute("speed").as_float();
		attackLeftAnimation.loop = parameters.child("attackLeftAnim").attribute("loop").as_bool();
	}
	//attack right anim
	if (attackRightAnimation.GetTotalFrames() == 0)
	{
		for (pugi::xml_node nodePlayer = parameters.child("attackRightAnim").child("pushback");
			nodePlayer; nodePlayer = nodePlayer.next_sibling("pushback"))
		{
			attackRightAnimation.PushBack({	nodePlayer.attribute("x").as_int(),
											nodePlayer.attribute("y").as_int(),
											nodePlayer.attribute("w").as_int(),
											nodePlayer.attribute("h").as_int() });
		}
		attackRightAnimation.speed = parameters.child("attackRightAnim").attribute("speed").as_float();
		attackRightAnimation.loop = parameters.child("attackRightAnim").attribute("loop").as_bool();
	}
	//dead anim
	if (deadAnimation.GetTotalFrames() == 0)
	{
		for (pugi::xml_node nodePlayer = parameters.child("deadAnim").child("pushback");
			nodePlayer; nodePlayer = nodePlayer.next_sibling("pushback"))
		{
			deadAnimation.PushBack({	nodePlayer.attribute("x").as_int(),
										nodePlayer.attribute("y").as_int(),
										nodePlayer.attribute("w").as_int(),
										nodePlayer.attribute("h").as_int() });
		}
	}

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
	if (!app->physics->pause)
	{
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
			if (tebody->body->GetLinearVelocity().x == 0)
			{
				currentAnimation = &idleAnimation;
			}
		}
		else if (!alive)
		{
			currentAnimation = &deadAnimation;
			vel = b2Vec2(0, -GRAVITY_Y);
			position.y = METERS_TO_PIXELS(tebody->body->GetTransform().p.y) - 16;
			tebody->body->SetActive(false);
		}

		tebody->body->SetLinearVelocity(vel);
		position.x = METERS_TO_PIXELS(tebody->body->GetTransform().p.x) - 16;
		position.y = METERS_TO_PIXELS(tebody->body->GetTransform().p.y) - 16;

		//blit sprite at the end
		currentAnimation->Update();
	}
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
			int py, tey;
			py = METERS_TO_PIXELS(physB->body->GetPosition().y);
			tey = METERS_TO_PIXELS(physA->body->GetPosition().y);

			if (!app->scene->godMode &&
				alive &&
				app->scene->player->alive &&
				py + physB->height + 3 <= tey)
			{
				alive = false;
				physB->body->SetLinearVelocity(b2Vec2(0, -20.0f));

			}
			else if (	!app->scene->godMode && 
						app->scene->player->alive && 
						alive &&
						tey < physA->height + 4 + py) 
			{
				app->scene->player->alive = false;
				app->sceneIntro->Win = false;
				app->sceneIntro->beforePlay = false;
				app->fade->FadeToBlack(app->scene, (Module*)app->sceneIntro, 20);
			}
		case ColliderType::UNKNOWN:
			LOG("TERRESTRE ENEMY Collision UNKNOWN");
			break;
		}
	}
}