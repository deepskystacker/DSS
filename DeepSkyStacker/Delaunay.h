/********************************************************************************
	Copyright (C) 2004-2005 Sjaak Priester	

	This is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this application; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
********************************************************************************/

// Delaunay
// Class to perform Delaunay triangulation on a set of vertices
//
// Version 1.2 (C) 2005, Sjaak Priester, Amsterdam.
// - Removed stupid bug in SetY; function wasn't used, so no consequences. Thanks to squat.
//
// Version 1.1 (C) 2005, Sjaak Priester, Amsterdam.
// - Removed bug which gave incorrect results for co-circular vertices.
//
// Version 1.0 (C) 2004, Sjaak Priester, Amsterdam.
// mailto:sjaak@sjaakpriester.nl

#pragma once

#include <set>
#include <algorithm>
#include <math.h>

using namespace std;

#include <gdiplus.h>
using namespace Gdiplus;

/*
#ifndef _GDIPLUS_H

// I designed this with GDI+ in mind. However, this particular code doesn't
// use GDI+ at all, only some of it's variable types.
// These definitions are substitutes for those of GDI+. 
typedef float REAL;
struct PointF
{
	PointF() : X(0), Y(0)	{}
	PointF(const PointF& p) : X(p.X), Y(p.Y)	{}
	PointF(REAL x, REAL y) : X(x), Y(y)	{}
	PointF operator+(const PointF& p) const	{ return PointF(X + p.X, Y + p.Y); }
	PointF operator-(const PointF& p) const	{ return PointF(X - p.X, Y - p.Y); }
	REAL X;
	REAL Y;
};

const REAL REAL_EPSILON = 1.192092896e-07F;	// = 2^-23; I've no idea why this is a good value, but GDI+ has it.

#endif // _GDIPLUS_H
*/
///////////////////
// vertex

class vertex
{
public:
	vertex()					: m_Pnt(0.0F, 0.0F)			{}
	vertex(const vertex& v)		: m_Pnt(v.m_Pnt)			{}
	vertex(const PointF& pnt)	: m_Pnt(pnt)				{}
	vertex(REAL x, REAL y)		: m_Pnt(x, y)				{}
	vertex(int x, int y)		: m_Pnt((REAL) x, (REAL) y)	{}

	bool operator<(const vertex& v) const
	{
		if (m_Pnt.X == v.m_Pnt.X) return m_Pnt.Y < v.m_Pnt.Y;
		return m_Pnt.X < v.m_Pnt.X;
	}

	bool operator==(const vertex& v) const
	{
		return m_Pnt.X == v.m_Pnt.X && m_Pnt.Y == v.m_Pnt.Y;
	}
	
	REAL GetX()	const	{ return m_Pnt.X; }
	REAL GetY() const	{ return m_Pnt.Y; }

	void SetX(REAL x)		{ m_Pnt.X = x; }
	void SetY(REAL y)		{ m_Pnt.Y = y; }

	const PointF& GetPoint() const		{ return m_Pnt; }
protected:
	PointF	m_Pnt;
};

typedef set<vertex> vertexSet;
typedef set<vertex>::iterator vIterator;
typedef set<vertex>::const_iterator cvIterator;

///////////////////
// triangle

class triangle
{
public:
	triangle(const triangle& tri)
		: m_Center(tri.m_Center)
		, m_R(tri.m_R)
		, m_R2(tri.m_R2)
	{
		for (int i = 0; i < 3; i++) m_Vertices[i] = tri.m_Vertices[i];
	}
	triangle(const vertex * p0, const vertex * p1, const vertex * p2)
	{
		m_Vertices[0] = p0;
		m_Vertices[1] = p1;
		m_Vertices[2] = p2;
		SetCircumCircle();
	}
	triangle(const vertex * pV)
	{
		for (int i = 0; i < 3; i++) m_Vertices[i] = pV++;
		SetCircumCircle();
	}

	triangle & operator = (const triangle& tri)
	{
		m_Center	= tri.m_Center;
		m_R			= tri.m_R;
		m_R2		= tri.m_R2;
		for (int i = 0; i < 3; i++) 
			m_Vertices[i] = tri.m_Vertices[i];

		return (*this);
	};

	bool operator<(const triangle& tri) const
	{
		if (m_Center.X == tri.m_Center.X) return m_Center.Y < tri.m_Center.Y;
		return m_Center.X < tri.m_Center.X;
	}

	const vertex * GetVertex(int i) const
	{
		ASSERT(i >= 0);
		ASSERT(i < 3);
		return m_Vertices[i];
	}

	bool IsLeftOf(cvIterator itVertex) const
	{
		// returns true if * itVertex is to the right of the triangle's circumcircle
		return itVertex->GetPoint().X > (m_Center.X + m_R);
	}

	bool CCEncompasses(cvIterator itVertex) const
	{
		// Returns true if * itVertex is in the triangle's circumcircle.
		// A vertex exactly on the circle is also considered to be in the circle.

		// These next few lines look like optimisation, however in practice they are not.
		// They even seem to slow down the algorithm somewhat.
		// Therefore, I've commented them out.

		// First check boundary box.
//		REAL x = itVertex->GetPoint().X;
//				
//		if (x > (m_Center.X + m_R)) return false;
//		if (x < (m_Center.X - m_R)) return false;
//
//		REAL y = itVertex->GetPoint().Y;
//				
//		if (y > (m_Center.Y + m_R)) return false;
//		if (y < (m_Center.Y - m_R)) return false;

		PointF dist = itVertex->GetPoint() - m_Center;		// the distance between v and the circle center
		REAL dist2 = dist.X * dist.X + dist.Y * dist.Y;		// squared
		return dist2 <= m_R2;								// compare with squared radius
	}
protected:
	const vertex * m_Vertices[3];	// the three triangle vertices
	PointF m_Center;				// center of circumcircle
	REAL m_R;			// radius of circumcircle
	REAL m_R2;			// radius of circumcircle, squared

	void SetCircumCircle();
};

// Changed in verion 1.1: collect triangles in a multiset.
// In version 1.0, I used a set, preventing the creation of multiple
// triangles with identical center points. Therefore, more than three
// co-circular vertices yielded incorrect results. Thanks to Roger Labbe.
typedef multiset<triangle> triangleSet;
typedef multiset<triangle>::iterator tIterator;
typedef multiset<triangle>::const_iterator ctIterator;

///////////////////
// edge

class edge
{
public:
	edge(const edge& e)	: m_pV0(e.m_pV0), m_pV1(e.m_pV1)	{}
	edge(const vertex * pV0, const vertex * pV1)
		: m_pV0(pV0), m_pV1(pV1)
	{
	}

	bool operator<(const edge& e) const
	{
		if (m_pV0 == e.m_pV0) return * m_pV1 < * e.m_pV1;
		return * m_pV0 < * e.m_pV0;
	}

	const vertex * m_pV0;
	const vertex * m_pV1;
};

typedef set<edge> edgeSet;
typedef set<edge>::iterator edgeIterator;
typedef set<edge>::const_iterator cedgeIterator;

///////////////////
// Delaunay

class Delaunay
{
public:
	// Calculate the Delaunay triangulation for the given set of vertices.
	void Triangulate(const vertexSet& vertices, triangleSet& output);

	// Put the edges of the triangles in an edgeSet, eliminating double edges.
	// This comes in useful for drawing the triangulation.
	void TrianglesToEdges(const triangleSet& triangles, edgeSet& edges);
protected:
	void HandleEdge(const vertex * p0, const vertex * p1, edgeSet& edges);
};
