#ifndef __SCENEINTRO_H__
#define __SCENEINTRO_H__

#include "Module.h"
#include "Player.h"
#include "Item.h"

struct SDL_Texture;

class SceneIntro : public Module
{
public:

	SceneIntro(bool startEnabled);

	// Destructor
	virtual ~SceneIntro();

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

public:

	bool beforePlay = true;
	bool Win;
	//CURRENT LEVEL THE PLAYER IS IN, REMEMBER TO SUBSTRACT 1 (IF PLAYER IS IN LEVEL 1, CURRENT LEVEL WILL BE 0)
	int currentLevel = 0;

private:

	SDL_Texture* StartImage;
	SDL_Texture* WinImage;
	SDL_Texture* LoseImage;

};

#endif // __SCENE_H__