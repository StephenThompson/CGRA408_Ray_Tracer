#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "cylinder.h"
#include "ray.h"

using namespace glm;

cylinder::cylinder(vec3 center, float radius, float height, material m) {
	m_center = center;
	m_radius = radius;
	m_height = height/2;
	m_bbox.m_minXYZ = center - (radius, m_height, radius);
	m_bbox.m_maxXYZ = center + (radius, m_height, radius);
	m_bbox.center = (m_bbox.m_minXYZ + m_bbox.m_maxXYZ) / 2.0f;
	m_material = m;
}

//sphere::~sphere(){
//}

bool cylinder::intersects(ray r, CollisionInfo& collisionInfo) {
	float A = r.direction.x * r.direction.x + r.direction.z * r.direction.z;
	float B = 2 * (r.direction.x * r.origin.x + r.direction.z * r.origin.z);
	float C = r.origin.x * r.origin.x + r.origin.z * r.origin.z - m_radius * m_radius;

	float quad = B * B - 4.0f * A * C;

	if (quad < epsilon<float>()) return false;

	quad = sqrt(quad);
	float dNear = (-B - quad)/(2 * A);
	float dFar = (-B + quad)/(2 * A);

	if (dNear > dFar) {
		float dTemp = dNear;
		dNear = dFar;
		dFar = dTemp;
	}

	if (dFar < epsilon<float>())
		return false;

	float nearHeight = (r.origin + dNear * r.direction).y;
	float farHeight = (r.origin + dFar * r.direction).y;

	if (dNear > 0 && nearHeight >= m_center.y - m_height && nearHeight <= m_center.y + m_height){
		collisionInfo.m_distance = dNear;
	} else if (farHeight >= m_center.y - m_height && farHeight <= m_center.y + m_height) {
		collisionInfo.m_distance = dFar;
	} else {
		return false;
	}
	// Broken
	//if (dFar < 0) return false;
	collisionInfo.m_distance = dNear;

	collisionInfo.m_point = r.origin + collisionInfo.m_distance * r.direction;
	collisionInfo.m_normal = normalize(vec3(collisionInfo.m_point.x - m_center.x, 0, collisionInfo.m_point.z - m_center.z));

	collisionInfo.m_material = m_material;
	return true;
}
