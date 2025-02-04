#pragma once
#include <wv/Math/Vector3.h>
#include <vector>
namespace wv
{

    template<typename T>
    class BezierCurve
    {
    public:

        BezierCurve(const std::vector<Vector3<T>>& controlPoints) : controlPoints(controlPoints) {}
        BezierCurve(std::initializer_list<Vector3<T>> points) : controlPoints(points) {}

        Vector3<T> evaluate(float t) const {
            std::vector<Vector3<T>> temp = controlPoints;
            int n = temp.size();

            for (int k = 1; k < n; ++k) {
                for (int i = 0; i < n - k; ++i) {
                    temp[i] = temp[i] * (1 - t) + temp[i + 1] * t;
                }
            }
            return temp[0];
        }


        std::vector<Vector3<T>> generateCurve(int steps) const {
            std::vector<Vector3<T>> curvePoints;
            for (int i = 0; i <= steps; ++i) {
                float t = static_cast<float>(i) / steps;
                curvePoints.push_back(evaluate(t));
            }
            return curvePoints;
        }

    private:
        std::vector<Vector3<T>> controlPoints;
    };
}
