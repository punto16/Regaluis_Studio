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

Player::Player(bool startEnabled) : Entity(EntityType::PLAYER)
{
	name.Create("player");

}

Player::~Player() {

}

bool Player::Awake() {

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

	//jump left anim
	if (jumpLeftAnimation.GetTotalFrames() == 0)
	{
		for (pugi::xml_node nodePlayer = parameters.child("jumpLeftAnim").child("pushback");
			nodePlayer; nodePlayer = nodePlayer.next_sibling("pushback"))
		{
			jumpLeftAnimation.PushBack({	nodePlayer.attribute("x").as_int(),
											nodePlayer.attribute("y").as_int(),
											nodePlayer.attribute("w").as_int(),
											nodePlayer.attribute("h").as_int() });
		}
	}

	//jump right anim
	if (jumpRightAnimation.GetTotalFrames() == 0)
	{
		for (pugi::xml_node nodePlayer = parameters.child("jumpRightAnim").child("pushback");
			nodePlayer; nodePlayer = nodePlayer.next_sibling("pushback"))
		{
			jumpRightAnimation.PushBack({	nodePlayer.attribute("x").as_int(),
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


	jumping = false;

	return true;
}

bool Player::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	currentAnimation = &idleAnimation;
	
	alive = true;
	winning = false;

	// L07 DONE 5: Add physics to the player - initialize physics body
	pbody = app->physics->CreateCircle(initialPosition.x + 15, initialPosition.y + 15, 15, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;
	app->map->player.Add(pbody);

	//physics of the player "aura"
	/*pbodyAura = app->physics->CreateRectangleSensor(initialPosition.x + 14, initialPosition.y + 14, 40 ,30, bodyType::DYNAMIC);
	pbodyAura->listener = this;
	pbodyAura->ctype = ColliderType::PLAYERAURA;*/


		 
	//initialize audio effect - !! Path is hardcoded, should be loaded from config.xml
	pickCoinFxId = app->audio->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");
	position.x = initialPosition.x;
	position.y = initialPosition.y;

	jumpsRemaining = MAXJUMPS;

	return true;
}

bool Player::Update()
{
	// L07 DONE 5: Add physics to the player - updated player position using physics
	if (!app->physics->pause)
	{
		float32 speed = 50;

		b2Vec2 vel;

		//vel = b2Vec2(0, -GRAVITY_Y - jumpForce);

		if (app->scene->godMode)
		{
			pbody->body->SetGravityScale(0);
			vel = b2Vec2(0, 0);
			alive = true;
			speed = 600.0f;
		}
		else
		{
			vel = pbody->body->GetLinearVelocity() + b2Vec2(0, -GRAVITY_Y * 0.05);
			pbody->body->SetGravityScale(1);
		}

		if (alive && !winning)
		{
			////L02: DONE 4: modify the position of the player using arrow keys and render the texture

			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			{
				app->scene->fixedCamera = true;
				b2Vec2 force = { -speed, 0 };
				pbody->body->ApplyForceToCenter(force, true);
				if (vel.x < -8)
				{
					vel.x = -8;
				}
				if (jumping)
				{
					currentAnimation = &jumpLeftAnimation;
				}
				else
				{
					currentAnimation = &walkLeftAnimation;
				}
			}
			if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			{
				app->scene->fixedCamera = true;
				b2Vec2 force = { speed, 0 };
				pbody->body->ApplyForceToCenter(force, true);
				if (vel.x > 8)
				{
					vel.x = 8;
				}
				if (jumping)
				{
					currentAnimation = &jumpRightAnimation;
				}
				else
				{
					currentAnimation = &walkRightAnimation;
				}
			}

			//with godmode
			if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT && app->scene->godMode)
			{
				app->scene->fixedCamera = true;
				b2Vec2 force = { 0, speed };
				pbody->body->ApplyForceToCenter(force, true);
				if (vel.y < -30)
				{
					vel.y = -30;
				}
			}
			if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT && app->scene->godMode)
			{
				app->scene->fixedCamera = true;
				b2Vec2 force = { 0, -speed };
				pbody->body->ApplyForceToCenter(force, true);
				if (vel.y > 30)
				{
					vel.y = 30;
				}
				pbody->GetRotation();
			}

			//jump
			if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && !app->scene->godMode && jumpsRemaining > 0)
			{
				app->scene->fixedCamera = true;
				vel.y = -20.0f;
				jumping = true;
				jumpsRemaining--;
			}

			//set idle animation if not moving
			if (app->input->GetKey(SDL_SCANCODE_A) == KeyState::KEY_IDLE &&
				app->input->GetKey(SDL_SCANCODE_D) == KeyState::KEY_IDLE &&
				!jumping)
			{
				currentAnimation = &idleAnimation;
				vel.x = 0;
			}

			//Set the velocity of the pbody of the player
			pbody->body->SetLinearVelocity(vel);

			//sets aura at the same position as player hitbo
			//pbody->GetPosition(playerAuraX,playerAuraY);
			//pbodyAura->SetPosition(playerAuraX + 7, playerAuraY);

			//Update player position in pixels
			position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
			position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;


		}
		else if (!alive)
		{
			currentAnimation = &deadAnimation;
			pbody->body->SetActive(false);
		}
		else if (winning)
		{
			currentAnimation = &idleAnimation;
			vel = b2Vec2(0, -GRAVITY_Y);
			pbody->body->SetLinearVelocity(vel);
			position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;
		}

		//updates animation and draws it
		currentAnimation->Update();

	}
	SDL_Rect rect = currentAnimation->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x, position.y + 2, &rect);

	if (!alive || winning && pbody->body->GetLinearVelocity().y == 0)
	{
		app->physics->pause = true;
	}

	return true;
}

bool Player::CleanUp()
{
	LOG("Cleanup of the player");

	app->tex->UnLoad(texture);
	
	return true;
}


void Player::OnCollision(PhysBody* physA, PhysBody* physB)
{
	if (physA->ctype == ColliderType::PLAYER)
	{
		switch (physB->ctype)
		{
		case ColliderType::ITEM:
			LOG("Collision ITEM");
			app->audio->PlayFx(pickCoinFxId);
			break;
		case ColliderType::PLATFORM:
			LOG("Collision PLATFORM");
			jumping = false;
			jumpsRemaining = MAXJUMPS;
			break;
		case ColliderType::WALL:
			LOG("Collision Wall");
			break;
		case ColliderType::FLOATINGTERRAIN:
			LOG("Collision FloatingTerrain");
			break;
		case ColliderType::WATER:
			LOG("Collision Water");
			if (!app->scene->godMode)
			{
				alive = false;
				app->sceneIntro->Win = false;
				app->sceneIntro->beforePlay = false;
				app->fade->FadeToBlack(app->scene, (Module*)app->sceneIntro, 20);
			}
			break;
		case ColliderType::VICTORY:
			LOG("Collision Victory");
			app->sceneIntro->currentLevel++;
			if (app->sceneIntro->currentLevel >= MAX_LEVELS)
			{
				app->sceneIntro->currentLevel = 0;
			}
			winning = true;
			app->sceneIntro->Win = true;
			app->sceneIntro->beforePlay = false;
			app->fade->FadeToBlack(app->scene, (Module*)app->sceneIntro, 20);
			break;
		case ColliderType::TERRESTREENEMY:
			LOG("Collision TerrestreEnemy");
		case ColliderType::UNKNOWN:
			LOG("Collision UNKNOWN");
			break;
		}
	}
}

void Player::EndCollision(PhysBody* physA, PhysBody* physB)
{
	if (physA->ctype == ColliderType::PLAYER)
	{
		switch (physB->ctype)
		{
		case ColliderType::ITEM:
			LOG("ENDCollision ITEM");
			break;
		case ColliderType::PLATFORM:
			LOG("ENDCollision PLATFORM");
			jumping = true;
			if (jumpsRemaining == MAXJUMPS)
			{
				jumpsRemaining = MAXJUMPS - 1;
			}
			break;
		case ColliderType::UNKNOWN:
			LOG("ENDCollision UNKNOWN");
			break;
		}
	}
}