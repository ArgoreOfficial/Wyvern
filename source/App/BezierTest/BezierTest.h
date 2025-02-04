#pragma once
#include <wv/Scene/Rigidbody.h>
#include "wv/Math/BezierCurve.h"


class BezierTest
{
public:
	BezierTest();
	~BezierTest();

	void Update(double deltaTime);

	void Create();
	wv::cRigidbody* rb;
	wv::BezierCurve<float> bezier = {};
	float t = 0.0f;
	float speed = 0.5f;
};