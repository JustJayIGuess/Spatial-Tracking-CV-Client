#include "Vector3.h"
#include <iostream>
#include "math.h"

using namespace mathutils;
mathutils::Vector3::Vector3() {
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

mathutils::Vector3::Vector3(float _x, float _y, float _z) {
	x = _x;
	y = _y;
	z = _z;
}

// Vector with values from 0.0-1.0
mathutils::Vector3 mathutils::Vector3::random() {
	float x = (float)std::rand() / RAND_MAX;
	float y = (float)std::rand() / RAND_MAX;
	float z = (float)std::rand() / RAND_MAX;
	return Vector3(x, y, z);
}

float mathutils::Vector3::sqr_magnitude() {
	return x * x + y * y + z * z;
}

float mathutils::Vector3::magnitude() {
	return std::sqrt(Vector3::sqr_magnitude());
}

void mathutils::Vector3::normalize() {
	float mag = magnitude();
	x /= mag;
	y /= mag;
	z /= mag;

}

void mathutils::Vector3::to_polar() {
	float theta = std::atan2(y, x);
	float w = std::sqrt(x * x + y * y);
	float phi = std::atan2(z, w);

	x = magnitude();
	y = theta;
	z = phi;
}

std::string mathutils::Vector3::to_string() {
	return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")"; 
}

mathutils::Vector3 mathutils::Vector3::random(float min, float max) {
	return (max - min) * Vector3::random() + Vector3(min, min, min);
}

void mathutils::Vector3::print() {
	std::cout << this->to_string() << std::endl;
}

mathutils::Vector3 mathutils::operator+(mathutils::Vector3 a, mathutils::Vector3 b) {
        return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
}

mathutils::Vector3 mathutils::operator*(mathutils::Vector3 a, float b) {
        return Vector3(a.x * b, a.y * b, a.z * b);
}

mathutils::Vector3 mathutils::operator*(float a, mathutils::Vector3 b) {
        return Vector3(a * b.x, a * b.y, a * b.z);
}

mathutils::Vector3 mathutils::operator-(mathutils::Vector3 a, mathutils::Vector3 b) {
        return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
}

mathutils::Vector3 mathutils::operator/(mathutils::Vector3 a, float b) {
        return a * (1.0f / b);
}

