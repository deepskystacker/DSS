#pragma once

#include <type_traits>
#include "DssRect.h"

template <class T>
bool pointInRect(const T& point, const DSSRect& rect)
{
	// If it is a point with real-valued coordinates (i.e. a point in a coordinate system), then rect's borders are inclusive.
	if constexpr (std::is_floating_point_v<decltype(std::declval<T>().x())> && std::is_floating_point_v<decltype(std::declval<T>().y())>)
		return point.x() >= rect.left() && point.x() <= rect.right() && point.y() >= rect.top() && point.y() <= rect.bottom();
	else
		return point.x() >= rect.left() && point.x() < rect.right() && point.y() >= rect.top() && point.y() < rect.bottom();
}

template <class T>
bool rectContainsPoint(const DSSRect& rect, const T& point)
{
	if constexpr (std::is_floating_point_v<decltype(std::declval<T>().x())> && std::is_floating_point_v<decltype(std::declval<T>().y())>)
		return rect.left() <= point.x() && rect.right() >= point.x() && rect.top() <= point.y() && rect.bottom() >= point.y();
	else
		return rect.left() <= point.x() && rect.right() > point.x() && rect.top() <= point.y() && rect.bottom() > point.y();
}
