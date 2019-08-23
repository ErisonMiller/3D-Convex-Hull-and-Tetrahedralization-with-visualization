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
};


inline bool PonitIsOutFace(const Triangle &f, const Vector3Df &p)
{
	return dot(f.getNormal(), (p - f.getCenter()).to_unitary()) < SMALL_NUMBER;
}

inline bool PonitIsOutFace(const Face &f, const Vector3Df &p)
{
	return dot(f.getNormal(), (p - f.getCenter()).to_unitary()) < SMALL_NUMBER;
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
		if (dot(f.getNormal(), (v1 - center).to_unitary()) > SMALL_NUMBER) {
			faces.push_back(f);
		}
		else { faces.push_back(Face{ v1, v3, v2 }); }

		//face 2
		f = Face{ v2, v3, v4 };
		if (dot(f.getNormal(), (v2 - center).to_unitary()) > SMALL_NUMBER) {
			faces.push_back(f);
		}
		else { faces.push_back(Face{ v2, v4, v3 }); }

		//face 3
		f = Face{ v1, v2, v4 };
		if (dot(f.getNormal(), (v1 - center).to_unitary()) > SMALL_NUMBER) {
			faces.push_back(f);
		}
		else { faces.push_back(Face{ v1, v4, v2 }); }

		//face 4
		f = Face{ v1, v4, v3 };
		if (dot(f.getNormal(), (v1 - center).to_unitary()) > SMALL_NUMBER) {
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


};


struct ObjectTetra
{
	vector<Tetra> tetras;
	Vector3Df cor;

	void add_tetra(const Tetra &t) { 
		tetras.push_back(t);
	}
	
};
