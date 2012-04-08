/*
 * RenderFilter.h
 *
 *  Created on: 26.02.2011
 *      Author: Администратор
 */

#ifndef RENDERFILTER_H_
#define RENDERFILTER_H_

#include <vector>
class GameObject;

class RenderFilter {

protected:
	RenderFilter();

public:

	virtual std::vector<GameObject*> doFilter(std::vector<GameObject*> objects) = 0;
	virtual void debugObjectDraw(GameObject* o) = 0;
	virtual void debugDraw() = 0;

	virtual ~RenderFilter();
};

#endif /* RENDERFILTER_H_ */
