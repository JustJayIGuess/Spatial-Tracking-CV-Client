#pragma once
#include <string>

namespace mathutils {
	struct Vector3 {
		float x;
		float y;
		float z;
		Vector3();
		Vector3(float _x, float _y, float _z);
		static Vector3 random();
		static Vector3 random(float min, float max);
		std::string to_string();
		void print();
		void normalize();
		float sqr_magnitude();
		float magnitude();
		void to_polar();
	};

	Vector3 operator+(Vector3 a, Vector3 b);
	Vector3 operator*(Vector3 a, float b);
	Vector3 operator*(float a, Vector3 b);
	Vector3 operator-(Vector3 a, Vector3 b);
	Vector3 operator/(Vector3 a, float b);
}
