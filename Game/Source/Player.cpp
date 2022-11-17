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

Player::Player(bool startEnabled) : Entity(EntityType::PLAYER)
{
	name.Create("Player");
	
	//animations
	idleAnimation.PushBack({ 0,0,30,30 });

	jumpLeftAnimation.PushBack({ 30,0,30,30 });
	
	jumpRightAnimation.PushBack({ 60,0,30,30 });

	walkLeftAnimation.PushBack({ 90,0,30,30 });
	walkLeftAnimation.PushBack({ 120,0,30,30 });
	walkLeftAnimation.PushBack({ 0,30,30,30 });
	walkLeftAnimation.PushBack({ 120,0,30,30 });
	walkLeftAnimation.pingpong = false;
	walkLeftAnimation.loop = true;
	walkLeftAnimation.speed = 0.1f;

	walkRightAnimation.PushBack({ 30,30,30,30 });
	walkRightAnimation.PushBack({ 60,30,30,30 });
	walkRightAnimation.PushBack({ 90,30,30,30 });
	walkRightAnimation.PushBack({ 60,30,30,30 });
	walkRightAnimation.pingpong = false;
	walkRightAnimation.loop = true;
	walkRightAnimation.speed = 0.1f;

	deadAnimation.PushBack({ 120,30,30,30 });
}

Player::~Player() {

}

bool Player::Awake() {

	//L02: DONE 1: Initialize Player parameters
	//pos = position;
	//texturePath = "Assets/Textures/player/idle1.png";

	//L02: DONE 5: Get Player parameters from XML
	initialPosition.x = parameters.attribute("x").as_int();
	initialPosition.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	jumping = false;

	return true;
}

bool Player::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	currentAnimation = &idleAnimation;
	
	alive = true;

	// L07 DONE 5: Add physics to the player - initialize physics body
	pbody = app->physics->CreateCircle(initialPosition.x + 15, initialPosition.y + 15, 15, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;

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

	if (alive)
	{
		////L02: DONE 4: modify the position of the player using arrow keys and render the texture

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {

			b2Vec2 force = { -speed, 0 };
			pbody->body->ApplyForceToCenter(force, true);
			if (vel.x < -10)
			{
				vel.x = -10;
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
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			b2Vec2 force = { speed, 0 };
			pbody->body->ApplyForceToCenter(force, true);
			if (vel.x > 10)
			{
				vel.x = 10;
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
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT && app->scene->godMode) {

			b2Vec2 force = { 0, speed };
			pbody->body->ApplyForceToCenter(force, true);
			if (vel.y < -30)
			{
				vel.y = -30;
			}
		}
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT && app->scene->godMode) {
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
	}

	


	//updates animation and draws it
	currentAnimation->Update();
	SDL_Rect rect = currentAnimation->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x, position.y + 2, &rect);

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
	//CUANDO HAGA COLLIDERS PARA EL SALTO, FIXEAR LOS SALTOS POCHOS DE UPDATE Y REPARAR ANIMACIONES DE SALTO
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
		case ColliderType::WATER:
			LOG("Collision Water");
			if (!app->scene->godMode)
			{
				alive = false;
				app->sceneIntro->Win = false;
				app->sceneIntro->beforePlay = false;
				app->fade->FadeToBlack(app->scene, (Module*)app->sceneIntro, 60);
			}
			break;
		case ColliderType::VICTORY:
			LOG("Collision Victory");
			app->sceneIntro->Win = true;
			app->sceneIntro->beforePlay = false;
			app->fade->FadeToBlack(app->scene, (Module*)app->sceneIntro, 60);
			break;
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