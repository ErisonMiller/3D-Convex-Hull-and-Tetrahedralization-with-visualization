//cgal convexhull
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <windows.h>
#include <time.h>
#include <omp.h>
#include <thread>

#include <algorithm>
//opengl includes
#include <GL/glut.h>

//my includes
//#include "Convex_Hull.h"
#include "Triang.h"


int width = 900, height = 900;
Camera cam;

bool show_original = true;
bool show_t = false, ani = false;

float size_triang = 0.5f;

Object obj;
vector<Object> objs, hulls;
vector<ObjectTetra> triangs;

void start_render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	{

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(0, 0, cam.resolution.x, cam.resolution.y);
		int largura = cam.fov.x*cam.resolution.x, altura = cam.fov.y*cam.resolution.y;
		glFrustum(-largura / 2, largura / 2, -altura / 2, altura / 2, 1, 100);
		glMatrixMode(GL_MODELVIEW);

		glLoadIdentity();

		gluLookAt(cam.position.x, cam.position.y, cam.position.z,
			cam.view.x, cam.view.y, cam.view.z,
			cam.up.x, cam.up.y, cam.up.z);


		glPointSize(10.0f);
		glColor3f(0, 0, 1);

		glPointSize(4.0f);
		glLineWidth(2.0f);
		glColor3f(1, 1, 1);
		//look at
		glBegin(GL_POINTS);
		glVertex3f(cam.view.x, cam.view.y, cam.view.z);
		glEnd();
		// Arrows
		glBegin(GL_LINES);
		glColor3f(0, 0.8f, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 1, 0);
		glEnd();
		glLineWidth(2.0f);
		//   grid

		glLineWidth(1.0f);
		glBegin(GL_LINES);
		for (float i = -10; i <= 10; ++i) {

			glColor3f(0.3f, 0.3f, 0.3f);
			if (i == 0.0f) {
				glColor3f(0, 0, 0.8f);
				//glLineWidth(2.0f);
			}
			glVertex3f(i, 0.01f, -10);
			glVertex3f(i, 0.01f, 10);

			if (i == 0.0f)glColor3f(0.8f, 0, 0);
			glVertex3f(10, 0.01f, i);
			glVertex3f(-10, 0.01f, i);
		}
		glEnd();
	}
}

void RenderObject(const Object &o) {
	float dist = 0.01f;
	glLineWidth(1.0f);
	for (const Triangle &t : o.faces) {

		glColor3f(o.cor.x, o.cor.y, o.cor.z);
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < 3; i++) {
			glVertex3f(t._v[i]->x, t._v[i]->y, t._v[i]->z);
		}
		glEnd();

		Vector3Df n = t.getNormal();
		n.normalize();

		glColor3f(0.1f, 0.1f , 0.1f);
		glBegin(GL_LINES);
		for (int i = 0; i < 3; i++) {
			glVertex3f(t._v[i]->x +n.x*dist, t._v[i]->y + n.y*dist, t._v[i]->z + n.z*dist);
			glVertex3f(t._v[(i+1)%3]->x + n.x*dist, t._v[(i + 1) % 3]->y + n.y*dist, t._v[(i + 1) % 3]->z + n.z*dist);

			n *= -1;
			glVertex3f(t._v[i]->x + n.x*dist, t._v[i]->y + n.y*dist, t._v[i]->z + n.z*dist);
			glVertex3f(t._v[(i + 1) % 3]->x + n.x*dist, t._v[(i + 1) % 3]->y + n.y*dist, t._v[(i + 1) % 3]->z + n.z*dist);
		}
		glEnd();
	}
	glPointSize(1.5f);
	glBegin(GL_POINTS);
	for (const Vector3Df *v : o.vertices) {

		glColor3f(0.5f, 0.1f, 0.1f);
		glVertex3f(v->x, v->y, v->z);
		
	}
	glEnd();

}

void RenderTetra(const ObjectTetra &o) {
	glLineWidth(1.0f);
	for (const Tetra &tt : o.tetras) {
		for (const Face &t : tt.faces) {
			const Vector3Df cor = tt.cor * ( fabs(dot(t.getNormal(), Vector3Df{ 1, 1, 1 }))*0.7f + 0.3f );

			glColor3f(cor.x, cor.y, cor.z);
			glBegin(GL_TRIANGLES);
				glVertex3f(t._v[0].x, t._v[0].y, t._v[0].z);
				glVertex3f(t._v[1].x, t._v[1].y, t._v[1].z);
				glVertex3f(t._v[2].x, t._v[2].y, t._v[2].z);
			glEnd();
		}
	}
	
}


void Load_Obj(string name) {

	objs.clear();
	hulls.clear();
	obj.clear();
	triangs.clear();
	
	ifstream infile(name);

	string tag;
	int num_objs = -1;
	int f1, f2, f3;
	float x, y, z;
	cout << "calculando...\n";
	while (infile >> tag) {
		if (tag == "o") {
			num_objs++;
			objs.push_back(Object());
			objs[num_objs].cor = Vector3Df{ (rand() % 1000)*0.001f, (rand() % 1000)*0.001f , (rand() % 1000)*0.001f };
			//cout << "Abri\n";

			continue;
		}
		if (tag == "v") {
			//num_vertices++;
			infile >> x;
			infile >> y;
			infile >> z;
			Vector3Df *p = new Vector3Df{ x, y, z };
			obj.add_vertex(p);
			objs[num_objs].add_vertex(p);
			continue;
		}
		if (tag == "f") {
			infile >> f1;
			infile >> f2;
			infile >> f3;
			//Surface_mesh s = objs[num_objs];
			//s.ver
			//objs[num_objs].add_face(f1 - num_vertices, f2 - num_vertices, f3 - num_vertices);
			objs[num_objs].add_face(Triangle{ obj.vertices[f1 - 1], obj.vertices[f2 - 1], obj.vertices[f3 - 1] });
			continue;
		}

		getline(infile, tag);
		continue;
	}

	infile.close();


	for (Object &o: objs) {
		std::sort(o.vertices.begin(), o.vertices.end(), [](const Vector3Df *v1, const Vector3Df *v2){return v1->y < v2->y;});
		
		ObjectTetra triang;
		Object hull = Start_Tri(o, triang);
		//Object hull = Start_Hull(o);


		int size = o.vertices.size();
		//cout << "Size: " << size << "\n";
		for (int i = 4; i < size; i++) {
			hull = AddPoint(hull, triang, o.vertices[i]);
			//hull = AddPoint(hull, o.vertices[i]);
			//cout << "[ " << ((i+1)*100.0f)/size << "% ]                              \r";
		}
		//triang = Triang(hull);
		
		triang.cor = o.cor;
		hull.cor = o.cor;
		hulls.push_back(hull);
		triangs.push_back(triang);
	}

	cout << "Acabei :)\n";

	unsigned num_tetras = 0;
	unsigned num_faces = 0;
	unsigned num_hulls = 0;
	unsigned num_pontos = 0; 

	for (ObjectTetra &ot : triangs) {
		num_tetras += ot.tetras.size();
		for (Tetra &t : ot.tetras) {
			t.reduce(size_triang);
		}
	}
	for (const Object &o : hulls) {
		num_hulls += o.faces.size();
	}
	for (const Object &o : objs) {
		num_faces += o.faces.size();
		num_pontos += o.vertices.size();
	}
	
	cout << "Pontos : " << num_pontos << "\n";
	cout << "Tetras : " << num_tetras << "\n";
	cout << "Fecho  : " << num_hulls  << "\n";
	cout << "Faces  : " << num_faces  << "\n";


}

// display function called by glutMainLoop(), gets executed every frame 
void disp(void)
{
	start_render();

	if (show_t) {
		for (const ObjectTetra &ot : triangs) {
			RenderTetra(ot);
		}
	}
	else {
		vector<Object> render_objects = objs;
		if (!show_original) render_objects = hulls;

		for (const Object &o : render_objects) {
			RenderObject(o);
		}
	}
	
	
	glutSwapBuffers();

}

// mouse event handlers

int lastX = 0, lastY = 0;
int theButtonState = 0;
int theModifierState = 0;

// camera mouse controls in X and Y direction
void motion(int x, int y)
{
	int deltaX = lastX - x;
	int deltaY = lastY - y;

	if (deltaX != 0 || deltaY != 0) {

		if (theButtonState == GLUT_LEFT_BUTTON)  // Rotate
		{
			Vector3Df v = cross(cam.view - cam.position, cam.up);
			cam.Transform(rotateArbitrary(deltaY, v));
			cam.Transform(rotateY(deltaX));
		}
		else if (theButtonState == GLUT_MIDDLE_BUTTON) // Zoom
		{

		}

		if (theButtonState == GLUT_RIGHT_BUTTON) // camera move
		{

			cam.position += cam.position*(deltaY*0.01f);
			//float2{ cam.fov.x * (1+deltaY*0.1f), cam.fov.y * (1 + (deltaY*0.1f)) };
		//interactiveCamera->changeRadius(-deltaY * 0.01);
		}

		lastX = x;
		lastY = y;
		glutPostRedisplay();

	}
}

void mouse(int button, int state, int x, int y)
{
	theButtonState = button;
	theModifierState = glutGetModifiers();
	lastX = x;
	lastY = y;

	motion(x, y);
}

void keyboard(unsigned char key, int /*x*/, int /*y*/) {

	string name;
	switch (key) {

	case(27): exit(0);
	case(' '):
		break;
	case('l'):
		cout << "Qual o nome do arquivo?\n";
		cin >> name;
		Load_Obj(name);
		break;
	case('o'):
		show_original = !show_original;
		break;
	case('q'):
		
		size_triang += 0.05f;
		for (ObjectTetra &ot : triangs) {
			for (Tetra &t : ot.tetras) {
				t.reduce(size_triang);
			}
		}
		break;
	case('w'):
		size_triang -= 0.05f;
		for (ObjectTetra &ot : triangs) {
			for (Tetra &t : ot.tetras) {
				t.reduce(size_triang);
			}
		}
		break;
	case('t'):
		show_t = !show_t;
		break;
	case('s'):
		cam = Camera{ Vector3Df{0,-10,0},
		Vector3Df{0,0,0},
		Vector3Df{0,0,1} };
		cam.resolution = float2{ width*1.0f, height*1.0f };
		cam.fov = float2{ 2.0f / width, 2.0f / height };
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char** argv) {
	//init camera
	cam = Camera{ Vector3Df{0,-10,0},
		Vector3Df{0,0,0},
		Vector3Df{0,0,1} };
	cam.resolution = float2{ width*1.0f, height*1.0f };
	cam.fov = float2{ 2.0f / width, 2.0f / height };

	// init glut:
	glutInit(&argc, argv);
	// specify the display mode to be RGB and single buffering:
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	// specify the initial window position:
	glutInitWindowPosition(100, 100);
	// specify the initial window size:
	glutInitWindowSize(width, height);
	// create the window and set title:
	glutCreateWindow("Convex Hull");


	
	// functions for user interaction
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(keyboard);


	// init opengl:
	glClearColor(0.8f, 0.8f, 0.8f, 0.0f);

	glMatrixMode(GL_PROJECTION);
	glViewport(0, 0, cam.resolution.x, cam.resolution.y);
	int largura = cam.fov.x*cam.resolution.x, altura = cam.fov.y*cam.resolution.y;
	glFrustum(-largura / 2, largura / 2, altura / 2, -altura / 2, 1, 100);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND);
	//glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Load_Obj("crab.obj");
	// register callback function to display graphics:
	glutDisplayFunc(disp);

	glutMainLoop();
	
	return 0;
	
}
