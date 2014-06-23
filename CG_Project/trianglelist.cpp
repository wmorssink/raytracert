#include "trianglelist.h"
#include <cstddef>
#include <float.h>
#include "node.h"
#include "Vec3D.h"
#include "mesh.h"



Triangle t[];
int index[];

void trianglelist(std::vector<element> _t) {
	for(unsigned i = 0; i < _t.size(); i++) {
		t[i] = _t.at(i).t;
		index[i] = _t.at(i).index;
	}
};

	bool hasEmpty(){
		return false;
	}
	node getBranch(){
		return NULL;
	}
	bool isEmpty(){
		return sizeof(t) == 0;
	}

	int intersectTriangle(Vec3Df origin, Vec3Df dest, Vec3Df* intersectOut){
		Vec3Df intersect; //intersection point of closest triangle
		int indexa = -1;	  //index of closest triangle
		float dist = FLT_MAX;

		Vec3Df R[] = { origin, dest };
		for (unsigned int i = 0; i < sizeof(t); i++){
			Vec3Df tempIntersect;
			Triangle triangle = t[i];

			Vec3Df T[3] = { MyMesh.vertices[triangle.v[0]].p, MyMesh.vertices[triangle.v[1]].p, MyMesh.vertices[triangle.v[2]].p };


			if (rayIntersectTriangle(R, T, &tempIntersect)){
				//ray intersects with the current triangle
				float tempDist = Vec3Df::distance(origin, tempIntersect);
				if (tempDist < dist){
					dist = tempDist;
					indexa = index[0];
					intersect = tempIntersect;
				}
			}
		}
		memcpy(intersectOut, &intersect, sizeof(Vec3Df));
		return indexa;
	}



