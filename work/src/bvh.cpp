#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "bvh.h"
#include "bbox.h"
#include "ray.h"
#include <iostream>

using namespace glm;

bvh::bvh(int maxCapacity, bbox initialSize) {
	MAX_CAPACITY = maxCapacity;
	m_root = new node();
	m_root->m_type = LEAF;
	m_root->boundingBox.m_maxXYZ = vec3(-100000);
	m_root->boundingBox.m_minXYZ = vec3(100000);
	m_root->unadjustedBoundingBox = initialSize;
}

bvh::~bvh() {
}

bvh::bvh(std::vector<aShape*> s, bbox initialSize) {
	m_root = new node();
	m_root->boundingBox = initialSize;
	create(s, m_root, 0);
}

bool sortX(aShape* i, aShape* j) { return (i->getBoundingBox().center.x < j->getBoundingBox().center.x); }
bool sortY(aShape* i, aShape* j) { return (i->getBoundingBox().center.y < j->getBoundingBox().center.y); }
bool sortZ(aShape* i, aShape* j) { return (i->getBoundingBox().center.z < j->getBoundingBox().center.z); }

void bvh::create(std::vector<aShape*> s, node* n, int depth) {
	if (s.size() == 1) {
		n->m_containedShape = s[0];
		n->boundingBox = s[0]->getBoundingBox();
		n->m_type = LEAF;
		maxDepth = max(maxDepth, depth);
	}
	else if (s.size() > 1) {
		n->m_type = INTERNAL;

		// Sort List
		vec3 dimension = n->boundingBox.m_maxXYZ - n->boundingBox.m_minXYZ;
		if (dimension.x > dimension.y && dimension.x > dimension.z) {
			std::sort(s.begin(), s.end(), sortX);
		}
		else if (dimension.y > dimension.x && dimension.y > dimension.z) {
			std::sort(s.begin(), s.end(), sortY);
		}
		else if (dimension.z > dimension.y && dimension.z > dimension.x) {
			std::sort(s.begin(), s.end(), sortZ);
		}

		// Calculate first half / left branch
		bbox leftBBox;
		leftBBox.m_maxXYZ = vec3(-10000);
		leftBBox.m_minXYZ = vec3(10000);
		std::vector<aShape*> leftShapes;
		for (unsigned int i = 0; i < s.size()/2; ++i) {
			leftShapes.push_back(s[i]);
			leftBBox.m_minXYZ = min(leftBBox.m_minXYZ, s[i]->getBoundingBox().m_minXYZ);
			leftBBox.m_maxXYZ = max(leftBBox.m_maxXYZ, s[i]->getBoundingBox().m_maxXYZ);
		}

		n->m_left = new node;
		n->m_left->boundingBox = leftBBox;
		create(leftShapes, n->m_left, depth + 1);

		// Calculate second half / right branch
		bbox rightBBox;
		rightBBox.m_maxXYZ = vec3(-10000);
		rightBBox.m_minXYZ = vec3(10000);
		std::vector<aShape*> rightShapes;
		for (unsigned int i = s.size() / 2; i < s.size(); ++i) {
			rightShapes.push_back(s[i]);
			rightBBox.m_minXYZ = min(rightBBox.m_minXYZ, s[i]->getBoundingBox().m_minXYZ);
			rightBBox.m_maxXYZ = max(rightBBox.m_maxXYZ, s[i]->getBoundingBox().m_maxXYZ);
		}

		n->m_right = new node;
		n->m_right->boundingBox = rightBBox;
		create(rightShapes, n->m_right, depth + 1);

	}
}

void bvh::insert(aShape* s) {
	insert(s, m_root, 0);
}

void bvh::insert(aShape* s, node* n, int depth) {
	if (n->m_type == LEAF) {
		if (n->m_containedShape == nullptr) {
			n->m_containedShape = s;
		} else {
			// Split b into 2 halfs along long side
			vec3 dimension = n->unadjustedBoundingBox.m_maxXYZ - n->unadjustedBoundingBox.m_minXYZ;
			n->m_left = new node;
			n->m_right = new node;

			n->m_left->m_containedShape = n->m_containedShape;
			n->m_right->m_containedShape = s;

			n->m_left->boundingBox = n->m_containedShape->getBoundingBox();
			n->m_right->boundingBox = s->getBoundingBox();

			n->m_containedShape = nullptr;

			/*vec3 centerOffset;
			vec3 sizeOffset;

			if (dimension.x > dimension.y && dimension.x > dimension.z) {
				centerOffset = vec3(dimension.x / 4, 0, 0);
				sizeOffset = vec3(dimension.x / 2, 0, 0);
			} else if (dimension.y > dimension.x && dimension.y > dimension.z) {
				centerOffset = vec3(0, dimension.y / 4, 0);
				sizeOffset = vec3(0, dimension.y / 2, 0);
			}
			else if (dimension.z > dimension.y && dimension.z > dimension.x) {
				centerOffset = vec3(0, 0, dimension.z / 4);
				sizeOffset = vec3(0, 0, dimension.z / 2);
			}

			n->m_left->unadjustedBoundingBox.center = n->unadjustedBoundingBox.center - centerOffset;
			n->m_left->unadjustedBoundingBox.m_minXYZ = n->unadjustedBoundingBox.m_minXYZ;
			n->m_left->unadjustedBoundingBox.m_minXYZ = n->unadjustedBoundingBox.m_maxXYZ - sizeOffset;

			n->m_right->unadjustedBoundingBox.center = n->unadjustedBoundingBox.center + centerOffset;
			n->m_right->unadjustedBoundingBox.m_minXYZ = n->unadjustedBoundingBox.m_minXYZ + sizeOffset;
			n->m_right->unadjustedBoundingBox.m_minXYZ = n->unadjustedBoundingBox.m_maxXYZ;*/

			// insert shapes into leaves
			maxDepth = max(maxDepth, depth);
			n->m_type = INTERNAL;
			/*for (unsigned int i = 0; i < n->shapes.size(); ++i) {
				insert(n->shapes[i], n, depth + 1);
			}
			n->shapes.clear();*/
		}
	} else {
		float disLeft = length(n->m_left->boundingBox.center - s->getBoundingBox().center);
		float disRight = length(n->m_right->boundingBox.center - s->getBoundingBox().center);
		if (disLeft < disRight) {
			insert(s, n->m_left, depth + 1);
		} else {
			insert(s, n->m_right, depth + 1);
		}
	}

	n->boundingBox.m_minXYZ = min(n->boundingBox.m_minXYZ, s->getBoundingBox().m_minXYZ);
	n->boundingBox.m_maxXYZ = max(n->boundingBox.m_maxXYZ, s->getBoundingBox().m_maxXYZ);
	n->boundingBox.center = n->boundingBox.m_maxXYZ - n->boundingBox.m_minXYZ;
}

std::vector<aShape*> bvh::queryRay(ray &r) {
	std::vector<aShape*> shapes;
	queryRay(r, m_root, shapes);
	return shapes;
}
void bvh::reportMaxDepth() {
	std::cout << "Max Depth : " << maxDepth << "\n";
}

void bvh::queryRay(ray &r, node* n, std::vector<aShape*> &shapes) {
	if (!n->boundingBox.intersects(r)) return;
	if (n->m_type == LEAF) {
		if (n->m_containedShape != nullptr) {
			shapes.push_back(n->m_containedShape);
		}
	}
	else {
		if (n->m_left != nullptr)
			queryRay(r, n->m_left, shapes);
		if (n->m_right != nullptr)
			queryRay(r, n->m_right, shapes);
	}
}