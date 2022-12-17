#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Player.h"
#include "Item.h"
#include "terrestreEnemy.h"
#include "flyingEnemy.h"
#include "Point.h"

#define CAMERASPEED 5

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene(bool startEnabled);

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool LoadState(pugi::xml_node& data);

	bool SaveState(pugi::xml_node& data);

public:

	//L02: DONE 3: Declare a Player attribute 
	Player* player;

	//terrestre enemies list
	List<TerrestreEnemy*> terrestreEnemies;

	//flying enemies list
	List<FlyingEnemy*> flyingEnemies;

	//ray
	Point<int> ray;

	//window data
	uint width, height, scale;

	int blackFont = -1;
	int whiteFont = -1;

	bool godMode = false;
	bool fixedCamera = true;

private:
	SDL_Texture* img;

	// Texture to highligh mouse position 
	SDL_Texture* mouseTileTex;
	SDL_Texture* originTex;

	// L12: Debug pathfing
	iPoint origin;
	bool originSelected = false;

};

#endif // __SCENE_H__