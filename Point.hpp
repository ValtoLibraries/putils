#pragma once

#include <initializer_list>
#include <cmath>
#include "hash.hpp"
#include "Serializable.hpp"

namespace putils
{
    template<typename Precision, std::size_t Dimensions = 2>
    struct Point : public Serializable<Point<Precision, Dimensions>>
    {
        Precision x;
        Precision y;

        Point(Precision x = 0, Precision y = 0)
                : Serializable<Point<Precision, Dimensions>>(
                std::make_pair("x", &Point::x),
                std::make_pair("y", &Point::y)
        ), x(x), y(y)
        {}

        Point(const Point &) noexcept = default;
        Point &operator=(const Point &) noexcept = default;

        Point(Point &&) noexcept = default;
        Point &operator=(Point &&) noexcept = default;

        template<typename P>
        bool operator==(const Point<P> &rhs) const noexcept { return x == rhs.x && y == rhs.y; }

        template<typename P>
        bool operator!=(const Point<P> &rhs) const noexcept { return !(*this == rhs); }

        template<typename P>
        Point operator+(const Point<P> &rhs) const noexcept { return { x + rhs.x, y + rhs.y }; }

        template<typename P>
        Point &operator+=(const Point<P> &rhs) noexcept { x += rhs.x; y += rhs.y; return *this; }

        template<typename P>
        Point operator-(const Point<P> &rhs) const noexcept { return { x - rhs.x, y - rhs.y }; }

        template<typename P>
        Point &operator-=(const Point<P> &rhs) noexcept { x -= rhs.x; y -= rhs.y; return *this; }

        template<typename P>
        Precision distanceTo(const Point<P> &rhs) const noexcept
        {
            return std::sqrt(
                    std::pow(x - rhs.x, 2) +
                    std::pow(y - rhs.y, 2)
            );
        }
    };

    template<typename Precision, std::size_t Dimensions = 2>
    struct Rect : public Serializable<Rect<Precision, Dimensions>>
    {
        Point<Precision> topLeft;
        Point<Precision> size;

        Rect(Point<Precision, Dimensions> topLeft = {}, Point<Precision, Dimensions> size = {})
                : Serializable<Rect<Precision, Dimensions>>(
                std::make_pair("topLeft", &Rect::topLeft),
                std::make_pair("size", &Rect::size)
        ), topLeft(topLeft), size(size)
        {}

        template<typename P>
        bool operator==(const Rect<P> &rhs) const { return topLeft == rhs.topLeft && size == rhs.size; }

        template<typename P>
        bool operator!=(const Rect<P> &rhs) const { return !(*this == rhs); }

        bool intersect(const Rect &other, bool inclusiveBorders = false) const
        {
            if (inclusiveBorders)
                return !(topLeft.x > other.topLeft.x + other.size.x ||
                         topLeft.x + size.x < other.topLeft.x ||
                         topLeft.y > other.topLeft.y + other.size.y ||
                         topLeft.y + size.y < other.topLeft.y
                );

            return !(topLeft.x >= other.topLeft.x + other.size.x ||
                     topLeft.x + size.x < other.topLeft.x ||
                     topLeft.y >= other.topLeft.y + other.size.y ||
                     topLeft.y + size.y < other.topLeft.y
            );
        }

        bool contains(const Point<Precision, 2> &point) const
        {
            return (topLeft.x <= point.x &&
                    topLeft.x + size.x > point.x &&
                    topLeft.y <= point.y &&
                    topLeft.y + size.y > point.y
            );
        }
    };

    template<typename Precision>
    struct Point<Precision, 3> : public Serializable<Point<Precision, 3>>
    {
        Precision x;
        Precision y;
        Precision z;

        Point(Precision x = 0, Precision y = 0, Precision z = 0)
                : Serializable<Point<Precision, 3>>(
                std::make_pair("x", &Point::x),
                std::make_pair("y", &Point::y),
                std::make_pair("z", &Point::z)
        ),
                x(x), y(y), z(z)
        {}

        Point(const Point &) noexcept = default;
        Point &operator=(const Point &) noexcept = default;

        Point(Point &&) noexcept = default;
        Point &operator=(Point &&) noexcept = default;

        template<typename P>
        bool operator==(const Point<P, 3> &rhs) const noexcept
        { return x == rhs.x && y == rhs.y && z == rhs.z; }

        template<typename P>
        bool operator!=(const Point<P, 3> &rhs) const noexcept { return !(*this == rhs); }

        Point operator-() const noexcept { return { -x, -y, -z }; }

        template<typename P>
        Point operator+(const Point<P, 3> &rhs) const noexcept { return { x + rhs.x, y + rhs.y, z + rhs.z }; }

        template<typename P>
        Point &operator+=(const Point<P, 3> &rhs) noexcept { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }

        template<typename P>
        Point operator-(const Point<P, 3> &rhs) const noexcept { return { x - rhs.x, y - rhs.y, z - rhs.z }; }

        template<typename P>
        Point &operator-=(const Point<P, 3> &rhs) noexcept { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
        template<typename P>
        Precision distanceTo(const Point<P, 3> &rhs) const noexcept
        {
            return std::sqrt(
                    std::pow(x - rhs.x, 2) +
                    std::pow(y - rhs.y, 2) +
                    std::pow(z - rhs.z, 2)
            );
        }
    };

    template<typename Precision>
    struct Rect<Precision, 3> : public Serializable<Rect<Precision, 3>>
    {
        Point<Precision, 3> topLeft;
        Point<Precision, 3> size;

        Rect(Point<Precision, 3> topLeft = {}, Point<Precision, 3> size = {})
            : Serializable<Rect<Precision, 3>>(
                std::make_pair("topLeft", &Rect::topLeft),
                std::make_pair("size", &Rect::size)
        ), topLeft(topLeft), size(size)
        {}

        template<typename P>
        bool operator==(const Rect<P> &rhs) { return topLeft == rhs.topLeft && size == rhs.size; }

        template<typename P>
        bool operator!=(const Rect<P> &rhs) { return !(*this == rhs); }

        bool intersect(const Rect &other) const
        {
            return !(topLeft.x >= other.topLeft.x + other.size.x ||
                     topLeft.x + size.x <= other.topLeft.x ||
                     topLeft.y >= other.topLeft.y + other.size.y ||
                     topLeft.y + size.y <= other.topLeft.y ||
                     topLeft.z >= other.topLeft.z + other.size.z ||
                     topLeft.z + size.z <= other.topLeft.z
            );
        }

        bool contains(const Point<Precision, 3> &point) const
        {
            return (topLeft.x <= point.x &&
                    topLeft.x + size.x > point.x &&
                    topLeft.y <= point.y &&
                    topLeft.y + size.y > point.y &&
                    topLeft.z <= point.z &&
                    topLeft.z + size.z > point.z
            );
        }
    };
}

namespace std
{
    template<typename Precision>
    struct hash<putils::Point<Precision>>
    {
        size_t operator()(const putils::Point<Precision> &coord) const noexcept
        {
            return putils::PairHash().operator()(std::make_pair(coord.x, coord.y));
        }
    };
}

