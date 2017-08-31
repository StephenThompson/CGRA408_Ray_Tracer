#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "pointLight.h"
#include "ray.h"

using namespace glm;

pointLight::pointLight(vec3 color, float power, vec3 center) {
	m_phi = color * power;
	m_center = center;
	m_color = color;
}

vec3 pointLight::getPosition() {
	return m_center;
}

vec3 pointLight::calculateLight(vec3 objectPosition, vec3 objectNormal) {
	float r = dot(m_center - objectPosition, m_center - objectPosition);
	float theta = max(0.f, dot(normalize(m_center - objectPosition), objectNormal));
	return  (theta * m_phi) / (4.f * pi<float>() * r * r);
}

vec3 pointLight::getLightColor() {
	return m_phi;// 4.f * pi<float>() * m_phi;
}
