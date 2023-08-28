#pragma once
#include <cmath>


//allows us to tell what function to use
enum class Activationfunctions {     Sigmoid = 0, Tanh,   LeakyReLU,   Elu,   Swish,   SoftMax,   Linear,   Quad,   Cubic };
static std::string functionToString[] = { "Sigmoid",   "Tanh", "LeakyReLU", "Elu", "Swish", "SoftMax", "Linear", "Quad", "Cubic" };

//holds the ranges if each function
static float ranges[][2] = { 
	{0, 1}, //sigmoid
	{-1, 1}, //tanh
	{0, 0}, //leakyReLU
	{0, 0}, //elu
	{0, 0}, //swish
	{0, 1}, //softMax
	{0, 0}, //Quadratic
	{0, 0}, //Cubic
};

//each function and its derivitive.
static float Sigmoid(float x) { return 1 / (1.f + std::exp(-x)); }
static float SigmoidDX(float x) { return Sigmoid(x) * (1 - Sigmoid(x)); }

static float Tanh(float x) { return 2 / (1 + std::exp(-2 * x)) - 1; }
static float TanhDX(float x) { return 1 - Tanh(x) * Tanh(x); }

static float LeakyReLU(float x) { return  __max(x, .01f * x); }
static float LeakyReLUDX(float x) { return (x >= 0) ? 1 : .01; }

static float Elu(float x) { return (x >= 0) ? x : (std::exp(x) - 1); }
static float EluDX(float x) { return (x >= 0) ? 1.f : (std::exp(x)); }

static float Swish(float x) { return x * Sigmoid(x); }
static float SwishDX(float x) { return Swish(x) + Sigmoid(x) * (1 - Swish(x)); }


#ifdef useSoftMax
//soft max needs to be applied to a vector, so it needs a template
template <typename T>
static T SoftMax(T x)
{
	float total = 0;
	for (int i = 0; i < x.length(); i++)
	{
		x[i] = std::exp(x[i]);
		total += x[i];
	}
	return x / total;
}
template <typename T>
static T SoftMaxDX(T x)
{
	return x * (T(1.f) - x);
}
#else
static float SoftMax(float x) { return 1 / x; }
static float SoftMaxDX(float x) { return 1 / x; }

#endif



static float Linear(float x) { return x; }
static float LinearDX(float x) { return 1; }

static float Quad(float x) { return x*x; }
static float QuadDX(float x) { return 2*x; }

static float Cubic(float x) { return x*x*x; }
static float CubicDX(float x) { return 3*x*x; }



static float activationFunc(float x, Activationfunctions func)
{
	switch (func)
	{
	case Activationfunctions::Sigmoid:
		return Sigmoid(x);
		break;
	case Activationfunctions::Tanh:
		return Tanh(x);
		break;
	case Activationfunctions::LeakyReLU:
		return LeakyReLU(x);
		break;
	case Activationfunctions::Elu:
		return Elu(x);
		break;
	case Activationfunctions::Swish:
		return Swish(x);
		break;
	case Activationfunctions::SoftMax:
		return SoftMax(x);
		break;
	case Activationfunctions::Linear:
		return Linear(x);
		break;
	case Activationfunctions::Quad:
		return Quad(x);
		break;
	case Activationfunctions::Cubic:
		return Cubic(x);
	default:
		break;
	}
}
static float activationFuncDX(float x, Activationfunctions func)
{
	switch (func)
	{
	case Activationfunctions::Sigmoid:
		return SigmoidDX(x);
		break;
	case Activationfunctions::Tanh:
		return TanhDX(x);
		break;
	case Activationfunctions::LeakyReLU:
		return LeakyReLUDX(x);
		break;
	case Activationfunctions::Elu:
		return EluDX(x);
		break;
	case Activationfunctions::Swish:
		return SwishDX(x);
		break;
	case Activationfunctions::SoftMax:
		return SoftMaxDX(x);
		break;
	case Activationfunctions::Linear:
		return LinearDX(x);
		break;
	case Activationfunctions::Quad:
		return QuadDX(x);
		break;
	case Activationfunctions::Cubic:
		return CubicDX(x);
	default:
		break;
	}
}