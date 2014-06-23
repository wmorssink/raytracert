#include "box.h"
#include "math.h"
#include <array>
#include <limits>
#include <float.h>
#include <typeinfo>
#include <trianglelist.h>

unsigned int GUESS = 3;
float GUESS_PERCENT = 0.1f;
unsigned int SPLIT_AT = 24;

Vec3Df loc;
float a[3];
node left;
node right;

void Box(Vec3Df _loc, float _a[], std::vector<element> triangles, char as) {

	loc = _loc;
	a = _a;

	float cost = median(triangles, as);
	float split = (cost + a[as]) / 2;

	std::vector<element> l[GUESS];
	std::vector<element> r[GUESS];

	for (int i = 0; i < GUESS; i++) {
		std::vector<element> t[] = splitList(triangles,
				split - (GUESS / 2 + i) * GUESS_PERCENT * a[as], as);
		l[i] = t[0];
		r[i] = t[1];
	}

	int eff = 0;
	int count = std::numeric_limits<int>::max();

	for (unsigned int i = 0; i < GUESS; i++) {
		int c = r[i].size() + l[i].size();
		if (c < count) {
			count = c;
			eff = i;
		}
	}

	float lw = a[as] - split;
	if (l[eff].size() > SPLIT_AT) {
		float la[] = a;
		la[as] = lw;
		left = Box(loc, la, l[eff], ((as + 1) % 3));
	} else {
		std::vector<element> left = new trianglelist(l[eff]);
	}
	if (r[eff].size() > SPLIT_AT) {
		float la[] = a;
		la[as] = split;
		Vec3Df n = new Vec3Df(loc);
		n.p[as] += lw;
		right = new Box(n, la, r[eff], ((as + 1) % 3));
	} else {
		right = new trianglelist(r[eff]);
	}
	if (right.hasEmpty()) {
		right = right.getBranch();
	}
	if (left.hasEmpty()) {
		left = left.getBranch();
	}

}

std::array<std::vector<element>, 2> splitList(std::vector<element> triangles, float split, char as) {
	std::vector<element> left;
	std::vector<element> right;

	for (unsigned int i = 0; i < triangles.size(); i++) {
		element current = triangles.at(i);
		if (current.isInRight(split, as))
			right.push_back(current);
		if (current.isInLeft(split, as))
			left.push_back(current);
	}

	std::array<std::vector<element>, 2> res;
	res[0] = left;
	res[1] = right;
	return res;
}

struct less_than_key {
	bool operator()(const element& struct1, const element& struct2, char as) {
		return (struct1.p[as] < struct2.p[as]);
	}
};

float median(std::vector<element> triangles, char as) {
	std::vector<element> t = triangles;

	//check if sort is correct, otherwise implement ourself.
	std::sort(t.begin(), t.end(), less_than_key(as));
	return t.at(t.size() / 2).p[as];
}

bool hasEmpty() {
	if(left.isEmpty())
		return true;
	if(right.isEmpty())
		return true;
	return false;
}

node getBranch() {
	if(!left.isEmpty())
		return left;
	return right;
}

bool isEmpty() {
	return false;
}

Vec3Df getLoc() {
	return loc;
}

std::array<float, 3> geta() {
	return a;
}

bool intersect(Vec3Df R[], Vec3Df* intersectOut, int* ind){
	float b[] = left.geta();
	float leftD = rayIntersectBox(R, left.getLoc(), b[0], b[1], b[2], intersectOut);
	float c[] = right.geta();
	float rightD = rayIntersectBox(R, right.getLoc(), c[0], c[1], c[2], intersectOut);

	if( type(left) == type(trianglelist) && type(right) == type(trianglelist) ){
		if (leftD < rightD) {
			int k = left.intersectTriangle(R[0], R[1], intersectOut);
			if(k == -1)
				return false;
			intersectOut = MyMesh.triangles[k];
			ind = k;
			return true;
		}

		if (rightD < leftD) {
			int k = right.intersectTriangle(R[0], R[1], intersectOut);
			if(k == -1)
				return false;
			intersectOut = MyMesh.triangles[k];
			ind = k;
			return true;
		}
		return false;
	}

	else if(type(left) == type(trianglelist)){
		int k = left.intersectTriangle(R[0], R[1], intersectOut);
		if(k == -1)
						return false;
		intersectOut = MyMesh.triangles[k];
		ind = k;
		return true;
	}
	else if(type(right) == type(trianglelist)){
		int k = left.intersectTriangle(R[0], R[1], intersectOut);
		if(k == -1)
						return false;
		intersectOut = MyMesh.triangles[k];
		ind = k;
		return true;
	}

	else if(leftD == FLT_MAX && rightD == FLT_MAX){
		return false;
	}
	else if (leftD == FLT_MAX){
		return right.intersect(R, intersectOut, ind);
	}
	else if(rightD == FLT_MAX){
		return left.intersect(R, intersectOut, ind);
	}
	else if (leftD < rightD) {
		if (left.intersect(R, intersectOut, ind)) {
			return true;
		}
		return right.intersect(R, intersectOut, ind);
	}
	else if (rightD < leftD) {
		if (right.intersect(R, intersectOut, ind)) {
			return true;
		}
		return left.intersect(R, intersectOut, ind);
	}
	return false;
}
