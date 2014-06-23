#include "element.h"


class element {
	public:
		Triangle t;
		float p[3];
		int index;

	element(Triangle _t, int _index) {
		t = _t;
		for (unsigned int n = 0; n < 3; n++) {
			for (unsigned int i = 0; i < 3; i++) {
				p[n] += t.v[i][n];
			}
		}
		p[0] /= 3;
		p[1] /= 3;
		p[2] /= 3;
		index = _index;
	}

	bool isInRight(float split, int as){
		for (unsigned int i = 0; i < 3; i++) {
			if(t.v[i][as] >= split){
				return true;
			}
		}
		return false;
	}

	bool isInLeft(float split, int as) {
		for (unsigned int i = 0; i < 3; i++) {
			if (t.v[i][as] <= split) {
				return true;
			}
		}
		return false;
	}
};

