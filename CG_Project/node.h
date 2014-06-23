/*
 * node.h
 *
 *  Created on: 23 Jun 2014
 *      Author: daan
 */

#ifndef NODE_H_
#define NODE_H_

#include "Vec3D.h"

class node {
public:
	node();
	virtual ~node();

	virtual node getBranch();
	virtual bool hasEmpty();
	virtual bool isEmpty();
	virtual Vec3Df getLoc();
	virtual float geta();
	virtual bool intersect(Vec3Df R[], Vec3Df* intersectOut, int* ind);
	virtual int intersectTriangle(Vec3Df origin, Vec3Df dest, Vec3Df* intersectOut);
};

#endif /* NODE_H_ */
