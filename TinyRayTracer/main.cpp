#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm> //<-- max/min 사용하기 위해 추가
#include "geometry.h"

struct Material {
	Material(const float& r, const Vec4f &a, const Vec3f &color, const float &spec) 
		: refractive_index{ r }, albedo(a), diffuse_color(color), specular_exponent(spec) {}
	Material() : refractive_index{ 1 }, albedo(1, 0, 0,0), diffuse_color(), specular_exponent() {}

	float refractive_index;
	Vec4f albedo;
	Vec3f diffuse_color;
	float specular_exponent;
};

struct Light
{
	//point light
	Light(const Vec3f& p, const float& i) : position{ p }, intensity{ i }
	{

	}
	Vec3f position;
	float intensity;
};

struct Sphere
{
	Vec3f center;
	float radius;
	Material material;

	Sphere(const Vec3f& c, const float& r, const Material& mat) : center{ c }, radius{ r }, material{mat}
	{

	}

	bool rayIntersect(const Vec3f& orig, const Vec3f& dir, float& closestDist) const
	{
		Vec3f L = center - orig; //orig에서 출발하여 center를 향하는 벡터
		float tca = L * dir; // L vector를 dir vector에 projection
		float d2 = L * L - tca * tca; //L^2 = tca^2 + r^2인 경우 접하는 삼각형.
		if (d2 > radius*radius) return false; //r^2보다 거리가 멀면, ray가 원에 부딫히지 않음
		float thc = sqrtf(radius*radius - d2);
		float t0 = tca - thc;
		float t1 = tca + thc; //t0와 t1은 가까운/먼 교차점까지의 파라메터
		if (t0 < 0) t0 = t1;  //접점이 하나인 경우 or dir 반대 방향에 원이 있는 경우를 처리
		if (t0 < 0) return false;

		if (t0 < closestDist)
		{
			closestDist = t0;
			return true;
		}
		return false;
	}

};

Vec3f reflect(const Vec3f &light, const Vec3f &normal) {
	return light - normal * 2.f*(light*normal);
}

Vec3f refract(const Vec3f& I, const Vec3f& N, const float& refractive_index)
{
	float cosi = -std::max(-1.f, std::min(1.f, I*N));
	float etai = 1, etat = refractive_index;
	Vec3f n = N;
	if (cosi < 0)
	{
		cosi = -cosi;
		std::swap(etai, etat);
		n = -N;
	}
	float eta = etai / etat;
	float k = 1 - eta * eta*(1 - cosi * cosi);
	//Step 8 메모 참조
	return k < 0 ? Vec3f(0, 0, 0) : I * eta + n * (eta*cosi - sqrtf(k));
}

bool sceneIntersect(const Vec3f& orig, const Vec3f& dir, const std::vector<Sphere>& scene,
	Vec3f& hit, Vec3f& N, Material &mat)
{
	float sphereDist = std::numeric_limits<float>::max();
	Vec3f fillColor{};
	bool filled = false;
	for (const Sphere& s : scene)
	{
		if (s.rayIntersect(orig, dir, sphereDist))
		{
			hit = orig + dir * sphereDist;
			N = (hit - s.center).normalize();
			mat = s.material;
		}
	}
	return sphereDist < 1000;
}

Vec3f castRay(const Vec3f& orig, const Vec3f& dir, const std::vector<Sphere>& scene, const std::vector<Light>& lights, size_t depth = 0)
{
	Vec3f point, N;
	Material mat;

	if (depth > 4 || !sceneIntersect(orig, dir, scene, point, N, mat))
	{
		return Vec3f(0.2, 0.7, 0.8);
	}

	Vec3f reflect_dir = reflect(dir, N).normalize();
	Vec3f refract_dir = refract(dir, N, mat.refractive_index).normalize();
	Vec3f reflect_orig = reflect_dir*N < 0 ? point - N * 1e-3 : point + N * 1e-3;
	Vec3f refract_orig = refract_dir*N < 0 ? point - N * 1e-3 : point + N * 1e-3;
	
	Vec3f reflect_color = castRay(reflect_orig, reflect_dir, scene, lights, depth+1); 
	Vec3f refract_color = castRay(refract_orig, refract_dir, scene, lights, depth+1);

	float diffuse_light_intensity = 0, specular_light_intensity = 0;
	for(const Light& light : lights)
	{
		Vec3f light_dir = (light.position - point).normalize();
		float light_dist = (light.position - point).norm();

		Vec3f shadow_point, shadow_normal;
		Material tempMaterial;
		Vec3f shadow_orig = light_dir * N < 0 ? point - N * 1e-3 : point + N * 1e-3;
		Vec3f adj_light_dir = (light.position - shadow_orig).normalize();
		if (sceneIntersect(shadow_orig, adj_light_dir, scene, shadow_point, shadow_normal, tempMaterial) 
			&& light_dist > (shadow_point-shadow_orig).norm())
		{
			continue;
		}

		diffuse_light_intensity += light.intensity * std::max(0.f, light_dir*N);
		specular_light_intensity += powf(std::max(0.f, reflect(light_dir, N)*dir), mat.specular_exponent)*light.intensity;
	}

	return mat.diffuse_color * diffuse_light_intensity * mat.albedo[0]
		+ Vec3f(1., 1., 1.)*specular_light_intensity * mat.albedo[1]
		+ reflect_color * mat.albedo[2]
		+ refract_color * mat.albedo[3];
}


void render(const std::vector<Sphere>& scene, const std::vector<Light>& lights) {
	const int width = 1024;
	const int height = 768;
	const int fov = 3.14f / 2.f;
	std::vector<Vec3f> framebuffer(width*height);

	for (size_t j = 0; j < height; j++) {
		for (size_t i = 0; i < width; i++) {
			float x = (2 * (i + 0.5) / (float)width - 1) * tan(fov / 2.)*width / (float)height;
			float y = -(2 * (j + 0.5) / (float)height - 1) * tan(fov / 2.);
			Vec3f dir = Vec3f(x, y, -1).normalize();
			framebuffer[i + j * width] = castRay(Vec3f(0, 0, 0), dir, scene, lights);
		}
	}

	std::ofstream ofs; // save the framebuffer to file
	ofs.open("./out.ppm", std::ofstream::out | std::ofstream::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (size_t i = 0; i < height*width; ++i) {
		for (size_t j = 0; j < 3; j++) {
			Vec3f &c = framebuffer[i];
			float max = std::max(c[0], std::max(c[1], c[2])); //saturation
			if (max > 1) c = c * (1. / max); //normalize

			ofs << (char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
		}
	}
	ofs.close();
}

int main() {

	Material ivory		(1.0, Vec4f(0.6, 0.3, 0.1, 0.0), Vec3f(0.4, 0.4, 0.3), 50.);
	Material glass		(1.5, Vec4f(0.0, 0.5, 0.1, 0.8), Vec3f(0.6, 0.7, 0.8), 125.);
	Material red_rubber	(1.0, Vec4f(0.9, 0.1, 0.0, 0.0), Vec3f(0.3, 0.1, 0.1), 10.);
	Material mirror		(1.0, Vec4f(0.0, 10.0, 0.8, 0.0), Vec3f(1.0, 1.0, 1.0), 1425.);

	std::vector<Sphere> scene;
	scene.emplace_back(Vec3f(-3, 0, -16), 2, ivory);
	scene.emplace_back(Vec3f(-1.0, -1.5, -12), 2, glass);
	scene.emplace_back(Vec3f(1.5, -0.5, -18), 3, red_rubber);
	scene.emplace_back(Vec3f(7, 5, -18), 4, mirror);

	std::vector<Light> lights;
	lights.emplace_back(Vec3f(-20, 20, 20), 1.5f);
	lights.emplace_back(Vec3f(30, 50, -25), 1.8);
	lights.emplace_back(Vec3f(30, 20, 30), 1.7);

	render(scene,lights);
	return 0;
}