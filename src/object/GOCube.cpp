#include "GOCube.h"
#include "../Common.h"
#include <stdexcept>

GOCube::GOCube() : GameObject() {
	init();
}


GOCube::GOCube(string id): GameObject(id) {
	init();
	bindLua();
}


GOCube::GOCube(FILE* f) : GameObject(f) {
	try {
		init();
		bindLua();
		size = readVector(f);
	} catch (std::runtime_error e) {
		throw std::runtime_error(FormatString("GOCube::GOCube(FILE* f): Can't create object from stream! Reason:\n%s", e.what()));
	}
}


void GOCube::save(FILE* f) {
	GameObject::save(f);
	writeVector(size, f);
}


void GOCube::init() {
	type = GO_CUBE;
	verticesBuff = NULL;
}

void GOCube::onWillRemoveFromWorld() {

}


void GOCube::bindLua() {
	registrateMethods(getFullID());
}


void GOCube::addVertex(int trinum, float x, float y, float z, float nx, float ny, float nz) {
	verticesBuff[trinum*2+0][0] = x;
	verticesBuff[trinum*2+0][1] = y;
	verticesBuff[trinum*2+0][2] = z;
	verticesBuff[trinum*2+1][0] = nx;
	verticesBuff[trinum*2+1][1] = ny;
	verticesBuff[trinum*2+1][2] = nz;

}


void GOCube::refreshBuffers() {
	GameObject::refreshBuffers();

	// for centering
	float offsetX = size.x / 2;
	float offsetY = size.y / 2;
	float offsetZ = size.z / 2;

	if (verticesBuff == NULL) {
		//verticesBuff = new vec3f[36];

		// allocate a new buffer
		glGenBuffers(1, &VBOBuffer);
		// bind the buffer object to use
		glBindBuffer(GL_ARRAY_BUFFER, VBOBuffer);
		// allocate enough space for the VBO
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3f) * 36 * 2, 0, GL_STATIC_DRAW);
	} else {
		glBindBuffer(GL_ARRAY_BUFFER, VBOBuffer);
	}

	verticesBuff = (vec3f*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

	addVertex(0, 0.0 - offsetX, size.y - offsetY, size.z - offsetZ, 0.0f, 0.0f, 1.0f);
	addVertex(1, 0.0 - offsetX, 0.0 - offsetY, size.z - offsetZ, 0.0f, 0.0f, 1.0f);
	addVertex(2, size.x - offsetX, 0.0 - offsetY, size.z - offsetZ, 0.0f, 0.0f, 1.0f);
	addVertex(3, size.x - offsetX, 0.0 - offsetY, size.z - offsetZ, 0.0f, 0.0f, 1.0f);
	addVertex(4, size.x - offsetX, size.y - offsetY, size.z - offsetZ, 0.0f, 0.0f, 1.0f);
	addVertex(5, 0.0 - offsetX, size.y - offsetY, size.z - offsetZ, 0.0f, 0.0f, 1.0f);

	addVertex(6, -offsetX, size.y - offsetY, -offsetZ, 0.0f, 0.0f, -1.0f);
	addVertex(7, size.x - offsetX, size.y - offsetY, -offsetZ, 0.0f, 0.0f, -1.0f);
	addVertex(8, size.x - offsetX, -offsetY, -offsetZ, 0.0f, 0.0f, -1.0f);
	addVertex(9, size.x - offsetX, -offsetY, -offsetZ, 0.0f, 0.0f, -1.0f);
	addVertex(10, -offsetX, -offsetY, -offsetZ, 0.0f, 0.0f, -1.0f);
	addVertex(11,  -offsetX, size.y - offsetY, -offsetZ, 0.0f, 0.0f, -1.0f);

	addVertex(12, size.x - offsetX, -offsetY, size.z - offsetZ, 1.0f, 0.0f, 0.0f);
	addVertex(13, size.x - offsetX, -offsetY, -offsetZ, 1.0f, 0.0f, 0.0f);
	addVertex(14, size.x - offsetX, size.y - offsetY, size.z - offsetZ, 1.0f, 0.0f, 0.0f);
	addVertex(15, size.x - offsetX, -offsetY, -offsetZ, 1.0f, 0.0f, 0.0f);
	addVertex(16, size.x - offsetX, size.y - offsetY, -offsetZ, 1.0f, 0.0f, 0.0f);
	addVertex(17, size.x - offsetX, size.y - offsetY, size.z - offsetZ, 1.0f, 0.0f, 0.0f);

	addVertex(18, -offsetX, size.y - offsetY, size.z - offsetZ, -1.0f, 0.0f, 0.0f);
	addVertex(19, -offsetX, size.y - offsetY, -offsetZ, -1.0f, 0.0f, 0.0f);
	addVertex(20, -offsetX, -offsetY, -offsetZ, -1.0f, 0.0f, 0.0f);
	addVertex(21, -offsetX, size.y - offsetY, size.z - offsetZ, -1.0f, 0.0f, 0.0f);
	addVertex(22, -offsetX, -offsetY, -offsetZ, -1.0f, 0.0f, 0.0f);
	addVertex(23, -offsetX, -offsetY, size.z - offsetZ, -1.0f, 0.0f, 0.0f);

	addVertex(24, size.x - offsetX, size.y - offsetY, -offsetZ, 0.0f, 1.0f, 0.0f);
	addVertex(25, -offsetX, size.y - offsetY, size.z - offsetZ, 0.0f, 1.0f, 0.0f);
	addVertex(26, size.x - offsetX, size.y - offsetY, size.z - offsetZ, 0.0f, 1.0f, 0.0f);
	addVertex(27, -offsetX, size.y - offsetY, size.z - offsetZ, 0.0f, 1.0f, 0.0f);
	addVertex(28, size.x - offsetX, size.y - offsetY, -offsetZ, 0.0f, 1.0f, 0.0f);
	addVertex(29, -offsetX, size.y - offsetY, -offsetZ, 0.0f, 1.0f, 0.0f);

	addVertex(30, -offsetX, -offsetY, -offsetZ, 0.0f, -1.0f, 0.0f);
	addVertex(31, size.x - offsetX, -offsetY, -offsetZ, 0.0f, -1.0f, 0.0f);
	addVertex(32, -offsetX, -offsetY, size.z - offsetZ, 0.0f, -1.0f, 0.0f);
	addVertex(33, size.x - offsetX, -offsetY, size.z - offsetZ, 0.0f, -1.0f, 0.0f);
	addVertex(34, -offsetX, -offsetY, size.z - offsetZ, 0.0f, -1.0f, 0.0f);
	addVertex(35, size.x - offsetX, -offsetY, -offsetZ, 0.0f, -1.0f, 0.0f);

	//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3f) * 36, verticesBuff);
	glUnmapBuffer(GL_ARRAY_BUFFER);
}



void GOCube::draw() {
	glPushMatrix();
	GameObject::draw();

	// save previous state of texturing
	bool texturingEnabled = glIsEnabled(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_2D);

	glBindBuffer(GL_ARRAY_BUFFER, VBOBuffer);         // for vertex coordinates

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3/*size of one vertex*/,
					GL_FLOAT/*type of each component*/,
					sizeof(vec3f)*2/*is the byte offset between vertices*/,
					BUFFER_OFFSET(0)/*no comments:P*/);
	glNormalPointer(GL_FLOAT/*type of each component*/,
					sizeof(vec3f) * 2/*is the byte offset between vertices*/,
					BUFFER_OFFSET(12)/*no comments:P*/);

	glDrawArrays(GL_TRIANGLES/*type of vertices*/, 0/*offset*/, 36/*count*/);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	
	// bind with 0, so, switch back to normal pointer operation
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	if (texturingEnabled)
		glEnable(GL_TEXTURE_2D);
	glPopMatrix();
}


void GOCube::update(float dt) {
	GameObject::update(dt);
}


BBox GOCube::getBBox() {
	if (isEnabledPhysics()) {
		return getPhysBBox();
	}
	return BBox(CVector(getAbsolutePosition().x - (size.x / 2.0f), getAbsolutePosition().y + (size.y / 2.0f)), CVector(getAbsolutePosition().x + (size.x / 2.0f), getAbsolutePosition().y - (size.y / 2.0f)));
}


void GOCube::makeCollisionShape() {
	switch (getCollisionShapeType()) {
	case CST_AUTOMATIC:
	case CST_AUTOMATIC_BOX:
		makeBoxShape(size);
		break;
	case CST_AUTOMATIC_SPHERE: {
		float radius = size.x;
		if (radius < size.y)
			radius = size.y;
		if (radius < size.z)
			radius = size.z;
		makeSphereShape(radius);
		}
		break;
	case CST_AUTOMATIC_CYLINDER: {
		float diameter = sqrt(size.x * size.x + size.z * size.z);
		makeCylinderShape(CVector(diameter, size.y));
		}
		break;
	default:
		break;
	}
	PhysicObject::makeCollisionShape();
}


void GOCube::setSize(CVector size) {
	this->size = size;
	needRefreshBuffers = true;
	setNeedRefreshCollisionShape();
}


int GOCube::methodsBridge(lua_State* luaVM) {
	if (isCurrentMethod("setSize")) {
		setSize(CVector(lua_tonumber(luaVM, 1), lua_tonumber(luaVM, 2), lua_tonumber(luaVM, 3)));
		return 0;
	}
	if (isCurrentMethod("getSize")) {
		luaPushVector(luaVM, getSize().x, getSize().y, getSize().z);
		return 1;
	}
	return GameObject::methodsBridge(luaVM);
}


GOCube::~GOCube()
{
}
