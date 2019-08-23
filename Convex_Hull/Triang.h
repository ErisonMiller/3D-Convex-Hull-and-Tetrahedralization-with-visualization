#pragma once

#include "Geometry.h"


Object Start_Tri(const Object &o1, ObjectTetra &ot)
{
	Object o;

	Vector3Df *v1 = o1.vertices[0];
	Vector3Df *v2 = o1.vertices[1];
	Vector3Df *v3 = o1.vertices[2];
	Vector3Df *v4 = o1.vertices[3];
	o.add_vertex(v1);
	o.add_vertex(v2);
	o.add_vertex(v3);
	o.add_vertex(v4);
	const Vector3Df center = (*v1 + *v2 + *v3 + *v4) / 4.0f;

	Triangle f;

	ot.add_tetra(Tetra(v1, v2, v3, v4));

	//face 1
	f = Triangle{ v1, v2, v3 };
	if (dot(f.getNormal(), (*v1 - center).to_unitary()) < QUASE_ZERO) {
		o.add_face(f);
	}
	else { o.add_face(Triangle{ v1, v3, v2 }); }

	//face 2
	f = Triangle{ v2, v3, v4 };
	if (dot(f.getNormal(), (*v2 - center).to_unitary()) < QUASE_ZERO) {
		o.add_face(f);
	}
	else { o.add_face(Triangle{ v2, v4, v3 }); }

	//face 3
	f = Triangle{ v1, v2, v4 };
	if (dot(f.getNormal(), (*v1 - center).to_unitary()) < QUASE_ZERO) {
		o.add_face(f);
	}
	else { o.add_face(Triangle{ v1, v4, v2 }); }

	//face 4
	f = Triangle{ v1, v4, v3 };
	if (dot(f.getNormal(), (*v1 - center).to_unitary()) < QUASE_ZERO) {
		o.add_face(f);
	}
	else { o.add_face(Triangle{ v1, v3, v4 }); }

	return o;
}

Object SelectExterPoints(const vector<Triangle> &out_faces, Object &o, Vector3Df *v)
{
	vector<Edge> edges;
	edges.reserve(out_faces.size());

	for (const Triangle &t : out_faces) {
		for (int i = 0; i < 3; i++) {
			Edge e1{ t._v[i], t._v[(i + 1) % 3] };
			unsigned e_index = 0;
			bool find = false;
			for (const Edge &e : edges) {
				if (e == e1) {
					find = true;
					edges[e_index] = edges.back();
					edges.pop_back();
					break;
				}
				e_index++;
			}
			if (!find)edges.push_back(e1);
		}
	}

	for (const Edge &e : edges) {
		o.faces.push_back(Triangle{ e.v1,e.v2,v });
	}
	return o;
}


Object AddPoint(const Object &o, ObjectTetra &ot, Vector3Df *v)
{
	Object Oaux;
	//insert v into the vertices
	Oaux.vertices = o.vertices;
	Oaux.vertices.push_back(v);
	Oaux.faces.reserve(o.faces.size());

	//extern faces,, that need to be put of;
	vector<Triangle> out_faces;
	out_faces.reserve(o.faces.size());

	//interate in the object faces to see what faces will stay at the hull 
	for (const Triangle &t : o.faces) {
		if (PonitIsOutFace(t, *v)) {
			out_faces.push_back(t);
		}
		else {
			Oaux.faces.push_back(t);
		}
	}

	if (!out_faces.empty()){
		for (const Triangle &f : out_faces) {
			if (fabs(dot(f.getNormal().to_unitary(), *v - *f.v1)) > SMALL_NUMBER) {
				ot.add_tetra(Tetra(f.v1, f.v2, f.v3, v));
			}
		}
	}

	Oaux = SelectExterPoints(out_faces, Oaux, v);

	return Oaux;
}
//*/





/*
vector<Triangle> Update_border(vector<Triangle> &border, vector<Triangle> &tetra) {
	int i = 0, s = border.size();
	while (i < s && tetra.size()>0) {
		Triangle tb = border[i];

		bool duplicated = false;
		int j = 0;
		while ( j < tetra.size()) {
			if (tb == tetra[j]) { duplicated = true; break; }
			j++;
		}
		if (duplicated) {
			tetra[j] = tetra.back();
			tetra.pop_back();
			border[i] = border.back();
			border.pop_back();
			s--;
		}
		else {
			i++;
		}
	}
	return tetra;
}

bool Colide(ObjectTetra tr, Triangle f, Vector3Df *v4) {
	
	Tetra t(f.v1, f.v2, f.v3, v4);
	t.reduce(0.1);
	
	for (Tetra &to : tr.tetras) {
		if (to.Colide(t)) {
			return true;
		}
	}
	return false;
}

vector<Triangle> Add_Point(vector<Vector3Df*> &points, Triangle &t, ObjectTetra &triang, Object tr) {
	
	Vector3Df n = t.getNormal().to_unitary();
	float d, min_dist = INFINITY;
	
	int closest_point = -1, s = points.size();
	for (int i = 0; i < s; i++) {
		if (t.has(*points[i]))continue;//nao add os pontos q ja fazem parte do triangulo
		d = (*points[i] - t.getCenter()).lengthsq();//normal pra dentro
		
		if (dot(*points[i] - *t.v1, n) > 0.0001 && d < min_dist && !Colide(triang, t, points[i])) {
			min_dist = d;
			closest_point = i;
		}
	}
	

	vector<Triangle> tetra; 
	if (closest_point >= 0) {

		Vector3Df *p = points[closest_point];
		
		tetra.push_back(Triangle{ t.v1, t.v2, p });
		tetra.push_back(Triangle{ t.v2, t.v3, p });
		tetra.push_back(Triangle{ t.v3, t.v1, p });

		triang.add_tetra(Tetra(t.v1, t.v2, t.v3, p));
	}
	return tetra;
}

ObjectTetra Triang(Object &hull) {
	vector<Vector3Df*> points = hull.vertices;
	Object triang = hull;
	ObjectTetra tetras;
	//triang.faces.clear();
	vector<Triangle> border = hull.faces;
	
	while (!border.empty()) {
		vector<Triangle> tetra = Add_Point(points, border.back(), tetras, triang);
		border.pop_back();
		
		tetra = Update_border(border, tetra);
		
		for (Triangle tt : tetra) {
			bool existe = false;
			for (Triangle tf : triang.faces) {
				if (tt == tf) {
					//std::cout << "\n\n %ии&%$иии&*(ии&%&*и%и&%&%$%и&*&и%&(&(и*&%&aaaaaaaaaaaaaaaaa\n";
					existe = true; break;
				}
			}
			if (!existe) {
				//std::cout << "tetras " << tetras.tetras.size() << "\n";
				//std::cout << "tri " << triang.faces.size() << "\n";
				//std::cout << "borda " << border.size() << "\n\n";

				triang.add_face(tt);
				border.push_back(tt);
			}
		}

	}
	return tetras;
}//*/