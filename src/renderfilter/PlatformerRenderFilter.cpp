/*
 * PlatformerRenderFilter.cpp
 *
 *  Created on: 26.02.2011
 *      Author: Администратор
 */

#include "PlatformerRenderFilter.h"
#include "../object/GameObject.h"
#include "../Camera.h"
#include "../GraphicEngine.h"

using namespace std;

PlatformerRenderFilter::PlatformerRenderFilter(CVector rest) {
	this->rest = rest;
}


BBox PlatformerRenderFilter::getCorrectedBBox(const BBox& bbox) {
	CVector Bul = bbox.upperLeft;
	CVector Blr = bbox.lowerRight;
	const float max_aditional_width = 300.0f;
	const float max_aditional_height = 300.0f;

	float xc = -Bul.z / 600.0f;
	if (xc > 1.0f) {
		xc = 1.0f;
	}
	if (xc < 0.0f) {
		xc = 0.0f;
	}

	CVector corretive(max_aditional_width * xc, -max_aditional_height * xc,  0);

	Bul -= corretive;
	Blr += corretive;

	return BBox(Bul, Blr);
}


vector<GameObject*> PlatformerRenderFilter::doFilter(vector<GameObject*> objects) {

	static vector<GameObject*> toDraw;
	toDraw.clear();

	CVector Aul = CVector(Game::instance->camera->position.x - (rest.x / 2.0f),Game::instance->camera->position.y + (rest.y / 2.0f)) ;
	CVector Alr = CVector(Game::instance->camera->position.x + (rest.x / 2.0f),Game::instance->camera->position.y - (rest.y / 2.0f)) ;

	for (vector<GameObject*>::iterator i = objects.begin(); i != objects.end(); i++) {
		BBox corrected = getCorrectedBBox((*i)->getBBox());
		// first upper left corner
		CVector Bul = corrected.upperLeft;
		CVector Blr = corrected.lowerRight;

		if ((Aul.y < Blr.y) || (Alr.y > Bul.y) || (Aul.x > Blr.x) || (Alr.x < Bul.x))
	    { } else {
			toDraw.push_back(*i);
	    }

/*	    if ((*i)->type == GO_CUBE) {
	    	Game::instance->graphicEngine->writeDebugText("lft %f, rig %f, tp %f, bot %f",
			left1, right1, top1, bottom1);
	    	Game::instance->graphicEngine->writeDebugText("lft %f, rig %f, tp %f, bot %f",
			left2, right2, top2, bottom2);
	    }*/

	}

	return toDraw;
}


void PlatformerRenderFilter::debugDraw() {
	CVector Aul = CVector(Game::instance->camera->position.x - (rest.x / 2.0f),Game::instance->camera->position.y + (rest.y / 2.0f)) ;
	CVector Alr = CVector(Game::instance->camera->position.x + (rest.x / 2.0f),Game::instance->camera->position.y - (rest.y / 2.0f)) ;
	Game::instance->graphicEngine->drawBBox(BBox(Aul, Alr));
}


void PlatformerRenderFilter::debugObjectDraw(GameObject* o) {

	Game::instance->graphicEngine->drawBBox(getCorrectedBBox(o->getBBox()));
}


PlatformerRenderFilter::~PlatformerRenderFilter() {
	// TODO Auto-generated destructor stub
}
