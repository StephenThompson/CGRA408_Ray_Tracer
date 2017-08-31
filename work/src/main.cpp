#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <thread> 
#include "image.h"
#include "aShape.h"
#include "aLight.h"
#include "camera.h"
#include "sphere.h"
#include "plane.h"
#include "cube.h"
#include "cylinder.h"
#include "mesh.h"
#include "pointLight.h"
#include "ray.h"
#include "bvh.h"
#include "parser.h"


using namespace glm;

std::vector<aShape*> shapes;
bvh* shapesBVH = nullptr;
std::vector<aLight*> lights;
camera *mainCamera = nullptr;

float g_farDistance = 10000;
vec3 ambientColor(0);

sceneInformation m_sceneInfo;

void generateScene();
void cleanScene();
void renderSceneToImage(image *i, unsigned int start, unsigned int end);
vec3 calculateRay(ray &r, int depth);
bool isShadow(ray &r, vec3 &lightPos);
int main(int, char**);

void generateScene(){
	mainCamera = m_sceneInfo.m_camera;
	shapes = m_sceneInfo.shapes;
	lights = m_sceneInfo.lights;
	shapesBVH = new bvh(shapes, m_sceneInfo.m_sceneBBox);
}

void cleanScene(){
	for (unsigned int k = 0; k < shapes.size(); ++k) {
		delete shapes[k];
	}

	for (unsigned int k = 0; k < lights.size(); ++k){
		delete lights[k];
	}

	delete shapesBVH;
	delete mainCamera;
}

void renderSceneToImage(image *i, unsigned int start, unsigned int end){
	int width = m_sceneInfo.m_xResolution;
	int height = m_sceneInfo.m_yResolution;
	for (unsigned int j = start; j < end; j += 3) {
		int x = (j/3) % width;
		int y = height - 1 - (j/3) / width;
		float dx = -float(x * 2 - width) / width;
		float dy = (float(y * 2 - height) / height) * (height / float(width));

		ray primaryRay = mainCamera->calculateRay(dx, dy);
		vec3 pixelValue = calculateRay(primaryRay, 0);
		vec3 exposureValue = sqrt(pixelValue * 3.0f) * 255.f;

		i->data[j] = (char)max(0.0f, min(255.0f, exposureValue.x));
		i->data[j + 1] = (char)max(0.0f, min(255.0f, exposureValue.y));
		i->data[j + 2] = (char)max(0.0f, min(255.0f, exposureValue.z));
	}
}

vec3 calculateRay(ray &r, int depth){
	vec3 returnValue = ambientColor;

	if (depth >= m_sceneInfo.m_maxdepth)
		return returnValue;

	CollisionInfo colInfo;
	colInfo.m_distance = g_farDistance;
	bool foundCollision = false;

	std::vector<aShape*> shapeList = shapesBVH->queryRay(r);

	for (unsigned int k = 0; k < shapeList.size(); ++k) {
		CollisionInfo colTempInfo;

		if (shapeList[k]->intersects(r, colTempInfo) && colInfo.m_distance > colTempInfo.m_distance) {
			colInfo = colTempInfo;
			foundCollision = true;
		}
	}


	if (foundCollision) {
		vec3 light;
		for (unsigned int k = 0; k < lights.size(); ++k){
			ray shadowRay;
			shadowRay.origin = colInfo.m_point + colInfo.m_normal * epsilon<float>() * 1000.0f;
			shadowRay.direction = normalize(lights[k]->getPosition() - shadowRay.origin);
			
			vec3 lightPos = lights[k]->getPosition();
			if (isShadow(shadowRay, lightPos)) continue;

			// Diffuse
			vec3 lightColor = lights[k]->calculateLight(colInfo.m_point, colInfo.m_normal);
			light += lightColor * colInfo.m_material.diffuseColor;

			// Specular
			vec3 reflected = reflect(shadowRay.direction, colInfo.m_normal);
			float specular = max(0.0f, dot(reflected, r.direction));
			light += pow(specular, colInfo.m_material.specularHardness) * colInfo.m_material.specularColor * lights[k]->getLightColor();
		}

		// reflection
		vec3 reflectionColor;
		if (colInfo.m_material.reflectionColor.x > 0 || colInfo.m_material.reflectionColor.y > 0 || colInfo.m_material.reflectionColor.z > 0){
			ray reflectedRay;
			reflectedRay.origin = colInfo.m_point + colInfo.m_normal * epsilon<float>() * 10000.0f;
			reflectedRay.direction = normalize(reflect(r.direction, colInfo.m_normal));
			if (dot(reflectedRay.direction, r.direction) < 1 - epsilon<float>() * 10000.0f)
				reflectionColor = calculateRay(reflectedRay, depth + 1) * colInfo.m_material.reflectionColor;
		}

		// refraction
		vec3 refractionColor;
		if (colInfo.m_material.refractionColor.x > 0 || colInfo.m_material.refractionColor.y > 0 || colInfo.m_material.refractionColor.z > 0) {
			ray refractedRay;
			if (dot(r.direction, colInfo.m_normal) > 0) {
				refractedRay.origin = colInfo.m_point + colInfo.m_normal * epsilon<float>() * 10000.0f;
				refractedRay.direction = normalize(refract(r.direction, -colInfo.m_normal, colInfo.m_material.ior));
			} else {
				refractedRay.origin = colInfo.m_point - colInfo.m_normal * epsilon<float>() * 10000.0f;
				refractedRay.direction = normalize(refract(r.direction, colInfo.m_normal, 1/colInfo.m_material.ior));
			}
			refractionColor = calculateRay(refractedRay, depth + 1) * colInfo.m_material.refractionColor;
		}
		returnValue = colInfo.m_material.emissionColor + ambientColor * colInfo.m_material.diffuseColor + light + reflectionColor + refractionColor;
	}
	return returnValue;
}

bool isShadow(ray &r, vec3 &lightPos){
	float distToLight = length(lightPos - r.origin);
	std::vector<aShape*> shapeList = shapesBVH->queryRay(r);

	for (unsigned int k = 0; k < shapeList.size(); ++k) {
		CollisionInfo c;
		c.m_distance = g_farDistance;
		if (shapeList[k]->intersects(r, c) && c.m_distance < distToLight) {
			return true;
		}
	}
	return false;
}

int main(int argc, char* argv[])
{
	if (argc != 2){
	  std::cout << "Scene file expected";
	  return 0;
	}
	
	parser p(argv[1]);
	m_sceneInfo = p.getSceneInfo();
	generateScene();

	std::cout << "\nStarting to render scene\n";
	image i(m_sceneInfo.m_xResolution, m_sceneInfo.m_yResolution, 3);
	const int num_threads = 32;

	std::thread t[num_threads];
	int size = ceil((i.data.size() / 3.0) / num_threads) * 3.0;
	for (int j = 0; j < num_threads; ++j) {
		t[j] = std::thread(renderSceneToImage, &i, j * size, min((j + 1) * size, int(i.data.size())));
	}

	for (int j = 0; j < num_threads; ++j) {
		t[j].join();
	}

	//renderSceneToImage(&i, 0, i.data.size());

	std::string output = m_sceneInfo.m_outputFilename;
	i.write(output.c_str());
	std::cout << "Completed rendering scene\n";
	cleanScene();

	std::cout << "image saved\n";
	#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	  getchar();
	#endif
	
	
	return 0;
}