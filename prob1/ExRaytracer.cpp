#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gl/glut.h>
#include "gmath.h"

// ���� ����
#define MAX_DEPTH 3

// ������ �����϶��� ����..
const int MAX_SPHERE = 5;
#define H 768
#define W 1024
unsigned char Image[H * W * 3];
std::vector<GSphere> SphereList;
std::vector<GLight> LightList;


// �ݹ� �Լ� ����
void Render();
void Reshape(int w, int h);
void KeyBoard(int key, int x, int y);
void timer(int Value);

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
	glutSpecialFunc(KeyBoard);

#ifdef RELEASE
	glutTimerFunc(300,timer,1);
#endif


	// ���� ���� ���� 2��
	GLight Light0;
	Light0.Pos.Set(0, 0, 1000.0);
	Light0.Ia.Set(0.2, 0.2, 0.2);
	Light0.Id.Set(0.8, 0.8, 0.8);
	Light0.Is.Set(0.9, 0.9, 0.9);
	LightList.push_back(Light0);


	GLight Light1;
	Light1.Pos.Set(-3000.0, 3000.0, 1000.0);
	Light1.Ia.Set(0.2, 0.2, 0.8);
	Light1.Id.Set(0.7, 0.7, 0.7);
	Light1.Is.Set(0.8, 0.2, 0.2);
	LightList.push_back(Light1);

	// rand�Լ��� ���ؼ� 
	time_t t;
	time(&t);
	srand(t);

	// ���� �������� ������ ����
	for (int i = 0; i < MAX_SPHERE; i++)
	{
		GSphere Sphere;
		Sphere.Pos.Set(0.0 + (rand() % 200) - 100, 0.0 + (rand() % 200) - 100, -400.0 - rand() % 100);
		Sphere.Rad = (rand() % 25) + 10.0;
		Sphere.Ka.Set(0.0 + (rand() % 10) / 10.0, 0.0 + (rand() % 10) / 10.0, 0.0 + +(rand() % 10) / 10.0);
		Sphere.Kd.Set(0.0 + (rand() % 10) / 10.0, 0.0 + (rand() % 10) / 10.0, 0.0 + +(rand() % 10) / 10.0);
		Sphere.Ks.Set(0.0 + (rand() % 10) / 10.0, 0.0 + (rand() % 10) / 10.0, 0.0 + +(rand() % 10) / 10.0);
		Sphere.ns = 8.0;
		// �ſ�ó�� �Ϻ��ϰ� �ݻ��ϰ� �Ѵ�
		Sphere.isMirror = rand() % 1;
		SphereList.push_back(Sphere);
	}

	// �ٽ� �׸���
	CreateImage();

	// �̺�Ʈ�� ó���� ���� ���� ������ �����Ѵ�.
	glutMainLoop();

	return 0;
}

void Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void KeyBoard(int key, int x, int y)
{
	// �Ʒ��� ȭ��ǥ�� ������
	if (key == GLUT_KEY_DOWN)
	{
		// ���� �ʱ�ȭ �ϰ�
		SphereList.clear();
		time_t t;
		time(&t);
		srand(t);

		// �ٽóְ�
		for (int i = 0; i < MAX_SPHERE; i++)
		{
			GSphere Sphere;
			Sphere.Pos.Set(0.0 + (rand() % 200) - 100, 0.0 + (rand() % 200) - 100, -400.0 - rand() % 100);
			Sphere.Rad = (rand() % 25) + 10.0;
			Sphere.Ka.Set(0.0 + (rand() % 10) / 10.0, 0.0 + (rand() % 10) / 10.0, 0.0 + +(rand() % 10) / 10.0);
			Sphere.Kd.Set(0.0 + (rand() % 10) / 10.0, 0.0 + (rand() % 10) / 10.0, 0.0 + +(rand() % 10) / 10.0);
			Sphere.Ks.Set(0.0 + (rand() % 10) / 10.0, 0.0 + (rand() % 10) / 10.0, 0.0 + +(rand() % 10) / 10.0);
			Sphere.ns = 8.0;
			Sphere.isMirror = rand() % 1;
			SphereList.push_back(Sphere);
		}
		// �ٽ� �׸���
		CreateImage();
		glutPostRedisplay();
	}
}


void timer(int Value)
{
// ����������϶��� �����ϴ� ���̴�
#ifdef RELEASE
	static double theta = 0.0;

	SphereList.clear();
	time_t t;
	time(&t);
	srand(t);

	// �ٽóְ�
	for (int i = 0; i < MAX_SPHERE; i++)
	{
		GSphere Sphere;
		Sphere.Pos.Set(0.0 + (rand() % 200) - 100, 0.0 + (rand() % 200) - 100, -400.0 - rand() % 100);
		Sphere.Rad = (rand() % 25) + 10.0;
		Sphere.Ka.Set(0.0 + (rand() % 10) / 10.0, 0.0 + (rand() % 10) / 10.0, 0.0 + +(rand() % 10) / 10.0);
		Sphere.Kd.Set(0.0 + (rand() % 10) / 10.0, 0.0 + (rand() % 10) / 10.0, 0.0 + +(rand() % 10) / 10.0);
		Sphere.Ks.Set(0.0 + (rand() % 10) / 10.0, 0.0 + (rand() % 10) / 10.0, 0.0 + +(rand() % 10) / 10.0);
		Sphere.ns = 8.0;
		Sphere.isMirror = rand() % 1;
		SphereList.push_back(Sphere);
	}
	// �ٽ� �׸���
	CreateImage();
	glutPostRedisplay();

	glutTimerFunc(100, timer, 1);
#endif
}


void Render()
{
	// Į�� ���ۿ� ���� ���� �����
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
	
	//time_t start, end, tSum = 0;
	//time(&start);

	for (int i = 0; i < H; ++i)
	{
		/*time_t start, end;
		time(&start);*/
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
		/*time(&end);
		time_t result = end - start;
		tSum += result;
		std::cout << "Line color calc done : " << result << std::endl;*/
	}

	//std::cout << "ImageCrete Done : " << tSum << std::endl;
	/*time(&end);
	time_t result = end - start;
	std::cout << "ImageCrete Done : " << result << std::endl;*/
}

GVec3 RayTrace(GLine ray, int depth)
{
	GVec3 C; // color
	if (depth++ > MAX_DEPTH)
		return C;

	double k_reflect; // �ݻ籤 ���
	//double k_refract = 0.3; // ������ ���

	int sidx;
	double t;

	// ���� ������ ����?
	if (intersect_line_sphere(ray, sidx, t))
	{
		GPos3 P = ray.Eval(t);
		GVec3 N = (P - SphereList[sidx].Pos).Normalize(),	  // normalize vector. ray? sphere?
			V = ray.v,									  // �Ի籤
			R = V - ((2.0 * (N * V)) * N);				  // �ݻ籤

														  // �ݻ籤��~
		GLine ray_reflect(P, R);

		// �ݻ��� ����(���� isMirror ����������)
		if (SphereList[sidx].isMirror)
			k_reflect = 1.0;
		else
			k_reflect = 0.3; // �ݻ籤 ���

		C = Phong(P, N, SphereList[sidx]) + k_reflect * RayTrace(ray_reflect, depth);
	}	
	return C;
}

// ����3 Ÿ���� ������� ���� ��
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
	
	for (int i = 0; i <(int) LightList.size(); i++)
	{
		GVec3 L = (LightList[i].Pos - P).Normalize(); // ������� ���� -> ���� 
		GVec3 V = (P - LightList[i].Pos).Normalize(); // �Ի籤
		GVec3 R = V - 2.0 * (N * V) * N;			  // �ݻ籤

		
		// nl
		double nl = N * L;
		if (nl < 0.0)
			nl = 0.0;

		// vr
		double vr = V * R;
		if (vr >= 0.0)
			vr = 0.0;
		else
			vr = pow(vr, Obj.ns);

		// ������ ���� ���������� �� ���
		GVec3 amb = multiply(LightList[i].Ia, Obj.Ka);
		GVec3 diff = multiply(LightList[i].Id, Obj.Kd);
		GVec3 spec = multiply(LightList[i].Is, Obj.Ks);
		C = C + amb + diff * nl + spec * vr;

	}
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

		// ū���� �༭ ���ϴµ� �����
		t = 10000000000.0;

		// ������ ���� �ε�����?
		double D = SQR(u1 * v1) - (u1*u1 - SphereList[i].Rad * SphereList[i].Rad);

		if (D > 0.0)	// ��
		{
			// t�� ����ϱ�
			double tmp = -(u1 * v1) - SQRT(D);

			// t�� 0���� ũ�� ����.
			if (tmp > 0.0)
			{
				// ���߿��� �� ������..
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