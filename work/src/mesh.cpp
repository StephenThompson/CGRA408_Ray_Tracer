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

#include <cmath>
#include <iostream> // input/output streams
#include <fstream>  // file streams
#include <sstream>  // string streams
#include <string>
#include <stdexcept>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "mesh.h"
#include "triangle.h"
#include "bvh.h"
#include "ray.h"


using namespace std;
using namespace glm;

mesh::mesh(string filename, vec3 center, material m) {
	m_filename = filename;
	m_material = m;
	m_center = center;
	m_bbox.center = center;
	m_trianglesAcceleration = new bvh(3, m_bbox);
	readOBJ(filename);
}

mesh::mesh(std::vector<glm::vec3> vertices, std::vector<int> indices, glm::vec3 center, material m) {
	m_material = m;
	m_center = center;
	m_bbox.center = center;
	m_bbox.m_minXYZ = vec3(100000);
	m_bbox.m_maxXYZ = vec3(-100000);

	m_points = vertices;
	m_uvs.clear();
	m_normals.clear();
	m_triangles.clear();

	for (unsigned int i = 0; i < vertices.size(); i++) {
		vertices[i] += m_center;
		m_bbox.m_minXYZ = min(vertices[i], m_bbox.m_minXYZ);
		m_bbox.m_maxXYZ = max(vertices[i], m_bbox.m_maxXYZ);
	}

	for (unsigned int i = 0; i < indices.size(); i += 3) {
		triangleDefinition tri;
		for (unsigned int j = 0; j < 3; ++j) {
			tri.v[j].p = tri.v[j].n = tri.v[j].t = indices[i + j];
		}
		m_trianglesDefinition.push_back(tri);
	}

	createNormals();

	vector<aShape*> templist;
	for (unsigned int i = 0; i < m_trianglesDefinition.size(); ++i) {
		triangleDefinition t = m_trianglesDefinition[i];
		triangle currentT(m_points[t.v[0].p], m_points[t.v[1].p], m_points[t.v[2].p],
			m_normals[t.v[0].n], m_normals[t.v[1].n], m_normals[t.v[2].n], m_material);

		m_triangles.push_back(currentT);

		triangle* currentT2 = new triangle(m_points[t.v[0].p], m_points[t.v[1].p], m_points[t.v[2].p],
			m_normals[t.v[0].n], m_normals[t.v[1].n], m_normals[t.v[2].n], m_material);

		templist.push_back(currentT2);
	}

	m_trianglesAcceleration = new bvh(templist, m_bbox);
}

mesh::~mesh() {
	delete m_trianglesAcceleration;
}

void mesh::readOBJ(string filename) {

	// Make sure our geometry information is cleared
		m_points.clear();
		m_uvs.clear();
		m_normals.clear();
		m_triangles.clear();

		// Load dummy points because OBJ indexing starts at 1 not 0
		m_points.push_back(vec3(0,0,0));
		m_uvs.push_back(vec2(0,0));
		m_normals.push_back(vec3(0,1,0));

		ifstream objFile(filename);

		if(!objFile.is_open()) {
			cerr << "Error reading " << filename << endl;
			throw runtime_error("Error :: could not open file.");
		}

		cout << "Loading model " << filename << endl;

		m_bbox.m_minXYZ = vec3(100000);
		m_bbox.m_maxXYZ = vec3(-100000);

		// good() means that failbit, badbit and eofbit are all not set
		while(objFile.good()) {

			// Pull out line from file
			string line;
			std::getline(objFile, line);
			istringstream objLine(line);

			// Pull out mode from line
			string mode;
			objLine >> mode;

			// Reading like this means whitespace at the start of the line is fine
			// attempting to read from an empty string/line will set the failbit
			if (!objLine.fail()) {

				if (mode == "v") {
					vec3 v;
					objLine >> v.x >> v.y >> v.z;
					v += m_center;
					m_bbox.m_minXYZ = min(v, m_bbox.m_minXYZ);
					m_bbox.m_maxXYZ = max(v, m_bbox.m_maxXYZ);
					m_points.push_back(v);

				} else if(mode == "vn") {
					vec3 vn;
					objLine >> vn.x >> vn.y >> vn.z;
					m_normals.push_back(vn);

				} else if(mode == "vt") {
					vec2 vt;
					objLine >> vt.x >> vt.y;
					m_uvs.push_back(vt);

				} else if(mode == "f") {

					vector<vertex> verts;
					while (objLine.good()){
						vertex v;

						// Assignment code (assumes you have all of v/vt/vn for each vertex)
						objLine >> v.p;		// Scan in position index
						if (m_uvs.size() <= 1 && m_normals.size() <= 1){
							v.n = v.p;
							verts.push_back(v);
							continue;
						}

						objLine.ignore(1);	// Ignore the '/' character

						// Deal with uvs
						if (m_uvs.size() <= 1){
							v.t = 0;
						} else {
							objLine >> v.t;		// Scan in uv (texture coord) index
						}

						objLine.ignore(1);	// Ignore the '/' character

						// Deal the normals
						if (m_normals.size() <= 1){
							v.n = v.p;
						} else {
							objLine >> v.n;		// Scan in normal index
						}
						verts.push_back(v);
					}

					// IFF we have 3 verticies, construct a triangle
					if(verts.size() == 3){
						triangleDefinition tri;
						tri.v[0] = verts[0];
						tri.v[1] = verts[1];
						tri.v[2] = verts[2];
						m_trianglesDefinition.push_back(tri);

					}
				}
			}
		}

		/*cout << "Reading OBJ file is DONE." << endl;
		cout << m_points.size()-1 << " points" << endl;
		cout << m_uvs.size()-1 << " uv coords" << endl;
		cout << m_normals.size()-1 << " normals" << endl;
		cout << m_trianglesDefinition.size() << " faces" << endl;*/


		// If we didn't have any normals, create them
		if (m_normals.size() <= 1)
			createNormals();

		vector<aShape*> templist;
		for (unsigned int i = 0; i < m_trianglesDefinition.size(); ++i) {
			triangleDefinition t = m_trianglesDefinition[i];
			triangle currentT(m_points[t.v[0].p], m_points[t.v[1].p], m_points[t.v[2].p], 
				m_normals[t.v[0].n], m_normals[t.v[1].n], m_normals[t.v[2].n], m_material);
	
			m_triangles.push_back(currentT);

			triangle* currentT2 = new triangle(m_points[t.v[0].p], m_points[t.v[1].p], m_points[t.v[2].p],
				m_normals[t.v[0].n], m_normals[t.v[1].n], m_normals[t.v[2].n], m_material);

			templist.push_back(currentT2);
			//m_trianglesAcceleration->insert(currentT2);
		}


		m_trianglesAcceleration = new bvh(templist, m_bbox);
		//std::cout << "Tree created\n";
		//m_trianglesAcceleration->reportMaxDepth();
}

// Generates normals for the model
//
void mesh::createNormals() {
	m_normals.clear();

	for (int k = 0; k < m_points.size(); ++k){
		m_normals.push_back(vec3(0.f, 0.f, 0.f));
	}

	for (int k = 0; k < m_trianglesDefinition.size(); ++k){
		triangleDefinition t = m_trianglesDefinition[k];

		// Get Normal
		vec3 v1 = m_points[t.v[0].n] - m_points[t.v[1].n];
		vec3 v2 = m_points[t.v[0].n] - m_points[t.v[2].n];
		vec3 normal = cross(v1, v2);

		for (int i = 0; i < 3; ++i){
			m_normals[t.v[i].n] += normal;
		}
	}

	for (int k = 1; k < m_normals.size(); ++k){
		m_normals[k] = normalize(m_normals[k]);
	}
}

bool mesh::intersects(ray r, CollisionInfo& collisionInfo){
	CollisionInfo returninfo;
	returninfo.m_distance = 1000;
	bool foundTriangle = false;

	vector<aShape*> possible_collision = m_trianglesAcceleration->queryRay(r);

	for (int k = 0; k < possible_collision.size(); ++k){
		CollisionInfo collisionTempInfo;
		
		if (possible_collision[k]->intersects(r, collisionTempInfo) && collisionTempInfo.m_distance < returninfo.m_distance){
			returninfo = collisionTempInfo;
			foundTriangle = true;
		}
	}

	if (!foundTriangle) return false;

	collisionInfo.m_distance = returninfo.m_distance;
	collisionInfo.m_normal = returninfo.m_normal;
	collisionInfo.m_point = returninfo.m_point;
	collisionInfo.m_material = m_material;
	return true;
}
