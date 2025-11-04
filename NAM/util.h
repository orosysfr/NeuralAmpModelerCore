#pragma once

// Utilities

#include <string>
#include <Eigen/Dense> // Eigen::MatrixXf

namespace util
{
std::string lowercase(const std::string& s);

template<typename T>
class Smoother {
public:
    explicit Smoother(T v = 0) noexcept : cur(v), tgt(v) {}

    void reset(double fs, double sec) noexcept { reset((int)std::floor(std::max(0.0, fs * sec))); }
    void reset(int samples) noexcept { ramp = std::max(0, samples); setCurrentAndTargetValue(tgt); }

    void setTargetValue(T v) noexcept {
        if (std::abs(v - tgt) <= std::numeric_limits<T>::epsilon()) return;
        if (ramp <= 0) { setCurrentAndTargetValue(v); return; }
        tgt  = v;
        left = ramp;
        step = (tgt - cur) / (T)left;
    }

    T getNextValue() noexcept {
        if (left <= 0) return cur = tgt;
        cur += step;
        if (--left == 0) cur = tgt;
        return cur;
    }

    T skip(int n) noexcept {
        if (n <= 0 || left <= 0) return cur = tgt;
        if (n >= left) { setCurrentAndTargetValue(tgt); return cur; }
        cur += step * (T)n;
        left -= n;
        return cur;
    }

    bool isSmoothing() const noexcept { return left > 0; }
    T getCurrentValue() const noexcept { return cur; }
    T getTargetValue() const noexcept { return tgt; }

    void setCurrentAndTargetValue(T v) noexcept {
        cur = tgt = v; left = 0; step = T{};
    }

private:
    T cur = 0, tgt = 0, step = 0;
    int left = 0, ramp = 0;
};

}; // namespace util
