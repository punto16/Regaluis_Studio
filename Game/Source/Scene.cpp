#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "EntityManager.h"
#include "Map.h"
#include "ModuleFadeToBlack.h"
#include "SceneIntro.h"

#include "Defs.h"
#include "Log.h"

Scene::Scene(bool startEnabled) : Module(startEnabled)
{
	name.Create("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;


	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	//AWAKE ACHICOPLAO

	pugi::xml_node configNode = app->LoadConfigFileToVar();
	pugi::xml_node config = configNode.child(name.GetString());

	// iterate all objects in the scene
	// Check https://pugixml.org/docs/quickstart.html#access
	for (pugi::xml_node itemNode = config.child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->parameters = itemNode;
	}

	//L02: DONE 3: Instantiate the player using the entity manager
	player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
	player->parameters = config.child("player");



	//img = app->tex->Load("Assets/Textures/test.png");
	app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");
	
	// L03: DONE: Load map
	app->map->Load(name.GetString());

	if (!app->entityManager->isEnabled)
	{
		app->entityManager->Enable();
	}

	//enable all entities
	app->entityManager->EnableEntities();


	 

	
	// L04: DONE 7: Set the window title with map/tileset info
	/*SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
		app->map->mapData.width,
		app->map->mapData.height,
		app->map->mapData.tileWidth,
		app->map->mapData.tileHeight,
		app->map->mapData.tilesets.Count());

	app->win->SetTitle(title.GetString());*/

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	////if player is below a platform, platform will be able to be passed throw
	//ListItem<PhysBody*>* collisionsItem;
	//collisionsItem = app->map->collisions.start;

	//while (collisionsItem != NULL)
	//{
	//	int colliderpositionY = collisionsItem->data->body->GetPosition().y + collisionsItem->data->height / 2;
	//	if (colliderpositionY < player->position.y + player->getPbody()->height)
	//	{
	//		collisionsItem->data->body->SetActive(false);
	//	}
	//	else
	//	{
	//		collisionsItem->data->body->SetActive(true);
	//	}
	//	collisionsItem = collisionsItem->next;
	//}


	for (b2Body* b = app->physics->world->GetBodyList(); b; b = b->GetNext())
	{
		PhysBody* pB = (PhysBody*)b->GetUserData();
		if (pB->ctype == ColliderType::FLOATINGTERRAIN) {
			int posX = 0;
			int posY = 0;
			pB->GetPosition(posX, posY);
			if (posY < player->position.y + 32) {
				pB->body->SetActive(false);
			}
			else {
				pB->body->SetActive(true);
			}
		}
	}

	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{




	if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		app->render->camera.y += 1;

	if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		app->render->camera.y -= 1;

	if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		app->render->camera.x += 1;
	
	if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		app->render->camera.x -= 1;
	//camera fix to player
	if (player->position.x > 300 && player->position.x < (app->map->mapData.tileWidth * app->map->mapData.width) - 724)
	{
		app->render->camera.x = -(player->position.x - 300);
	}
	else if (player->position.x <= 300)
	{
		app->render->camera.x = 0;
	}
	else if (player->position.x > (app->map->mapData.tileWidth * app->map->mapData.width) - 724)
	{
		app->render->camera.x = -((app->map->mapData.tileWidth * app->map->mapData.width) - 1024);
	}




	// L03: DONE 3: Request App to Load / Save when pressing the keys F5 (save) / F6 (load)
	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
	{
		app->SaveGameRequest();
	}
	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
	{
		app->LoadGameRequest();
	}



	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)//godmode to player
	{
		if (godMode)
		{
			godMode = false;
		}
		else
		{
			godMode = true;
		}
	}


	//app->render->DrawTexture(img, 380, 100); // Placeholder not needed any more



	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)//restart current level
	{
		PhysBody* pbody = player->getPbody();
		//PhysBody* pbodyAura = player->getPbodyAura();
		pbody->SetPosition(player->initialPosition.x, player->initialPosition.y);
		//pbodyAura->SetPosition(player->initialPosition.x, player->initialPosition.y);
	}
	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		PhysBody* pbody = player->getPbody();
		//PhysBody* pbodyAura = player->getPbodyAura();
		pbody->SetPosition(player->initialPosition.x, player->initialPosition.y);
		//pbodyAura->SetPosition(player->initialPosition.x, player->initialPosition.y);
	}

	// Draw map
	app->map->Draw();

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		app->sceneIntro->beforePlay = true;
		app->fade->FadeToBlack(this, (Module*)app->sceneIntro, 60);
	}


	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	//player->Disable();			//MUST BE REPAIRED
	if (app->entityManager->IsEnabled())
	{
		app->entityManager->Disable();
	}

	app->map->CleanUp();

	return true;
}


bool Scene::LoadState(pugi::xml_node& data)
{
	//camera.x = data.child("camera").attribute("x").as_int();
	//camera.y = data.child("camera").attribute("y").as_int();

	PhysBody* pbody = player->getPbody();

	pbody->SetPosition(data.child("player").attribute("x").as_int(), data.child("player").attribute("y").as_int());
	pbody->body->SetLinearVelocity(b2Vec2(data.child("player").attribute("velx").as_int(), data.child("player").attribute("vely").as_int()));
	player->jumpsRemaining = data.child("player").attribute("jumpsRemaining").as_int();

	return true;
}

// L03: DONE 8: Create a method to save the state of the renderer
// using append_child and append_attribute
bool Scene::SaveState(pugi::xml_node& data)
{
	pugi::xml_node playerNode = data.append_child("player");
	
	//cam.append_attribute("x") = camera.x;
	//cam.append_attribute("y") = camera.y;

	playerNode.append_attribute("x") = (player->position.x + 16);
	playerNode.append_attribute("y") = (player->position.y + 16);
	if (abs(player->getPbody()->body->GetLinearVelocity().x) < 0.5)
	{
		playerNode.append_attribute("velx") = (0);
	}
	else
	{
		playerNode.append_attribute("velx") = (player->getPbody()->body->GetLinearVelocity().x);
	}

	playerNode.append_attribute("vely") = (player->getPbody()->body->GetLinearVelocity().y);
	playerNode.append_attribute("jumpsRemaining") = (player->jumpsRemaining);
	
	return true;
}