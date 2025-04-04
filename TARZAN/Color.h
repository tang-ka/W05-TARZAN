#pragma once
#include <algorithm> // std::clamp
#include <cmath>     // std::sqrt

struct FLinearColor
{
    float R, G, B, A;

    FLinearColor() : R(0), G(0), B(0), A(1) {}
    FLinearColor(float InR, float InG, float InB, float InA = 1.0f)
        : R(InR), G(InG), B(InB), A(InA) {
    }

    static FLinearColor White() { return FLinearColor(1, 1, 1, 1); }
    static FLinearColor Black() { return FLinearColor(0, 0, 0, 1); }
    static FLinearColor Red() { return FLinearColor(1, 0, 0, 1); }
    static FLinearColor Green() { return FLinearColor(0, 1, 0, 1); }
    static FLinearColor Blue() { return FLinearColor(0, 0, 1, 1); }

    FLinearColor operator+(const FLinearColor& Other) const
    {
        return FLinearColor(R + Other.R, G + Other.G, B + Other.B, A + Other.A);
    }

    FLinearColor operator-(const FLinearColor& Other) const
    {
        return FLinearColor(R - Other.R, G - Other.G, B - Other.B, A - Other.A);
    }

    FLinearColor operator*(float Scalar) const
    {
        return FLinearColor(R * Scalar, G * Scalar, B * Scalar, A * Scalar);
    }

    FLinearColor operator/(float Scalar) const
    {
        float InvScalar = 1.0f / Scalar;
        return FLinearColor(R * InvScalar, G * InvScalar, B * InvScalar, A * InvScalar);
    }

    FLinearColor operator*(const FLinearColor& Other) const
    {
        return FLinearColor(R * Other.R, G * Other.G, B * Other.B, A * Other.A);
    }

    // 색상 보간
    static FLinearColor Lerp(const FLinearColor& A, const FLinearColor& B, float T)
    {
        return A + (B - A) * T;
    }

    // 밝기 (Luminance)
    float GetLuminance() const
    {
        return 0.2126f * R + 0.7152f * G + 0.0722f * B;
    }

    // Clamp (0~1 범위로)
    void Clamp()
    {
        R = std::clamp(R, 0.0f, 1.0f);
        G = std::clamp(G, 0.0f, 1.0f);
        B = std::clamp(B, 0.0f, 1.0f);
        A = std::clamp(A, 0.0f, 1.0f);
    }

    FLinearColor ToSRGB() const
    {
        auto GammaCorrect = [](float C) {
            return (C <= 0.0031308f) ? C * 12.92f : 1.055f * std::pow(C, 1.0f / 2.4f) - 0.055f;
            };
        return FLinearColor(GammaCorrect(R), GammaCorrect(G), GammaCorrect(B), A);
    }

    static FLinearColor FromSRGB(float R, float G, float B, float A = 1.0f)
    {
        auto Linearize = [](float C) {
            return (C <= 0.04045f) ? C / 12.92f : std::pow((C + 0.055f) / 1.055f, 2.4f);
            };
        return FLinearColor(Linearize(R), Linearize(G), Linearize(B), A);
    }
};
