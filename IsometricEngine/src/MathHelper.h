#pragma once
#undef min
#undef max
#include <algorithm>

static inline int Clamp(int value, int min, int max)
{
	return value < min ? min : (value >= max ? max : value);
}

static inline int ClampPositive(int value)
{
	return value < 0 ? 0 : value;
}

static inline int DistanceSquared(int x1, int y1, int x2, int y2)
{
	return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

static inline int AbsoluteValue(int x)
{
	return x >= 0 ? x : -x;
}

struct Point
{
	int x;
	int y;

	Point(int x = 0, int y = 0) : x(x), y(y) {}
	Point(const Point& arg) : Point(arg.x, arg.y) {}

	bool zero() const
	{
		return this->x == 0 && this->y == 0;
	}

	Point& operator=(const Point& rhs)
	{
		this->x = rhs.x;
		this->y = rhs.y;
		return *this;
	}

	Point operator+(const Point& rhs) const
	{
		return Point(this->x + rhs.x, this->y + rhs.y);
	}

	Point operator-(const Point& rhs) const
	{
		return Point(this->x - rhs.x, this->y - rhs.y);
	}

	void operator+=(const Point& rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
	}

	void operator-=(const Point& rhs)
	{
		this->x -= rhs.x;
		this->y -= rhs.y;
	}

	Point operator*(const int rhs) const
	{
		return Point(this->x * rhs, this->y * rhs);
	}

	Point operator/(const int rhs) const
	{
		return Point(this->x / rhs, this->y / rhs);
	}

	bool operator==(const Point& rhs) const
	{
		return (this->x == rhs.x && this->y == rhs.y);
	}
};

struct Rect
{
	int left;
	int top;
	int right;
	int bot;

	Rect() {};
	Rect(const int left, const int top, const int right, const int bot) : left(left), top(top), right(right), bot(bot) {}
	Rect(const Point topLeft, const Point botRight) : left(topLeft.x), top(topLeft.y), right(botRight.x), bot(botRight.y) {}

	static Rect FromPoints(const Point a, const Point b) { return Rect(std::min(a.x, b.x), std::min(a.y, b.y), std::max(a.x, b.x), std::max(a.y, b.y)); };
	static Rect Bounding(const Point a, const Point b) { return Rect(std::min(a.x, b.x), std::min(a.y, b.y), std::max(a.x, b.x) + 1, std::max(a.y, b.y) + 1); }

	int Width() const { return right - left; };
	int Height() const { return bot - top; };
	int Area() const { return Width() * Height(); }

	Point TopLeft() const { return Point(left, top); };
	Point TopRight() const { return Point(right, top); };
	Point BotLeft() const { return Point(left, bot); };
	Point BotRight() const { return Point(right, bot); };

	bool PointInside(const Point point) { return point.x >= left && point.x < right && point.y >= top && point.y < bot; };

	void Clamp(int left, int top, int right, int bot) { this->left = ::Clamp(this->left, left, right); this->top = ::Clamp(this->top, top, bot); this->right = ::Clamp(this->right, left, right); this->bot = ::Clamp(this->bot, top, bot); };
	void Clamp(const Point topLeft, const Point botRight) { Clamp(topLeft.x, topLeft.y, botRight.x, botRight.y); };
};


static inline int AboveLine(const Point& lineA, const Point& lineB, const Point& point)
{
	return ((lineB.x - lineA.x)*(point.y - lineA.y) - (lineB.y - lineA.y)*(point.x - lineA.x));
}