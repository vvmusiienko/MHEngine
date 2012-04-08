/*
 * MapProperties.cpp
 *
 *  Created on: 10.03.2010
 *      Author: Администратор
 */

#include "MapProperties.h"
#include "Game.h"
#include "object/GameObject.h"
#include "object/GOSkyBox.h"
#include "GraphicEngine.h"
#include "MapManager.h"

MapProperties::MapProperties(Game* game) {
	this->game = game;
	id = "map";
	type = GO_MAP_PROPS;

	ambientLight = MGColor(0.5f, 0.5f, 0.5f, 1.0f);
	clearColor = MGColor(0.0f, 0.0f, 0.0f, 0.0f);
	skyBox = "background_4.tga";
	enabledLighting = true;

	game->luaVM->doString(id + " = {}");
	game->luaVM->doString("map.getFullID = function() return \"map\" end");
	game->luaVM->registerProperty(id + ".%sid", MG_STRING, &id);
	game->luaVM->registerProperty(id + ".%stype", MG_INT, &type);
	game->luaVM->registerProperty(id + ".%senabledLighting", MG_BOOL, &enabledLighting);
	game->luaVM->registerProperty(id + ".%sskyBox", MG_STRING, &skyBox);
	game->luaVM->registerProperty(id + ".%sambientLight", MG_COLOR, &this->ambientLight);
	game->luaVM->registerProperty(id + ".%sclearColor", MG_COLOR, &this->clearColor);

}


void MapProperties::load(FILE* f) {
	ambientLight = readColor(f);
	clearColor = readColor(f);
	skyBox = readString(f);
	apply();
}


void MapProperties::save(FILE* f) {
	writeChar(type, f);
	writeColor(ambientLight, f);
	writeColor(clearColor, f);
	writeString(skyBox, f);
}


void MapProperties::apply() {
	/*if (enabledLighting) {
		glEnable(GL_LIGHTING);
	} else {
		glDisable(GL_LIGHTING);
	}*/
	game->graphicEngine->setAmbient(ambientLight);
	game->graphicEngine->setClearColor(clearColor);
	if (skyBox != "") {
		game->skyBox = new GOSkyBox(skyBox);
		game->mapManager->addObject(game->skyBox);
	}
}


MapProperties::~MapProperties() {
	// TODO Auto-generated destructor stub
}
