#include "Camera.h"
#include "Game.h"
#include "object/GOPlayer.h"
#include "MGConfig.h"

Camera::Camera(Game* game) {
	this->game = game;
	position = game->config->getVector("camera_position");
	lookAt = game->config->getVector("camera_lookAt");
	up = game->config->getVector("camera_up");

	game->luaVM->registerProperty("%scamera_position", MG_VECTOR, &position);
	game->luaVM->registerProperty("%scamera_lookAt", MG_VECTOR, &lookAt);
	game->luaVM->registerProperty("%scamera_up", MG_VECTOR, &up);
}


void Camera::update(float dt) {
    if (cameraFollowsPlayer) {
    	if (game->player) {
        	if (game->player->getPosition().x - position.x > 10)
        		position.x = game->player->getPosition().x - 10;
        	if (game->player->getPosition().x - position.x < -20)
        		position.x = game->player->getPosition().x + 20;

        	if (game->player->getPosition().y + 10 - position.y > 23)
        		position.y = game->player->getPosition().y + 10 - 23;
        	if (game->player->getPosition().y + 10 - position.y < -25)
        		position.y = game->player->getPosition().y + 10 + 25;
    		position.z = 260;
    	}
    } else {

    	float k = 6 * dt;
    	if(game->keys[87]) {
    		position += lookAt.UnitVector() * k;
    	}
    	if(game->keys[83]) {
    		position -= lookAt.UnitVector() * k;
    	}
        if(game->keys[65]) {
            position.x += lookAt.UnitVector().z * k;
            position.z -= lookAt.UnitVector().x * k;
        }
        if(game->keys[68]) {
            position.x -= lookAt.UnitVector().z * k;
            position.z += lookAt.UnitVector().x * k;
        }
    	if(game->keys[32]) {
    		position.y += 1 * k;
    	}
        if(game->keys[17]) {
        	position.y -= 1 * k;
        }
    }
}


Camera::~Camera() {
}
