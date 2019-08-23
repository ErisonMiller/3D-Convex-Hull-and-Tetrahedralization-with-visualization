#pragma once

#include "Geometry.h"

Object Start_Hull(Object o1)
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

	//face 1
	f = Triangle{ v1, v2, v3 };
	if (dot(f.getNormal(), (*v1 - center).to_unitary()) < SMALL_NUMBER) {
		o.add_face(f);
	}
	else { o.add_face(Triangle{ v1, v3, v2 }); }

	//face 2
	f = Triangle{v2, v3, v4};
	if (dot(f.getNormal(), (*v2 - center).to_unitary()) < SMALL_NUMBER) {
		o.add_face(f);
	}
	else { o.add_face(Triangle{ v2, v4, v3 }); }

	//face 3
	f = Triangle{ v1, v2, v4 };
	if (dot(f.getNormal(), (*v1 - center).to_unitary()) < SMALL_NUMBER) {
		o.add_face(f);
	}
	else { o.add_face(Triangle{ v1, v4, v2 }); }

	//face 4
	f = Triangle{ v1, v4, v3 };
	if (dot(f.getNormal(), (*v1 - center).to_unitary()) < SMALL_NUMBER) {
		o.add_face(f);
	}
	else { o.add_face(Triangle{ v1, v3, v4 }); }

	return o;
}


Object SelectExterPoints(const vector<Triangle> &out_faces, Object &o, Vector3Df *v)
{
	vector<Edge> edges;
	edges.reserve(out_faces.size());

	for (const Triangle t : out_faces) {
		for (int i = 0; i < 3; i++) {
			Edge e1{ t._v[i], t._v[(i + 1) % 3] };
			unsigned e_index = 0;
			bool find = false;
			for (Edge e : edges) {
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

	for (const Edge e : edges) {
		o.faces.push_back(Triangle{ e.v1,e.v2,v });
	}
	return o;
}


Object AddPoint(const Object &o, Vector3Df *v)
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
	for (const Triangle t: o.faces) {
		if (PonitIsOutFace(t, *v)) {
			out_faces.push_back(t);
		}
		else {
			Oaux.faces.push_back(t);
		}
	}

	Oaux = SelectExterPoints(out_faces, Oaux, v);

	return Oaux;
}
