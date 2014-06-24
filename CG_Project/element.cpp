#include "element.h"

Triangle t;
float p[3];
int elementindex;

void element(Triangle _t, int _index) {
	t = _t;
	for (unsigned int n = 0; n < 3; n++) {
		for (unsigned int i = 0; i < 3; i++) {
			p[n] += t.v[i];
					//t.v[i][n];
		}
	}
	p[0] /= 3;
	p[1] /= 3;
	p[2] /= 3;
	elementindex = _index;
}

bool isInRight(float split, int as) {
	for (unsigned int i = 0; i < 3; i++) {
	if ( MyMesh.vertices[t.v[i]].p[as] >= split ) {
			return true;
		}
	}
	return false;
}

bool isInLeft(float split, int as) {
	for (unsigned int i = 0; i < 3; i++) {
		if (MyMesh.vertices[t.v[i]].p[as] <= split) {
			return true;
		}
	}
	return false;
}

