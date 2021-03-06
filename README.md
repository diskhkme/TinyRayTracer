# TinyRayTracer
https://github.com/ssloy/tinyraytracer/wiki/Part-1:-understandable-raytracing

Step1: 이미지 저장하기
-----------
* Geometry / 이미지 저장 코드
* Geometry.h에는 2차원/3차원/4차원 벡터 및 그 연산자 오버로딩 정의되어 있음
* main.cpp에서는 출력 이미지를 std::vector<Vec3f>로 만들고, 이를 ppm파일로 출력

Step2: 원 렌더링하기
----------
* Ray Intersect
* Cast Ray
* Calculate dir for each pixel
</br><img src="/TinyRayTracer/Img/Step2.png" width="540" height="960"></img>

Step3: 원 여러개 렌더링하기
----------
* 색깔 멤버 추가
* 앞에 있는 색상이 칠해지도록 로직 추가

Step4: Lighting
----------
* normal과 light vector간 dot product로 intensity 계산

Step5: Specular Lighting
----------
* Material 추가
* specular lighting 계산

Step6: Shadow
----------
* point에서부터 light 방향으로 intersection을 한 번 더 계산하여 shadow 생성
</br><img src="/TinyRayTracer/Img/Step6.png" width="540" height="960"></img>

Step7: Reflection
----------
* dir의 refelction 방향으로 recursive하게 빛을 쏘아 반사 효과 생성

Step8: Refraction
----------
* Snell's law를 사용하여 굴절 효과 생성
</br><img src="/TinyRayTracer/Img/Step8.jpg" width="540" height="960"></img>

Step9 : Add checkerboard
----------
* 직선, 평면의 교차 식을 이용하여 checkerboard 평면에 대한 interection을 추가 계산