#ifndef TRIANGLELIST_H_
#define TRIANGLELIST_H_
#include "node.h"
#include "Vec3D.h"
#include "raytracing.h"
#include "element.h"

class trianglelist : public node {
public:
	trianglelist();
	virtual ~trianglelist();
	bool hasEmpty() override;
	node getBranch() override;
	bool isEmpty() override;
	int intersectTriangle(Vec3Df origin, Vec3Df dest, Vec3Df* intersectOut) override;
};

#endif /* TRIANGLELIST_H_ */
