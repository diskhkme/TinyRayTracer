# TinyRayTracer
https://github.com/ssloy/tinyraytracer/wiki/Part-1:-understandable-raytracing

Step1: 이미지 저장하기
-----------
*Geometry / 이미지 저장 코드
 * Geometry.h에는 2차원/3차원/4차원 벡터 및 그 연산자 오버로딩 정의되어 있음
 * main.cpp에서는 출력 이미지를 std::vector<Vec3f>로 만들고, 이를 ppm파일로 출력