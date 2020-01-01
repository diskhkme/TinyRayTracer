#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm> //<-- max/min 사용하기 위해 추가
#include "geometry.h"

struct Sphere
{
	Vec3f center;
	float radius;

	Sphere(const Vec3f& c, const float& r) : center{ c }, radius{ r }
	{

	}

	bool rayIntersect(const Vec3f& orig, const Vec3f& dir, float& t0) const
	{
		Vec3f L = center - orig; //orig에서 출발하여 center를 향하는 벡터
		float tca = L * dir; // L vector를 dir vector에 projection
		float d2 = L * L - tca * tca; //L^2 = tca^2 + r^2인 경우 접하는 삼각형.
		if (d2 > radius*radius) return false; //r^2보다 거리가 멀면, ray가 원에 부딫히지 않음
		float thc = sqrtf(radius*radius - d2);
		t0 = tca - thc; //t0는 참조자로 넘어온 값. 여기에서 바뀜
		float t1 = tca + thc; //t0와 t1은 가까운/먼 교차점까지의 파라메터
		if (t0 < 0) t0 = t1;  //접점이 하나인 경우 or dir 반대 방향에 원이 있는 경우를 처리
		if (t0 < 0) return false;
		return true;
	}

};

Vec3f castRay(const Vec3f& orig, const Vec3f& dir, const Sphere& sphere)
{
	float sphereDist = std::numeric_limits<float>::max();
	if (!sphere.rayIntersect(orig, dir, sphereDist))
	{
		return Vec3f(0.2, 0.7, 0.8); //부딫히지 않은 경우
	}
	return Vec3f(0.4, 0.4, 0.3); //부딫힌 경우
}

void render() {
	const int width = 1024;
	const int height = 768;
	const int fov = 3.14f / 2.f;
	std::vector<Vec3f> framebuffer(width*height);

	Sphere sphere{ Vec3f(0,0,-5),1.f };

	for (size_t j = 0; j < height; j++) {
		for (size_t i = 0; i < width; i++) {
			float x = (2 * (i + 0.5) / (float)width - 1) * tan(fov / 2.)*width / (float)height;
			float y = -(2 * (j + 0.5) / (float)height - 1) * tan(fov / 2.);
			Vec3f dir = Vec3f(x, y, -1).normalize();
			framebuffer[i + j * width] = castRay(Vec3f(0, 0, 0), dir, sphere); //카메라는 0,0,0에 위치
		}
	}

	std::ofstream ofs; // save the framebuffer to file
	ofs.open("./out.ppm", std::ofstream::out | std::ofstream::binary); //<--수정
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (size_t i = 0; i < height*width; ++i) {
		for (size_t j = 0; j < 3; j++) {
			ofs << (char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
		}
	}
	ofs.close();
}

int main() {
	render();
	return 0;
}