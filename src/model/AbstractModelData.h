/*
 * AbstractModelData.h
 *
 *  Created on: 11.01.2011
 *      Author: Администратор
 */

#ifndef ABSTRACTMODELDATA_H_
#define ABSTRACTMODELDATA_H_

class AbstractModelData {
public:
	virtual bool isStatic() = 0; // return true if this model have not animations
	virtual AbstractModelData* getCopy() = 0; // return new Instance of AbstractModel which is copy of current object
	virtual float* getTriangles() = 0; // return raw triangles array (for phys purposes for example) if model animated then return triangles of first frame
	virtual int getTrianglesCount() = 0; // return count of triangles returned by getTriangles
};

#endif /* ABSTRACTMODELDATA_H_ */
