#pragma once
#include "Linear_Algebra.h"
#include <vector>

struct Triangle;

struct Edge
{
	Vector3Df *v1, *v2;

	bool has(Vector3Df *v) const { return (*v1 == *v || *v2 == *v); }
	inline bool operator==(const Edge& e) const { return has(e.v1) && has(e.v2); }

	int side(const Vector3Df &n, const Vector3Df &v) const {
		float d = dot(cross(*v2 - *v1, n), v-*v1);
		if (fabs(d) < SMALL_NUMBER)return 0;
		if (d < -SMALL_NUMBER)return -1;
		return 1;
	}

	int side_with_erro(const Vector3Df &n, const Vector3Df &v) const {
		float d = dot(cross(*v2 - *v1, n), v - *v1);
		//if (fabs(d) < SMALL_NUMBER)return 0;
		//if (fabs(d) < SMALL_NUMBER)return 0;
		//if (d < -SMALL_NUMBER)return -1;
		//return 1;

		if (d > -SMALL_NUMBER)return 1;
		return -1;
	}

	bool colide(const Vector3Df &n, const Edge &e) const {
		if (has(e.v1) || has(e.v2)) return false;
		return side_with_erro(n, *e.v1) * side_with_erro(n, *e.v2) < 0 && e.side_with_erro(n, *v1) * e.side_with_erro(n, *v2) < 0;
	}
};

struct Triangle
{
	union {
		struct { Vector3Df *v1, *v2, *v3; };
		Vector3Df *_v[3];
	};

	inline Vector3Df getNormal() const {
		return cross(*v2 - *v1, *v3 - *v1).to_unitary();
	}
	inline Vector3Df getCenter() const {
		return (*v1 + *v2 + *v3) / 3.0f;
	}
	
	inline bool operator==(const Triangle& t) {
		return this->has(t.v1) && this->has(t.v2) && this->has(t.v3);
	}

	inline bool has(Vector3Df *v) {
		return (v1 == v || v2 == v || v3 == v);
	}
	bool has(Vector3Df v) {
		return (*v1 == v || *v2 == v || *v3 == v);
	}

};

struct Object
{
	vector<Vector3Df*> vertices;
	vector<Triangle> faces;
	Vector3Df cor;

	void clear() { vertices.clear(); faces.clear(); }
	void add_vertex(Vector3Df *v) { vertices.push_back(v); }
	inline void add_face(const Triangle &t) { faces.push_back(t); }
};


struct Face {
	union {
		struct { Vector3Df v1, v2, v3; };
		Vector3Df _v[3];
	};

	Vector3Df getNormal() const {
		return cross(v2 - v1, v3 - v1).to_unitary();
	}
	Vector3Df getCenter() const {
		return (v1 + v2 + v3) / 3.0f;
	}

	inline bool operator==(const Face& t) {
		return this->has(t.v1) && this->has(t.v2) && this->has(t.v3);
	}

	bool has(Vector3Df v) {
		return (v1 == v || v2 == v || v3 == v);
	}

	/*
	bool same_plane(Face t) {

		Vector3Df n = getNormal();
		return
			fabs(dot(t.v1 - v1, n)) < 0.0001 &&
			fabs(dot(t.v2 - v1, n)) < 0.0001 &&
			fabs(dot(t.v3 - v1, n)) < 0.0001;
	}

	bool same_side(Face t) {

		Vector3Df n = getNormal();
		float f1 = dot(t.v1 - v1, n)
			, f2 = dot(t.v2 - v1, n)
			, f3 = dot(t.v3 - v1, n);
		return (f1 < SMALL_NUMBER && f2 < SMALL_NUMBER && f3 < SMALL_NUMBER)
			|| (f1 > -SMALL_NUMBER && f2 > -SMALL_NUMBER && f3 > -SMALL_NUMBER);
	}

	bool same_plane(Edge e) {
		Vector3Df n = getNormal();
		return
			fabs(dot(*e.v1 - v1, n)) < 0.0001 &&
			fabs(dot(*e.v2 - v1, n)) < 0.0001;
	}


	bool same_side(Vector3Df _v1, Vector3Df _v2) {
		Vector3Df n = getNormal();
		if (has(_v1) || has(_v2))return true;
		float r1 = dot(n, _v1 - v1),
			r2 = dot(n, _v2 - v1);
		return (r1 > -0.001 && r2 > -0.001) || (r1 < 0.001 && r2 < 0.001);
	}
	
	
	vector<Edge> getEdges() {
		vector<Edge> e;
		e.push_back(Edge{ &v1, &v2 });
		e.push_back(Edge{ &v2, &v3 });
		e.push_back(Edge{ &v3, &v1 });
		return e;
	}

	bool inside(Vector3Df v) {
		Vector3Df n = getNormal();
		vector<Edge> e = getEdges();
		int i1 = e[0].side_with_erro(n, v),
			i2 = e[1].side_with_erro(n, v),
			i3 = e[2].side_with_erro(n, v);
		return  i1 == i2 && i2 == i3;
	}
	bool colide(Edge e) {
		vector<Edge> es = getEdges();
		Vector3Df n = getNormal();
		
		float t = -dot(n, *e.v1 - v1) / (dot(*e.v2 - *e.v1, n));

		if (t < 0 || t > 1) return false;
		Vector3Df p = *e.v1 + ((*e.v2 - *e.v1)*t);
		return inside(p);
	}
	bool colide(Face f) {
		
		vector<Edge> arestas = f.getEdges();

		for (Edge &a : arestas) {
			if (colide(a)) {
				return true;
			}
		}
		
		return false;
	}*/
};


inline bool PonitIsOutFace(const Triangle &f, const Vector3Df &p)
{
	return dot(f.getNormal(), (p - f.getCenter()).to_unitary()) < -QUASE_ZERO;
}

inline bool PonitIsOutFace(const Face &f, const Vector3Df &p)
{
	return dot(f.getNormal(), (p - f.getCenter()).to_unitary()) < -QUASE_ZERO;
}


struct Tetra
{
	union {
		struct { Vector3Df v1, v2, v3, v4; };
		Vector3Df _v[4];
	};
	Vector3Df v_original[4];
	vector<Face> faces;
	Vector3Df cor;
	float raio;

	Vector3Df getCenter() {
		return (v1 + v2 + v3 + v4) / 4.0f;
	}

	Tetra(Vector3Df *p1, Vector3Df *p2, Vector3Df *p3, Vector3Df *p4) {
		v1 = *p1;
		v2 = *p2;
		v3 = *p3;
		v4 = *p4;
		v_original[0] = v1;
		v_original[1] = v2;
		v_original[2] = v3;
		v_original[3] = v4;

		UpdateFaces();

		float div = 1.0f / 1000.0f;
		cor = Vector3Df{ (rand() % 1000)*div, (rand() % 1000)*div, (rand() % 1000)*div };

		
		raio = 0;
		Vector3Df c = getCenter();
		for (int i = 0; i < 4; i++) {
			float ax = (c - _v[i]).length();
			if (ax > raio) {
				raio = ax;
			}
		}

	}

	void UpdateFaces() {
		Vector3Df center = getCenter();

		faces.clear();

		Face f;

		//face 1
		f = Face{ v1, v2, v3 };
		if (dot(f.getNormal(), (v1 - center).to_unitary()) < QUASE_ZERO) {
			faces.push_back(f);
		}
		else { faces.push_back(Face{ v1, v3, v2 }); }

		//face 2
		f = Face{ v2, v3, v4 };
		if (dot(f.getNormal(), (v2 - center).to_unitary()) < QUASE_ZERO) {
			faces.push_back(f);
		}
		else { faces.push_back(Face{ v2, v4, v3 }); }

		//face 3
		f = Face{ v1, v2, v4 };
		if (dot(f.getNormal(), (v1 - center).to_unitary()) < QUASE_ZERO) {
			faces.push_back(f);
		}
		else { faces.push_back(Face{ v1, v4, v2 }); }

		//face 4
		f = Face{ v1, v4, v3 };
		if (dot(f.getNormal(), (v1 - center).to_unitary()) < QUASE_ZERO) {
			faces.push_back(f);
		}
		else { faces.push_back(Face{ v1, v3, v4 }); }


	}
	
	void reduce(float size_triang) {

		Vector3Df c = getCenter();
		for (int i = 0; i < 4; i++) {
			_v[i] = v_original[i] + (c - v_original[i])*size_triang;
		}

		UpdateFaces();
	}

	/*
	bool Inside(const Vector3Df *v) {
		bool is_in = true;
		for (Face& t : faces) {
			is_in = is_in && !PonitIsOutFace(t, *v);
		}
		return is_in;
	}
	
	bool Colide(Tetra t) {
		if ((t.getCenter() - getCenter()).length() > raio + t.raio) {
			return false;
		}

		for (Face &f : faces) {
			for (Face &f2 : t.faces) {
				if (f2.same_side(f) || f.same_side(f2)) {
					//continue;
					//return false;
				}else{
					if (f.colide(f2) || f2.colide(f)) {
						return true;
					}
				}
			}
		}
		return false;
	}
	*/


};

/*
struct Tetra2
{
	union {
		struct { Vector3Df *v1, *v2, *v3, *v4; };
		Vector3Df *_v[4];
	};

	Vector3Df getCenter() const {
		return (*v1 + *v2 + *v3 + *v4) / 4.0f;
	}
	inline bool operator==(const Tetra& t) {
		return this->has(t.v1) 
			&& this->has(t.v2) 
			&& this->has(t.v3) 
			&& this->has(t.v4);
	}

	inline bool has(Vector3Df *v) {
		return (v1 == v || v2 == v || v3 == v || v3 == v);
	}
	bool has(Vector3Df v) {
		return (*v1 == v || *v2 == v || *v3 == v || *v4 == v);
	}

	vector<Triangle> getFaces() {
		vector<Triangle> faces;
		faces.push_back(Triangle{ v1,v2,v3 });
		faces.push_back(Triangle{ v2,v1,v4 });
		faces.push_back(Triangle{ v3,v2,v4 });
		faces.push_back(Triangle{ v1,v3,v4 });
		return faces;
	}

	bool Inside(Vector3Df &v) {
		if (has(v))return false;
		float i0 = dot(cross((*v2 - *v1), (*v3 - *v1)), (v - *v1));
		float i1 = dot(cross((*v1 - *v2), (*v4 - *v2)), (v - *v2));
		float i2 = dot(cross((*v2 - *v3), (*v4 - *v3)), (v - *v3));
		float i3 = dot(cross((*v3 - *v1), (*v4 - *v1)), (v - *v1));
		return (i0 > -0.00001 && i1 > -0.00001 && i2 > -0.00001 && i3 > -0.00001);
	}
	
	bool Colide(Triangle t) {
		vector<Triangle> faces1 = getFaces();
		//vector<Triangle> faces2 = t.getFaces();
		for (Triangle t1 : faces1) {
			if (!(t1 == t) && t1.same_plane(t))return true;
		}


		bool b1 = Inside(*t.v1),
			b2 = Inside(*t.v2),
			b3 = Inside(*t.v3);
		return b1 != b2 || b2 != b3 || b3!=b1;
	}

	bool Colide(Tetra &t) {
		//bool b1 = Inside(*t.v1),
		//	b2 = Inside(*t.v2),
		//	b3 = Inside(*t.v3),
		//	b4 = Inside(*t.v4);
		//return !(b1 == b2 && b2 == b3 && b3 == b4);
		
		return t.Colide(Triangle{ v1,v2,v3 })
			|| t.Colide(Triangle{ v1,v2,v4 })
			|| t.Colide(Triangle{ v2,v3,v4 })
			|| t.Colide(Triangle{ v3,v1,v4 });
		
		//vector<Triangle> faces1 = getFaces();
		//vector<Triangle> faces2 = t.getFaces();
		//for (Triangle t1 : faces1) {
		//	for (Triangle t2 : faces2) {
		//		//if (t1 == t2)
		//		if (!(t1 == t2) && t1.Colide(t2) && t2.Colide(t1))return true;
		//	}
		//}
		//return false;
	}
};
*/


struct ObjectTetra
{
	vector<Tetra> tetras;
	Vector3Df cor;

	void add_tetra(const Tetra &t) { 
		tetras.push_back(t);
	}
	
	/*
	void insert_point(Vector3Df *v) {
		int tetra_posi = 0;
		for (Tetra t : tetras) {
			if (t.Inside(v)) {
				bool same_plane = false;
				for (Triangle f : t.faces) {
					if (fabs(dot(f.getNormal().to_unitary(), *v - *f.v1)) < SMALL_NUMBER) {
						same_plane = true;
						return;
					}
				}
				if (same_plane)break;

				//achei o tetraedro pra ser dividido
				for (Triangle f : t.faces) {
					add_tetra(Tetra(f.v1, f.v2, f.v3, v));
				}
				tetras[tetra_posi] = tetras.back();
				tetras.pop_back();
				return;
			}
			tetra_posi++;
		}
	}
	
	void Transform(const Matrix4 &m) {}
	*/
};
