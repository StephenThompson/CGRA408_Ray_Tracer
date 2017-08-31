#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "aShape.h"
#include "bbox.h"
#include <vector>
#include <limits>

class bvh {
private:
	enum nodeType {INTERNAL, LEAF};
	struct node{
		bbox boundingBox;
		bbox unadjustedBoundingBox;
		node* m_left;
		node* m_right;
		nodeType m_type = LEAF;
		aShape* m_containedShape;

		~node() {
			if (m_left != nullptr)
				delete m_left;
			if (m_right != nullptr)
				delete m_right;
			if (m_containedShape!= nullptr)
				delete m_containedShape;
		}
	};

	int MAX_CAPACITY;
	int maxDepth = 0;
	node* m_root;
	bbox m_boundingBox;
	void insert(aShape* s, node* n, int depth);
	void create(std::vector<aShape*> s, node* n, int depth);
	void queryRay(ray &r, node* n, std::vector<aShape*> &shapes);
public:
	bvh(int maxCapacity, bbox initialSize);
	~bvh();
	bvh(std::vector<aShape*> s, bbox initialSize);
	void insert(aShape* s);
	void reportMaxDepth();
	std::vector<aShape*> queryRay(ray &r);
};
