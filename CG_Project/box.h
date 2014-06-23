/*
 * box.h
 *
 *  Created on: 23 Jun 2014
 *      Author: daan
 */

#ifndef BOX_H_
#define BOX_H_
#include "node.h"
#include "element.h"
#include <vector>
#include <array>

class box : public node {
public:
	void Box(Vec3Df _loc, float _a[], std::vector<element> triangles, char as);
	std::array<std::vector<element>, 2> splitList(std::vector<element> triangles, float split, char as);
	float median(std::vector<element> triangles, char as);
	bool hasEmpty() override;
	node getBranch() override;
	bool isEmpty() override;
	Vec3Df getLoc() override;
	float geta() override;
	bool intersect(Vec3Df R[], Vec3Df* intersectOut, int* ind) override;
};

#endif /* BOX_H_ */
