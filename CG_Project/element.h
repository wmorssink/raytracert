#ifndef ELEMENT_H_
#define ELEMENT_H_

#include "mesh.h"
#include "raytracing.h"

class element {
public:
	Triangle t;
	float p[3];
	int elementindex;
	element(Triangle _t, int _index);
	bool isInRight(float split, int as);
	bool isInLeft(float split, int as);
};

#endif /* ELEMENT_H_ */
