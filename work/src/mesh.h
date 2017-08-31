//---------------------------------------------------------------------------
//
// Copyright (c) 2015 Taehyun Rhee, Joshua Scott, Ben Allen
//
// This software is provided 'as-is' for assignment of COMP308 in ECS,
// Victoria University of Wellington, without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
//----------------------------------------------------------------------------

#pragma once

#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "aShape.h"
#include "triangle.h"
#include "bvh.h"

struct vertex {
	int p = 0; // index for point in m_points
	int t = 0; // index for uv in m_uvs
	int n = 0; // index for normal in m_normals
};

struct triangleDefinition {
	vertex v[3]; //requires 3 verticies
};

class mesh : public aShape  {
private:
	std::string m_filename;
	std::vector<glm::vec3> m_points;	// Point list
	std::vector<glm::vec2> m_uvs;		// Texture Coordinate list
	std::vector<glm::vec3> m_normals;	// Normal list
	std::vector<triangleDefinition> m_trianglesDefinition;
	std::vector<triangle> m_triangles;

	bvh* m_trianglesAcceleration;

	void readOBJ(std::string);
	void createNormals();
public:
	mesh(std::string, glm::vec3 center, material m);
	mesh(std::vector<glm::vec3> vertices, std::vector<int> indices, glm::vec3 center, material m);
	~mesh();
	bool intersects(ray r, CollisionInfo& collisionInfo);
};
