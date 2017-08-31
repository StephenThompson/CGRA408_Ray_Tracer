#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.h"
#include "ray.h"

using namespace glm;
 
camera::camera(vec3 position, vec3 center, vec3 up, float fov) {
	setLookAt(position, center, up);
	setFOV(fov);
}

ray camera::calculateRay(float x, float y) {
	ray r;
	r.origin = m_position;
	r.direction = normalize(m_right * x + m_up * y + m_forward * m_viewingPlane);
	return r;
}

void camera::setLookAt(vec3 position, vec3 center, vec3 up) {
	m_position = position;
	mat4 lookatMat = lookAt(position, center, up);
	m_forward = vec3(vec4(0, 0, -1, 0) * lookatMat);
	m_right = vec3(vec4(1, 0, 0, 0) * lookatMat);
	m_up = vec3(vec4(0, 1, 0, 0) * lookatMat);
}

void camera::setFOV(float fov) {
	m_viewingPlane = 1 / tan(radians(fov*0.5));
}