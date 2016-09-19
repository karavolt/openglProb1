#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gl/glut.h>
#include "gmath.h"

// ���� ����
#define MAX_DEPTH 3
#define H 768
#define W 1024
unsigned char Image[H * W * 3];
std::vector<GSphere> SphereList;
std::vector<GLight> LightList;

// �ݹ� �Լ� ����
void Render();
void Reshape(int w, int h);

// ���� ���� �Լ�
int GetIdx(int i, int j);
void CreateImage();
GVec3 RayTrace(GLine ray, int depth);
GVec3 Phong(GPos3 P, GVec3 N, GSphere Obj);
bool intersect_line_sphere(GLine ray, int &sidx, double &t);

int main(int argc, char **argv)
{
	// OpenGL �ʱ�ȭ, ������ ũ�� ����, ���÷��� ��� ����
	glutInit(&argc, argv);
	glutInitWindowSize(W, H);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// ������ ���� �� �ݹ� �Լ� ���
	glutCreateWindow("RayTracer");
	glutDisplayFunc(Render);
	glutReshapeFunc(Reshape);
	
	// ���� ����
	GLight Light0;
	Light0.Pos.Set(0.0, 0.0, 0.0);
	Light0.Ia.Set(0.2, 0.2, 0.2);
	Light0.Id.Set(0.7, 0.7, 0.7);
	Light0.Is.Set(0.8, 0.8, 0.8);
	LightList.push_back(Light0);

	// ��鿡 ���� ��ġ�Ѵ�.
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
	
	// �̹����� ����
	CreateImage();

	// �̺�Ʈ�� ó���� ���� ���� ������ �����Ѵ�.
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
	// Į�� ���ۿ� ���� ���� �����
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// frame������ �̹��� ��ȭ update
	// imageUpdate();

	// Į�� ���ۿ� Image �����͸� ���� �׸���.
	glDrawPixels(W, H, GL_RGB, GL_UNSIGNED_BYTE, Image);
	
	// Į�� ���� ��ȯ�Ѵ�
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
			// ��������� (����) -> �����ǥ��ġ)
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

	double k_reflect = 0.3; // �ݻ籤 ���
	double k_refract = 0.3; // ������ ���

	int sidx;	// ������ �����ϴ� ���� ��� ���� �ε���
	// t* ���� ���� �����ַ�. 
	double t;	// ���������� ������ �Ķ���� t
				// GLINE.Evall(t)���� �ȴ�.
				

	if (intersect_line_sphere(ray, sidx, t))
	{
		// ����...
		GPos3 P = ray.Eval(t);
		GVec3 N = N.Normalize(), // normalize vector. ray? sphere?
			V, // ����?
			R; // �ݻ籤���� ���⺤��?
		GLine ray_reflect, ray_refract; // �����ִµ�.. 
		/*
		p = point_of_intersection(v, S);
		R = reflection(v, S, p);
		T = refraction(v, S, p);
		*/

// 		C = Phong(P, N, SphereList[sidx]) 
// 			+ k_reflect * RayTrace(ray_reflect, depth)	// �ݻ籤��
// 			+ k_refract * RayTrace(ray_refract, depth);	// ��������
	}

	return C;
}

// �����̵�
GVec3 Phong(GPos3 P, GVec3 N, GSphere Obj)
{
	GVec3 C;
	// ������ Ư¡ * ���� ����..�� ���� ������.
	// la*ka, ld*kd, ls*ks
	C = Obj.Ka + Obj.Kd + Obj.Ks;
	return C;
}

bool intersect_line_sphere(GLine ray, int &sidx, double &t)
{
	// ���� �߽ɿ��� ��������.

	// sidx :: ray���� ���� ����� ���� idx
	// t : ������

	// spherelist���� ���� ������ �������� ã�´�
	
	// ������ ���� ������
	return true;

	// ������ �����ϴ� ����� ���ų�, ����ڰ� ������ �ִ���̸� �ʰ�������.
	return false;
	
}