#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gl/glut.h>
#include "gmath.h"

// 전역 변수
#define MAX_DEPTH 3
#define H 768
#define W 1024
unsigned char Image[H * W * 3];
std::vector<GSphere> SphereList;
std::vector<GLight> LightList;

// 콜백 함수 선언
void Render();
void Reshape(int w, int h);

// 광선 추적 함수
int GetIdx(int i, int j);
void CreateImage();
GVec3 RayTrace(GLine ray, int depth);
GVec3 Phong(GPos3 P, GVec3 N, GSphere Obj);
bool intersect_line_sphere(GLine ray, int &sidx, double &t);

int main(int argc, char **argv)
{
	// OpenGL 초기화, 윈도우 크기 설정, 디스플레이 모드 설정
	glutInit(&argc, argv);
	glutInitWindowSize(W, H);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// 윈도우 생성 및 콜백 함수 등록
	glutCreateWindow("RayTracer");
	glutDisplayFunc(Render);
	glutReshapeFunc(Reshape);
	
	// 조명 설정
	GLight Light0;
	Light0.Pos.Set(0.0, 0.0, 0.0);
	Light0.Ia.Set(0.2, 0.2, 0.2);
	Light0.Id.Set(0.7, 0.7, 0.7);
	Light0.Is.Set(0.8, 0.8, 0.8);
	LightList.push_back(Light0);

	// 장면에 구를 배치한다.
	GSphere Sphere0;
	Sphere0.Pos.Set(-80, -50, -450.0);
	Sphere0.Rad = 50.0;
	Sphere0.Ka.Set(0.2, 0.2, 0.8);
	Sphere0.Kd.Set(0.0, 0.0, 0.7);
	Sphere0.Ks.Set(0.9, 0.9, 0.9);
	Sphere0.ns = 8.0;
	SphereList.push_back(Sphere0);

	GSphere Sphere1;
	Sphere1.Pos.Set(-80, 0, -400.0);
	Sphere1.Rad = 50.0;
	Sphere1.Ka.Set(0.8, 0.2, 0.2);
	Sphere1.Kd.Set(0.7, 0.0, 0.0);
	Sphere1.Ks.Set(0.9, 0.9, 0.9);
	Sphere1.ns = 8.0;
	SphereList.push_back(Sphere1);

	GSphere Sphere2;
	Sphere2.Pos.Set(0, -50, -500.0);
	Sphere2.Rad = 50.0;
	Sphere2.Ka.Set(0.2, 0.8, 0.2);
	Sphere2.Kd.Set(0.0, 0.7, 0.0);
	Sphere2.Ks.Set(0.9, 0.9, 0.9);
	Sphere2.ns = 8.0;
	SphereList.push_back(Sphere2);
	
	// 이미지를 생성
	CreateImage();

	// 이벤트를 처리를 위한 무한 루프로 진입한다.
	glutMainLoop();
	
	return 0;
}

void Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}


void timer(int id)
{
	static double theta = 0.0;
}

void Render()
{
	// 칼라 버퍼와 깊이 버퍼 지우기
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// frame에따른 이미지 변화 update
	// imageUpdate();

	// 칼라 버퍼에 Image 데이터를 직접 그린다.
	glDrawPixels(W, H, GL_RGB, GL_UNSIGNED_BYTE, Image);
	
	// 칼라 버퍼 교환한다
	glutSwapBuffers();
}

void CreateImage()
{
	int x0 = -W / 2;
	int y0 = H / 2 - 1;
	double z = -(H / 2) / tan(M_PI * 15 / 180.0);
	for (int i = 0; i < H; ++i)
	{
		for (int j = 0; j < W; ++j)
		{
			double x = x0 + j;
			double y = y0 - i;
			// 광선만들기 (시점) -> 현재목표위치)
			GLine ray(GPos3(0.0, 0.0, 0.0), GPos3(x, y, z));
			GVec3 Color = RayTrace(ray, 0);

			int idx = (H - 1 - i) * W * 3 + j * 3;

			// Threshold color
 			unsigned char r = (Color[0] > 1.0) ? 255 : Color[0] * 255;
 			unsigned char g = (Color[1] > 1.0) ? 255 : Color[1] * 255;
 			unsigned char b = (Color[2] > 1.0) ? 255 : Color[2] * 255;

			Image[idx] = r;
			Image[idx + 1] = g;
			Image[idx + 2] = b;
		}
	}
}

GVec3 RayTrace(GLine ray, int depth)
{
	GVec3 C; // color
	if (depth++ > MAX_DEPTH)
		return C;

	double k_reflect = 0.3; // 반사광 계수
	double k_refract = 0.3; // 굴절광 계수

	int sidx;	// 광선과 교차하는 가장 까가운 구의 인덱스
	// t* 값이 가장 적은애로. 
	double t;	// 교차점에서 광선의 파라미터 t
				// GLINE.Evall(t)쓰면 된당.
				

	if (intersect_line_sphere(ray, sidx, t))
	{
		// 구현...
		GPos3 P = ray.Eval(t);
		GVec3 N = N.Normalize(), // normalize vector. ray? sphere?
			V, // 법선?
			R; // 반사광선의 방향벡터?
		GLine ray_reflect, ray_refract; // 여기있는데.. 
		/*
		p = point_of_intersection(v, S);
		R = reflection(v, S, p);
		T = refraction(v, S, p);
		*/

// 		C = Phong(P, N, SphereList[sidx]) 
// 			+ k_reflect * RayTrace(ray_reflect, depth)	// 반사광선
// 			+ k_refract * RayTrace(ray_refract, depth);	// 굴절광선
	}

	return C;
}

// 퐁셰이딩
GVec3 Phong(GPos3 P, GVec3 N, GSphere Obj)
{
	GVec3 C;
	// 광원의 특징 * 구의 재질..을 통한 색상계산.
	// la*ka, ld*kd, ls*ks
	C = Obj.Ka + Obj.Kd + Obj.Ks;
	return C;
}

bool intersect_line_sphere(GLine ray, int &sidx, double &t)
{
	// 구의 중심에서 광원방향.

	// sidx :: ray에서 가장 가까운 구의 idx
	// t : 교차점

	// spherelist내의 구와 광선의 교차점을 찾는당
	
	// 만나는 구가 있으면
	return true;

	// 광선이 교차하는 곡면이 없거나, 사용자가 정의한 최대깊이를 초과했을떄.
	return false;
	
}