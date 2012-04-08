#ifndef CAMERA_H_
#define CAMERA_H_

#include "CVector.h"
#include "LuaVM.h"


class Game;

class Camera {

private:
	Game* game;

public:
	
	bool cameraFollowsPlayer;
	CVector position;
	CVector lookAt;
	CVector up;
	
	void update(float dt);

	Camera(Game* game);
	virtual ~Camera();
};

#endif /*CAMERA_H_*/
