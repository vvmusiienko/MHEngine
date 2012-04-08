#include "GOPlate.h"
#include "../TextureCache.h"
#include <stdexcept>

GOPlate::GOPlate() : GameObject() {
	setEnabled(false); // Not enabled by default (for compitability reasons)
	init();
}


GOPlate::GOPlate(string id) : GameObject(id) {
	setEnabled(false); // Not enabled by default (for compitability reasons)
	init();
	bindLua();
}


GOPlate::GOPlate(FILE* f) : GameObject(f) {
	try {
		init();
		size = readVector(f);
		textureName = readString(f);
		texture = TextureCache::getInstance()->load(textureName);
		density = readFloat(f);
		bindLua();
	} catch (std::runtime_error e) {
		throw std::runtime_error(FormatString("GOPlate::GOPlate(FILE* f): Can't create object from stream! Reason: %s", e.what()));
	}
}


void GOPlate::save(FILE* f) {
	GameObject::save(f);
	writeVector(size, f);
	writeString(textureName, f);
	writeFloat(density, f);
}


void GOPlate::init() {
	type = GO_PLATE;
	flipHorizontal = false;
	flipVertical = false;
	oldVerticesCount = -1;
	textureZoomS = 1;
	textureZoomT = 1;
	density = 0;
	texture = NULL;
	useArea = false;
	tw = 1.0f;
	th = 1.0f;
	tx = 0.0f;
	ty = 0.0f;
	autoSize = false;
	enablePhysics(false);
	glGenBuffers(1, &VBOBuffer);
}


void GOPlate::setArea(CVector origin, CVector size) {
	useArea = true;
	areaOrigin = origin;
	areaSize = size;
	needRefreshBuffers = true;
}


void GOPlate::refreshBuffers() {
	GameObject::refreshBuffers();

	float DX;
	float DY;

	if (autoSize && texture) {
		size = CVector(texture->getWidth(), texture->getHeight());
	}

	if (useArea && texture) {
		tw = areaSize.x / texture->getWidth();
		th = areaSize.y / texture->getHeight();
		tx = areaOrigin.x / texture->getWidth();
		ty = areaOrigin.y / texture->getHeight();
	}
	else
	if (texture) {
		tx = texture->getU();
		tw = flipHorizontal ? -texture->getUL() : texture->getUL();
		ty = texture->getV();
		th = flipVertical ? -texture->getVL() : texture->getVL();
	}

	if (density == 0) {
		DX = 1.0/(size.x+1); // density (cubs per one)
		DY = 1.0/(size.y+1);
	} else {
		DX = 1.0/density; // density (cubs per one)
		DY = 1.0/density;
	}

	int px = (int)ceil(size.x / (1.0 / DX)); // pieces for x
	int py = (int)ceil(size.y / (1.0 / DY)); // pieces for y
	
	float psx = size.x / (float)px; // pieces step for x
	float psy = size.y / (float)py; // pieces step for y

	float tpss = tw / textureZoomS / (float)px; // texture pieces step for s
	float tpst = th / textureZoomT / (float)py; // texture pieces step for t
	
	// for positioning
	CVector p = getPositioningOffset(getPositioningMode(), size.x, size.y);
	float offsetX =  p.x;
	float offsetY =  p.y;

	verticesCount = (4 + (px - 1) * 2) * py + (py - 1) * 2;
				//  | one row count  | * py + |degenerate vertices|


	if (verticesCount != oldVerticesCount) {
		glBindBuffer(GL_ARRAY_BUFFER, VBOBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(MHVertex) * verticesCount, 0, GL_STATIC_DRAW);
	} else {
		glBindBuffer(GL_ARRAY_BUFFER, VBOBuffer);
	}
	vertexBuffer = (MHVertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (!vertexBuffer) {
		Log::warning("Can't map buffer. GOPlate(\"%s\")", id.c_str());
		return;
	}

	oldVerticesCount = verticesCount;

	// calculate normal
	/*CVector v1(0.0f, 0.0f);
	CVector v2(1.0f, 0.0f);
	CVector v3(0.0f, 1.0);
	CVector n = (v1 - v2).CrossProduct(v2-v3);
	n.Normalize();
*/
	CVector n = CVector(0,0,1.0f);

		int currentVerticleIndex = 0;
		for (int y = 0; y < py; y++) {
			for (int x = 0; x < px; x++) {
				
				// B   D
				// | / |
				// A   C
				if (py > 1 && x == 0 && y > 0) { // second degenerate vertex (for each begin except first)
					//B
					vertexBuffer[currentVerticleIndex].x = x * psx + offsetX;
					vertexBuffer[currentVerticleIndex].y = psy + (y * psy) + offsetY;
					vertexBuffer[currentVerticleIndex].z = 0;
					vertexBuffer[currentVerticleIndex].nx = n.x;
					vertexBuffer[currentVerticleIndex].ny = n.y;
					vertexBuffer[currentVerticleIndex].nz = n.z;
					vertexBuffer[currentVerticleIndex].s = tpss * x + tx;
					vertexBuffer[currentVerticleIndex].t = tpst + tpst * y + ty;
					currentVerticleIndex++;
				}

				if (x == 0) {
				//B
				vertexBuffer[currentVerticleIndex].x = x * psx + offsetX;
				vertexBuffer[currentVerticleIndex].y = psy + (y * psy) + offsetY;
				vertexBuffer[currentVerticleIndex].z = 0;
				vertexBuffer[currentVerticleIndex].nx = n.x;
				vertexBuffer[currentVerticleIndex].ny = n.y;
				vertexBuffer[currentVerticleIndex].nz = n.z;
				vertexBuffer[currentVerticleIndex].s = tpss * x + tx;
				vertexBuffer[currentVerticleIndex].t = tpst + tpst * y + ty;
				currentVerticleIndex++;

				// A
				vertexBuffer[currentVerticleIndex].x = x * psx + offsetX;
				vertexBuffer[currentVerticleIndex].y = y * psy + offsetY;
				vertexBuffer[currentVerticleIndex].z = 0;
				vertexBuffer[currentVerticleIndex].nx = n.x;
				vertexBuffer[currentVerticleIndex].ny = n.y;
				vertexBuffer[currentVerticleIndex].nz = n.z;
				vertexBuffer[currentVerticleIndex].s = tpss * x + tx;
				vertexBuffer[currentVerticleIndex].t = tpst * y + ty;
				currentVerticleIndex++;
				}
				
				//D
				vertexBuffer[currentVerticleIndex].x = psx + (x * psx) + offsetX;
				vertexBuffer[currentVerticleIndex].y = psy + (y * psy) + offsetY;
				vertexBuffer[currentVerticleIndex].z = 0;
				vertexBuffer[currentVerticleIndex].nx = n.x;
				vertexBuffer[currentVerticleIndex].ny = n.y;
				vertexBuffer[currentVerticleIndex].nz = n.z;
				vertexBuffer[currentVerticleIndex].s = tpss + tpss * x + tx;
				vertexBuffer[currentVerticleIndex].t = tpst + tpst * y + ty;
				currentVerticleIndex++;

				//C
				vertexBuffer[currentVerticleIndex].x = psx + (x * psx) + offsetX;
				vertexBuffer[currentVerticleIndex].y = y * psy + offsetY;
				vertexBuffer[currentVerticleIndex].z = 0;
				vertexBuffer[currentVerticleIndex].nx = n.x;
				vertexBuffer[currentVerticleIndex].ny = n.y;
				vertexBuffer[currentVerticleIndex].nz = n.z;
				vertexBuffer[currentVerticleIndex].s = tpss + tpss * x + tx;
				vertexBuffer[currentVerticleIndex].t = tpst * y + ty;
				currentVerticleIndex++;

				if (py > 1 && x == px - 1 && y != py - 1) { // last vertex must copy (degenerate)
					//C
					vertexBuffer[currentVerticleIndex].x = psx + (x * psx) + offsetX;
					vertexBuffer[currentVerticleIndex].y = y * psy + offsetY;
					vertexBuffer[currentVerticleIndex].z = 0;
					vertexBuffer[currentVerticleIndex].nx = n.x;
					vertexBuffer[currentVerticleIndex].ny = n.y;
					vertexBuffer[currentVerticleIndex].nz = n.z;
					vertexBuffer[currentVerticleIndex].s = tpss + tpss * x + tx;
					vertexBuffer[currentVerticleIndex].t = tpst * y + ty;
					currentVerticleIndex++;
				}

			}
		}


		if (!glUnmapBuffer(GL_ARRAY_BUFFER) ) {
#ifdef MH_DEBUG
			Log::warning("Unmap buffer failed. GOPlate(\"%s\")", id.c_str());
#endif
		}
}


void GOPlate::draw() {
	glPushMatrix();
	GameObject::draw();

	// save previous state of texturing
	bool texturingEnabled = glIsEnabled(GL_TEXTURE_2D);
	if (texture) {
		texture->bind();
	} else {
		glDisable(GL_TEXTURE_2D);
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBOBuffer);         // for vertex coordinates
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3/*size of one vertex*/,
					GL_FLOAT/*type of each component*/,
					sizeof(MHVertex)/*is the byte offset between vertices*/,
					BUFFER_OFFSET(0)/*no comments:P*/);
	glNormalPointer(GL_FLOAT/*type of each component*/,
					sizeof(MHVertex)/*is the byte offset between vertices*/,
					BUFFER_OFFSET(12)/*no comments:P*/);
	glTexCoordPointer(2/*size of one vertex*/,
					GL_FLOAT/*type of each component*/,
					sizeof(MHVertex)/*is the byte offset between vertices*/,
					BUFFER_OFFSET(24)/*no comments:P*/);
	glDrawArrays(GL_TRIANGLE_STRIP/*type of vertices*/, 0/*offset*/, verticesCount/*count*/);

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	// bind with 0, so, switch back to normal pointer operation
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	if (!texture) {
		if (texturingEnabled) {
			glEnable(GL_TEXTURE_2D);
		} else {
			glDisable(GL_TEXTURE_2D);
		}
	}
	glPopMatrix();
	
}


void GOPlate::update(float dt) {
	GameObject::update(dt);
}


void GOPlate::setFlipHorizontal(bool v) {
	if (v != flipHorizontal)
		needRefreshBuffers = true;
	flipHorizontal = v;
}


bool GOPlate::getFlipHorizontal() {
	return flipHorizontal;
}


void GOPlate::setFlipVertical(bool v) {
	if (v != flipVertical)
		needRefreshBuffers = true;
	flipVertical = v;
}


bool GOPlate::getFlipVertical() {
	return flipVertical;
}


void GOPlate::setAutoSize(bool as) {
	autoSize = as;
}


bool GOPlate::getAutoSize() {
    return autoSize;
}


void GOPlate::setSize(CVector size) {
	if (this->size != size) {
		needRefreshBuffers = true;
	}
	this->size = size;
}


void GOPlate::setTextureZoomS(float z) {
	textureZoomS = z;
	needRefreshBuffers = true;
}


void GOPlate::setTextureZoomT(float z) {
	textureZoomT = z;
	needRefreshBuffers = true;
}


void GOPlate::setDensity(float d) {
	density = d;
	needRefreshBuffers = true;
}


void GOPlate::setTexture(string name) {
	texture = TextureCache::getInstance()->load(name);
	if (name != textureName)
		needRefreshBuffers = true;
	textureName = name;
}


void GOPlate::setTexture(Texture* tex) {
	if (tex == NULL) return;
	if (tex != texture)
		needRefreshBuffers = true;
	texture = (Texture2D*)tex;
	textureName = tex->name();
}

Texture* GOPlate::getTexture() {
	return texture;
}


void GOPlate::bindLua() {
	game->luaVM->registerProperty(id + ".%stextureName", MG_STRING, &this->textureName);
	game->luaVM->registerProperty(id + ".%sdensity", MG_FLOAT, &this->density);
	game->luaVM->registerProperty(id + ".%ssize", MG_VECTOR, &this->size);
	registrateMethods(getFullID());
}


int GOPlate::methodsBridge(lua_State* luaVM) {
	if (isCurrentMethod("setTexture")) {
		setTexture(lua_tostring(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("setDensity")) {
		setDensity(lua_tonumber(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("setSize")) {
		setSize(CVector(lua_tonumber(luaVM, 1), lua_tonumber(luaVM, 2), lua_tonumber(luaVM, 3)));
		return 0;
	}
	return GameObject::methodsBridge(luaVM);
}


GOPlate::~GOPlate() {
	glDeleteBuffers(1, &VBOBuffer);
}
