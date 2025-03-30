#include "delta_time_adjustor.hpp"

#include <stdexcept>
#include <iostream>
#include <algorithm>

DeltaTimeAdjustor::DeltaTimeAdjustor(double duration, float tau, std::vector<glm::vec2> points)
    : points{points}, polynomials{}, duration{duration}, accumulated_time{0.0}, tau{tau} {
    if (points.size() < 4) {
        return;
        // TODO: or if you have overlapping adjacent points
        throw std::runtime_error("DeltaTimeAdjustor needs at least 4 control points!");
    }

    for (int i = 0; i < points.size() - 3; i++) {
        // for derivation, see https://www.cs.cmu.edu/~fp/courses/graphics/asst5/catmullRom.pdf
        Polynomial polynomial;
        polynomials.push_back(polynomial);
        polynomials[i].coef_constant = points[i + 1];
        polynomials[i].coef_linear = -tau * points[i] + tau * points[i + 2];
        polynomials[i].coef_quadratic = 2.0f * tau * points[i] + (tau - 3.0f) * points[i + 1] +
                                        (3.0f - 2.0f * tau) * points[i + 2] - tau * points[i + 3];
        polynomials[i].coef_cubic =
            -tau * points[i] + (2.0f - tau) * points[i + 1] + (tau - 2.0f) * points[i + 2] + tau * points[i + 3];

        polynomials[i].x_length =
            points[i + 2].x - points[i + 1].x; // assumes points are monotonic and sorted by their x-coordinate
    }
}

glm::vec2 DeltaTimeAdjustor::interpolate(double delta_time) {
    accumulated_time = std::clamp(accumulated_time + delta_time, 0.0, duration);

    float x_length = 0.0f;
    int i = 0;
    while (x_length + polynomials[i].x_length < accumulated_time) {
        x_length += polynomials[i].x_length;
        i++;
    }

    float t = 0.0f;
    if (delta_time >= 0.0 && i == cache.i) {
        // use cached variables to numerically integrate for arc length, if possible
        t = cache.t;
        x_length = cache.x_length;
    }
    while (x_length < accumulated_time) {
        x_length = (polynomials[i].coef_constant + polynomials[i].coef_linear * t +
                    polynomials[i].coef_quadratic * t * t + polynomials[i].coef_cubic * t * t * t)
                       .x;
        t += dt;
    }

    cache.i = i;
    cache.t = t;
    cache.x_length = x_length;

    return polynomials[i].coef_constant + polynomials[i].coef_linear * t + polynomials[i].coef_quadratic * t * t +
           polynomials[i].coef_cubic * t * t * t;
}

void DeltaTimeAdjustor::reset() {
    accumulated_time = 0.0;
    cache.i = -1;
}
