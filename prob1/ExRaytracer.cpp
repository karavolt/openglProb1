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
static int funccnt = 0;

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
	Light0.Pos.Set(-40.0, 40.0, 350.0);
	Light0.Ia.Set(0.2, 0.2, 0.2);
	Light0.Id.Set(0.7, 0.7, 0.7);
	Light0.Is.Set(0.8, 0.8, 0.8);
	LightList.push_back(Light0);

	// ��鿡 ���� ��ġ�Ѵ�.
	GSphere Sphere0;
	Sphere0.Pos.Set(-80, 0, -450.0);
	Sphere0.Rad = 50.0;
	Sphere0.Ka.Set(0.2, 0.2, 0.8);
	Sphere0.Kd.Set(0.0, 0.0, 0.7);
	Sphere0.Ks.Set(0.9, 0.9, 0.9);
	Sphere0.ns = 8.0;
	SphereList.push_back(Sphere0);

	GSphere Sphere1;
	Sphere1.Pos.Set(80, 0, -450.0);
	Sphere1.Rad = 50.0;
	Sphere1.Ka.Set(0.8, 0.2, 0.2);
	Sphere1.Kd.Set(0.7, 0.0, 0.0);
	Sphere1.Ks.Set(0.9, 0.9, 0.9);
	Sphere1.ns = 8.0;
	SphereList.push_back(Sphere1);


	//GSphere Sphere2;
	//Sphere2.Pos.Set(0, 80, -500.0);
	//Sphere2.Rad = 50.0;
	//Sphere2.Ka.Set(0.2, 0.8, 0.2);
	//Sphere2.Kd.Set(0.0, 0.7, 0.0);
	//Sphere2.Ks.Set(0.9, 0.9, 0.9);
	//Sphere2.ns = 8.0;
	//SphereList.push_back(Sphere2);

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
			//std::cout << i << " " << j << std::endl;
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
		//std::cout << "��1" << std::endl;
		// ����...
		GPos3 P = ray.Eval(t);
		GVec3 N = (P - SphereList[sidx].Pos).Normalize(), // normalize vector. ray? sphere?
			V = ray.v, // �Ի籤
			R = V - ((2.0 * (N * V)) * N); // �ݻ籤
										   //std::cout << "��2" << std::endl;
		GLine ray_reflect(P, R);

		C = Phong(P, N, SphereList[sidx]) +
			k_reflect * RayTrace(ray_reflect, depth);
		//k_refract * RayTrace(ray_refract, depth);	// ��������
		//std::cout << "��3" << std::endl;
	}

	//std::cout << "funccount : " << funccnt << std::endl;
	funccnt++;
	return C;
}

GVec3 multiply(GVec3 lhs, GVec3 rhs) {
	GVec3 temp;
	temp[0] = lhs[0] * rhs[0];
	temp[1] = lhs[1] * rhs[1];
	temp[2] = lhs[2] * rhs[2];
	return temp;
}

// �����̵�
GVec3 Phong(GPos3 P, GVec3 N, GSphere Obj)
{
	GVec3 C;
	GLine L(P, LightList[0].Pos); // ������� ���� -> ���� 
	GVec3 V(LightList[0].Pos - P); // �Ի籤
	GVec3 R = V.Normalize() - ((2 * (N * V.Normalize())) * N);
	// ������ Ư¡ * ���� ����..�� ���� ������.
	// la*ka + ((ld*kd) *(N * L) + ((ls*ks) * (V * R)^ns 

	//C = (multiply(LightList[0].Is, Obj.Ks) * (pow((V.Normalize() * R.Normalize()), Obj.ns)));

	GVec3 amb = multiply(LightList[0].Ia, Obj.Ka);
	GVec3 diff = (multiply(LightList[0].Id, Obj.Kd) * (N * L.v.Normalize()));
	GVec3 spec = (multiply(LightList[0].Is, Obj.Ks) * (pow((V.Normalize() * R.Normalize()), Obj.ns)));


	// �ϴ� ���� �������� ����� �غ���
	C = C + amb + diff + spec;
	//C = C + diff;

	return C;
}


bool intersect_line_sphere(GLine ray, int &sidx, double &t)
{


	for (int i = 0; i < (int)SphereList.size(); i++) {
		GVec3 v1, u1;
		double uv;
		v1 = ray.v;
		u1 = ray.p - SphereList[i].Pos;
		uv = u1 * v1;

		t = 10000000000.0;

		double D = -(uv)+sqrt(pow(uv, 2.0) - (pow(norm(u1), 2.0) - pow(SphereList[i].Rad, 2.0)));


		if (D > 0.0)
		{
			double tmp = -(u1 * v1) - SQRT(D);

			if (tmp > 0.0)
			{
				if (tmp < t)
				{
					sidx = i;
					t = tmp;
					return true;
				}
			}
		}
	}

	return false;
}