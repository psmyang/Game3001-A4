#include "MathManager.h"

double MathManager::Distance(const double& x1, const double& x2, const double& y1, const double& y2)
{
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

double MathManager::Distance(const int& x1, const int& x2, const int& y1, const int& y2)
{
	return (sqrt(((double)x2 - (double)x1) * ((double)x2 - (double)x1) + ((double)y2 - (double)y1) * ((double)y2 - (double)y1)));
}

double MathManager::AngleBetweenPoints(const double dy, const double dx)
{
	return atan2(dy, dx); // In radians.
}

double MathManager::AngleBetweenPoints(const double dx1, const double dy1, const double dx2, const double dy2)
{
	return Rad2Deg(atan2((dy2 - dy1), (dx2 - dx1)));
}

void MathManager::SetDeltas(const double angle, double& dx, double& dy, double fx, double fy)
{
	dx = SetDeltaX(angle, fx);
	dy = SetDeltaY(angle, fy);
}

double MathManager::SetDeltaX(const double angle, double fx)
{
	return cos(angle) * fx;
}

double MathManager::SetDeltaY(const double angle, double fy)
{
	return sin(angle) * fy;
}

double MathManager::Deg2Rad(const double angle)
{
	return angle * (M_PI / 180.0);
}

double MathManager::Rad2Deg(const double angle)
{
	return angle * (180.0 / M_PI);
}

SDL_Rect MathManager::ConvertFRect2Rect(const SDL_FRect& r)
{
	SDL_Rect temp = { (int)r.x, (int)r.y, (int)r.w, (int)r.h };
	return temp;
}

double MathManager::Angle180(double a)
{ // Constrains an angle between -180 and 180.
	a = fmod(a + 180.0, 360.0);
	if (a < 0)
		a += 360.0;
	return a - 180.0;
}

double MathManager::Angle360(double a)
{ // Constrains an angle between 0 and 360.
	a = fmod(a, 360.0);
	if (a < 0)
		a += 360.0;
	return a;
}

double MathManager::LerpD(double a, double b, double factor)
{
	return ((1.0 - factor) * a) + (factor * b);
}

double MathManager::LerpRad(double a, double b, double factor)
{
	double result;
	double diff = b - a;
	if (diff < -M_PI)
	{
		b += M_PI * 2;
		result = LerpD(a, b, factor);
		if (result >= M_PI * 2)
			result -= M_PI * 2;
	}
	else if (diff > M_PI)
	{
		b -= M_PI * 2;
		result = LerpD(a, b, factor);
		if (result < 0.0)
			result += M_PI * 2;
	}
	else
		result = LerpD(a, b, factor);
	return result;
}

SDL_Point MathManager::HalfwayPoint(const SDL_Point& from, const SDL_Point& to)
{
	return { (int)(((double)from.x + (double)to.x) / 2.0), (int)(((double)from.y + (double)to.y) / 2.0) };
}
