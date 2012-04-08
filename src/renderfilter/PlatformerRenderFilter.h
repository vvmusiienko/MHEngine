/*
 * PlatformerRenderFilter.h
 *
 *  Created on: 26.02.2011
 *      Author: Администратор
 */

#ifndef PLATFORMERRENDERFILTER_H_
#define PLATFORMERRENDERFILTER_H_

#include "RenderFilter.h"
#include "../CVector.h"
class BBox;

class PlatformerRenderFilter: public RenderFilter {

	CVector	rest;
	BBox getCorrectedBBox(const BBox& bbox);
public:
	PlatformerRenderFilter(CVector rest);
	std::vector<GameObject*> doFilter(std::vector<GameObject*> objects);
	void debugDraw();
	void debugObjectDraw(GameObject* o);

	virtual ~PlatformerRenderFilter();
};

#endif /* PLATFORMERRENDERFILTER_H_ */
