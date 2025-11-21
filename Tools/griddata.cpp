77/****************************************************************************
**
** Copyright (C) 2025 David C. Partridge
**
** BSD License Usage
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of DeepSkyStacker nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
**
****************************************************************************/
// griddata.cpp : Implementation file
//
// Based upon plgridd.c which is part of PLPlot:
// Copyright (C) 2004  Joao Cardoso
// Copyright (C) 2004-2015 Alan W. Irwin
//
#include "pch.h"
#include <vector>
#include <zexcept.h>

#include "griddata.h"

#ifdef WITH_NN
extern "C"
{
#include "nn/nn.h"
#include "nn/delaunay.h"
}
#endif

#ifdef WITH_CSA
extern "C"
{
#include "csa.h"
}
#endif

namespace
{
    // forward declarations
    static void
        grid_nnaidw(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z,
            const std::vector<double> xg, const std::vector<double> yg, std::vector<double>& zg);

    static void
        grid_nnli(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z,
            const std::vector<double> xg, const std::vector<double> yg, std::vector<double>& zg,
            double threshold);

    static void
        grid_nnidw(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z,
            const std::vector<double> xg, const std::vector<double> yg, std::vector<double>& zg,
            int knn_order);

#ifdef WITH_CSA
    static void
        grid_csa(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z,
            const std::vector<double> xg, const std::vector<double> yg, std::vector<double>& zg);
#endif

#ifdef WITH_NN
    static void
        grid_nni(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z,
            const std::vector<double> xg, const std::vector<double> yg, std::vector<double>& zg,
            double wtmin);

    static void
        grid_dtli(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z,
            const std::vector<double> xg, const std::vector<double> yg, std::vector<double>& zg);
#endif

    static void
        dist1(const double gxvalue, const double gyvalue, const std::vector<double> x, const std::vector<double> y, int knn_order);
    static void
        dist2(const double gxvalue, const double gyvalue, const std::vector<double> x, const std::vector<double> y);

#define KNN_MAX_ORDER    100

    typedef struct pt
    {
        double dist;
        int item;
    }PT;

    thread_local PT items[KNN_MAX_ORDER];
} // anonymous namespace

namespace DSS
{
    //--------------------------------------------------------------------------
    //
    // GriddData::interpolate(): grids data from irregularly sampled data.
    //
    //    Real world data is frequently irregularly sampled, but most 3D plots
    //    require regularly gridded data. This function does exactly this
    //    using several methods:
    //    Irregularly sampled data x[npts], y[npts], z[npts] is gridded into
    //    zg[nptsx, nptsy] according to method 'type' and grid information
    //    xg[nptsx], yg[nptsy].
    //
    //    'type' can be:
    //
    //       GRID_CSA:    Bivariate Cubic Spline approximation (1)
    //       GRID_NNIDW:  Nearest Neighbors Inverse Distance Weighted
    //       GRID_NNLI:   Nearest Neighbors Linear Interpolation
    //       GRID_NNAIDW: Nearest Neighbors Around Inverse Distance Weighted
    //       GRID_DTLI:   Delaunay Triangulation Linear Interpolation (2)
    //       GRID_NNI:    Natural Neighbors interpolation (2)
    //
    // (1): Copyright 2000-2002 CSIRO Marine Research, Pavel Sakov's csa library
    // (2): Copyright 2002 CSIRO Marine Research, Pavel Sakov's nn library
    //
    //--------------------------------------------------------------------------
    void
        GridData::interpolate(
            const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z,
            const std::vector<double>& xg, const std::vector<double>& yg, std::vector<double>& zg,
            InterpolationType type, float data)
    {
        size_t i;

        if (x.size() != y.size() || x.size() != z.size())
        {
            ZInvalidParameter invParm("x, y, z arrays must have the same size",
                0,
                ZException::recoverable);
            invParm.setErrorCodeGroup(ZException::other);
            invParm.addLocation(ZEXCEPTION_LOCATION());
            invParm.logExceptionData();
            throw invParm;
        }
        if (x.size() < 2)
        {
            ZInvalidParameter invParm("at least two data points are required",
                0,
                ZException::recoverable);
            invParm.setErrorCodeGroup(ZException::other);
            invParm.addLocation(ZEXCEPTION_LOCATION());
            invParm.logExceptionData();
            throw invParm;
        }
        if (xg.size() < 2 || yg.size() < 2)
        {
            ZInvalidParameter invParm("Bad output array dimensions",
                0,
                ZException::recoverable);
            invParm.setErrorCodeGroup(ZException::other);
            invParm.addLocation(ZEXCEPTION_LOCATION());
            invParm.logExceptionData();
            throw invParm;
        }

        // Check that points in xg and in yg are strictly increasing

        for (i = 0; i < xg.size() - 1; i++)
        {
            if (xg[i] >= xg[i + 1])
            {
                ZInvalidParameter invParm("xg array must be strictly increasing",
                    4,
                    ZException::recoverable);
                invParm.setErrorCodeGroup(ZException::other);
                invParm.addLocation(ZEXCEPTION_LOCATION());
                invParm.logExceptionData();
                throw invParm;
            }
        }
        for (i = 0; i < yg.size() - 1; i++)
        {
            if (yg[i] >= yg[i + 1])
            {
                ZInvalidParameter invParm("yg array must be strictly increasing",
                    5,
                    ZException::recoverable);
                invParm.setErrorCodeGroup(ZException::other);
                invParm.addLocation(ZEXCEPTION_LOCATION());
                invParm.logExceptionData();
                throw invParm;
            }
        }

        // clear array to be returned
        size_t zgSize = xg.size() * yg.size();
        zg.clear();
        zg.reserve(zgSize);
        zg.resize(zgSize, 0.0);
        // std::numeric_limits<double>::quiet_NaN() signals a not processed grid point

        switch (type)
        {
        case (InterpolationType::GRID_CSA): //  Bivariate Cubic Spline Approximation
#ifdef WITH_CSA
            grid_csa(x, y, z, xg, yg, zg);
#else
            {
                const char* errorMessage = "GridData::interpolate You must have the CSA library installed to use GRID_CSA.\n"
                    "  Reverting to GRID_NNAIDW.";
#if defined(_CONSOLE)
                std::cerr << errorMessage.toUtf8().constData() << "\n";;
#else
                QMessageBox::information(nullptr, "DeepSkyStacker", errorMessage);
#endif
            }
            grid_nnaidw(x, y, z, xg, yg, zg);
#endif
            break;

        case (InterpolationType::GRID_NNIDW): // Nearest Neighbors Inverse Distance Weighted
            grid_nnidw(x, y, z, xg, yg, zg, static_cast<int>(data));
            break;

        case (InterpolationType::GRID_NNLI): // Nearest Neighbors Linear Interpolation
            grid_nnli(x, y, z, xg, yg, zg, data);
            break;

        case (InterpolationType::GRID_NNAIDW): // Nearest Neighbors "Around" Inverse Distance Weighted
            grid_nnaidw(x, y, z, xg, yg, zg);
            break;

        case (InterpolationType::GRID_DTLI): // Delaunay Triangulation Linear Interpolation
#ifdef WITH_NN
            grid_dtli(x, y, z, xg, yg, zg);
#else
            {
                const char* errorMessage = "GridData::interpolate You must have the Qhull library installed to use GRID_DTLI.\n"
                    "  Reverting to GRID_NNAIDW.";
#if defined(_CONSOLE)
                std::cerr << errorMessage.toUtf8().constData() << "\n";
#else
                QMessageBox::information(nullptr, "DeepSkyStacker", errorMessage);
#endif
            }
            grid_nnaidw(x, y, z, xg, yg, zg);
#endif
            break;

        case (InterpolationType::GRID_NNI): // Natural Neighbors
#ifdef WITH_NN
            grid_nni(x, y, z, xg, yg, zg, data);
#else
            {
                const char* errorMessage = "GridData::interpolate You must have the Qhull library installed to use GRID_NNI.\n"
                    "  Reverting to GRID_NNAIDW.";
#if defined(_CONSOLE)
                std::cerr << errorMessage.toUtf8().constData() << "\n";;
#else
                QMessageBox::information(nullptr, "DeepSkyStacker", errorMessage);
#endif
            }
            grid_nnaidw(x, y, z, xg, yg, zg);
#endif
            break;

        default:
            ZInvalidParameter invParm("Invalid algorithm",
                7,
                ZException::recoverable);
            invParm.setErrorCodeGroup(ZException::other);
            invParm.addLocation(ZEXCEPTION_LOCATION());
            invParm.logExceptionData();
            throw invParm;
        }
    }
}

namespace
{
#ifdef WITH_CSA
    //
    // Bivariate Cubic Spline Approximation using Pavel Sakov's csa package
    //
    // std::numeric_limits<double>::quiet_NaN()s are returned where no interpolation can be done.
    //

    static void
        grid_csa(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z,
            const std::vector<double> xg, const std::vector<double> yg, std::vector<double>& zg)
    {
        //std::vector<double> xt, yt, zt;
        point* pin, * pgrid, * pt;
        csa* a = NULL;
        size_t i, j, nptsg;

        pin = new point [x.size()];

        pt = pin;
        for (i = 0; i < x.size(); i++)
        {
            pt->x = x[i];
            pt->y = y[i];
            pt->z = z[i];
            pt++;
        }

        nptsg = xg.size() * yg.size();
		pgrid = new point [nptsg];

        pt = pgrid;
        for (j = 0; j < yg.size(); j++)
        {
            for (i = 0; i < xg.size(); i++)
            {
                pt->x = xg[i];
                pt->y = yg[j];
                pt++;
            }
        }

        a = csa_create();
        csa_addpoints(a, static_cast<int>(x.size()), pin);
        csa_calculatespline(a);
        csa_approximatepoints(a, static_cast<int>(nptsg), pgrid);

        for (i = 0; i < xg.size(); i++)
        {
            for (j = 0; j < yg.size(); j++)
            {
                pt = &pgrid[j * xg.size() + i];
                // zops->set(zgp, i, j, (PLFLT)pt->z);
                zg[i + j * xg.size()] = pt->z;
            }
        }

        csa_destroy(a);
        delete [] pin;
        delete [] pgrid;
    }
#endif // WITH_CSA

    // Nearest Neighbors Inverse Distance Weighted, brute force approach.
    //
    // The z value at the grid position will be the weighted average
    // of the z values of the KNN points found. The weight is the
    // inverse squared distance between the grid point and each
    // neighbor.
    //

    static void
        grid_nnidw(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z,
            const std::vector<double> xg, const std::vector<double> yg, std::vector<double>& zg,
            int knn_order)
    {
        double wi, nt;

        if (knn_order > KNN_MAX_ORDER)
        {
            ZInvalidParameter invParm("GridData: GRID_NNIDW: knn_order too big",
                7,
                ZException::recoverable);
            invParm.setErrorCodeGroup(ZException::other);
            invParm.addLocation(ZEXCEPTION_LOCATION());
            invParm.logExceptionData();
            throw invParm;
        }

        if (knn_order == 0)
        {

            const char* errorMessage = "GridData: GRID_NNIDW: knn_order must be specified with 'data' arg. Using 15.";
#if defined(_CONSOLE)
            std::cerr << errorMessage.toUtf8().constData() << "\n";
#else
            QMessageBox::information(nullptr, "DeepSkyStacker", errorMessage);
#endif
            knn_order = 15;
        }
        // 
		// Use ALL available processors for the interpolation
		//
#pragma omp parallel for num_threads(omp_get_num_procs()) private(wi, nt)
        for (int i = 0; i < xg.size(); i++)
        {
            for (int j = 0; j < yg.size(); j++)
            {
                dist1(xg[i], yg[j], x, y, knn_order);

#ifdef GMS  // alternative weight coeficients. I Don't like the results
                // find the maximum distance
                md = items[0].dist;
                for (int k = 1; k < knn_order; k++)
                    if (items[k].dist > md)
                        md = items[k].dist;
#endif
				// zops->set(zgp, i, j, 0.0); Was already set to 0.0 at initialization
                nt = 0.;

                for (int k = 0; k < knn_order; k++)
                {
                    if (items[k].item == -1) // not enough neighbors found ?!
                        continue;
#ifdef GMS
                    wi = (md - items[k].dist) / (md * items[k].dist);
                    wi = wi * wi;
#else
                    wi = 1. / (items[k].dist * items[k].dist);
#endif
                    zg[i + j*xg.size()] += (wi * z[items[k].item]);
                    nt += wi;
                }
                if (nt != 0.)
                    zg[i + j*xg.size()] /= nt;
                else
                    zg[i + j*xg.size()] = std::numeric_limits<double>::quiet_NaN();
            }
        }
    }

    // Nearest Neighbors Linear Interpolation
    //
    // The z value at the grid position will be interpolated from the
    // plane passing through the 3 nearest neighbors.
    //

    static void
        grid_nnli(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z,
            const std::vector<double> xg, const std::vector<double> yg, std::vector<double>& zg,
            double threshold)
    {
		std::array<double, 4> xx, yy, zz;
        double t, A, B, C, D, d1, d2, d3, max_thick;
        size_t i, j;
        int   ii, excl, cnt, excl_item;

        if (threshold == 0.)
        {
            const char* errorMessage = "GridData: GRID_NNLI: threshold must be specified with 'data' arg. Using 1.001";
#if defined(_CONSOLE)
            std::cerr << errorMessage.toUtf8().constData() << "\n";
#else
            QMessageBox::information(nullptr, "DeepSkyStacker", errorMessage);
#endif
            threshold = 1.001;
        }
        else if (threshold > 2. || threshold < 1.)
        {
            ZInvalidParameter invParm("GridData: GRID_NNLI: 1. < threshold < 2.",
                7,
                ZException::recoverable);
            invParm.setErrorCodeGroup(ZException::other);
            invParm.addLocation(ZEXCEPTION_LOCATION());
            invParm.logExceptionData();
            throw invParm;
            return;
        }

        for (i = 0; i < xg.size(); i++)
        {
            for (j = 0; j < yg.size(); j++)
            {
                dist1(xg[i], yg[j], x, y, 3);

                // see if the triangle is a thin one
                for (ii = 0; ii < 3; ii++)
                {
                    xx[ii] = x[items[ii].item];
                    yy[ii] = y[items[ii].item];
                    zz[ii] = z[items[ii].item];
                }

                d1 = sqrt((xx[1] - xx[0]) * (xx[1] - xx[0]) + (yy[1] - yy[0]) * (yy[1] - yy[0]));
                d2 = sqrt((xx[2] - xx[1]) * (xx[2] - xx[1]) + (yy[2] - yy[1]) * (yy[2] - yy[1]));
                d3 = sqrt((xx[0] - xx[2]) * (xx[0] - xx[2]) + (yy[0] - yy[2]) * (yy[0] - yy[2]));

                if (d1 == 0. || d2 == 0. || d3 == 0.) // coincident points
                {
                    zg[i + j*xg.size()] = std::numeric_limits<double>::quiet_NaN();
                    continue;
                }

                // make d1 < d2
                if (d1 > d2)
                {
                    t = d1; d1 = d2; d2 = t;
                }

                // and d2 < d3
                if (d2 > d3)
                {
                    t = d2; d2 = d3; d3 = t;
                }

                if ((d1 + d2) / d3 < threshold) // thin triangle!
                {
                    zg[i + j*xg.size()] = std::numeric_limits<double>::quiet_NaN();    // deal with it later
                }
                else                                // calculate the plane passing through the three points
                {
                    A = yy[0] * (zz[1] - zz[2]) + yy[1] * (zz[2] - zz[0]) + yy[2] * (zz[0] - zz[1]);
                    B = zz[0] * (xx[1] - xx[2]) + zz[1] * (xx[2] - xx[0]) + zz[2] * (xx[0] - xx[1]);
                    C = xx[0] * (yy[1] - yy[2]) + xx[1] * (yy[2] - yy[0]) + xx[2] * (yy[0] - yy[1]);
                    D = -A * xx[0] - B * yy[0] - C * zz[0];

                    // and interpolate (or extrapolate...)
                    zg[i + j*xg.size()] = (-xg[i] * A / C - yg[j] * B / C - D / C);
                }
            }
        }

        // now deal with NaNs resulting from thin triangles. The idea is
        // to use the 4 KNN points and exclude one at a time, creating
        // four triangles, evaluating their thickness and choosing the
        // most thick as the final one from where the interpolating
        // plane will be build.  Now that I'm talking of interpolating,
        // one should really check that the target point is interior to
        // the candidate triangle... otherwise one is extrapolating
        //

        {
            for (i = 0; i < xg.size(); i++)
            {
                for (j = 0; j < yg.size(); j++)
                {
                    if (std::isnan(zg[i + j*xg.size()]))
                    {
                        dist1(xg[i], yg[j], x, y, 4);

                        // sort by distances. Not really needed!
                        // for (ii=3; ii>0; ii--) {
                        // for (jj=0; jj<ii; jj++) {
                        // if (items[jj].dist > items[jj+1].dist) {
                        // t = items[jj].dist;
                        // items[jj].dist = items[jj+1].dist;
                        // items[jj+1].dist = t;
                        // }
                        // }
                        // }
                        //

                        max_thick = 0.; excl_item = -1;
                        for (excl = 0; excl < 4; excl++) // the excluded point

                        {
                            cnt = 0;
                            for (ii = 0; ii < 4; ii++)
                            {
                                if (ii != excl)
                                {
                                    xx[cnt] = x[items[ii].item];
                                    yy[cnt] = y[items[ii].item];
                                    cnt++;
                                }
                            }

                            d1 = sqrt((xx[1] - xx[0]) * (xx[1] - xx[0]) + (yy[1] - yy[0]) * (yy[1] - yy[0]));
                            d2 = sqrt((xx[2] - xx[1]) * (xx[2] - xx[1]) + (yy[2] - yy[1]) * (yy[2] - yy[1]));
                            d3 = sqrt((xx[0] - xx[2]) * (xx[0] - xx[2]) + (yy[0] - yy[2]) * (yy[0] - yy[2]));
                            if (d1 == 0. || d2 == 0. || d3 == 0.) // coincident points
                                continue;

                            // make d1 < d2
                            if (d1 > d2)
                            {
                                t = d1; d1 = d2; d2 = t;
                            }
                            // and d2 < d3
                            if (d2 > d3)
                            {
                                t = d2; d2 = d3; d3 = t;
                            }

                            t = (d1 + d2) / d3;
                            if (t > max_thick)
                            {
                                max_thick = t;
                                excl_item = excl;
                            }
                        }

                        if (excl_item == -1) // all points are coincident?
                            continue;

                        // one has the thicker triangle constructed from the 4 KNN
                        cnt = 0;
                        for (ii = 0; ii < 4; ii++)
                        {
                            if (ii != excl_item)
                            {
                                xx[cnt] = x[items[ii].item];
                                yy[cnt] = y[items[ii].item];
                                zz[cnt] = z[items[ii].item];
                                cnt++;
                            }
                        }

                        A = yy[0] * (zz[1] - zz[2]) + yy[1] * (zz[2] - zz[0]) + yy[2] * (zz[0] - zz[1]);
                        B = zz[0] * (xx[1] - xx[2]) + zz[1] * (xx[2] - xx[0]) + zz[2] * (xx[0] - xx[1]);
                        C = xx[0] * (yy[1] - yy[2]) + xx[1] * (yy[2] - yy[0]) + xx[2] * (yy[0] - yy[1]);
                        D = -A * xx[0] - B * yy[0] - C * zz[0];

                        // and interpolate (or extrapolate...)
                        zg[i + j*xg.size()] = (-xg[i] * A / C - yg[j] * B / C - D / C);
                    }
                }
            }
        }
    }

    //
    // Nearest Neighbors "Around" Inverse Distance Weighted, brute force approach.
    //
    // This uses the 1-KNN in each quadrant around the grid point, then
    // Inverse Distance Weighted is used as in GRID_NNIDW.
    //

    static void
        grid_nnaidw(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z,
            const std::vector<double> xg, const std::vector<double> yg, std::vector<double>& zg)
    {
        double d, nt;
        size_t i, j;
        unsigned int k;

        for (i = 0; i < xg.size(); i++)
        {
            for (j = 0; j < yg.size(); j++)
            {
                dist2(xg[i], yg[j], x, y);
				// zops->set(zgp, i, j, 0.); No need, was set to 0.0 at initialization
                nt = 0.;
                for (k = 0; k < 4; k++)
                {
                    if (items[k].item != -1)                              // was found
                    {
                        d = 1. / (items[k].dist * items[k].dist);         // 1/square distance
                        zg[i + j*xg.size()] += (d * z[items[k].item]);
                        nt += d;
                    }
                }
                if (nt == 0.) // no points found?!
                    zg[i + j*xg.size()] = std::numeric_limits<double>::quiet_NaN();
                else
                    zg[i + j*xg.size()] /= nt;
            }
        }
    }

#ifdef WITH_NN
    //
    // Delaunay Triangulation Linear Interpolation using Pavel Sakov's nn package
    //
    // The Delaunay Triangulation on the data points is build and for
    // each grid point the triangle where it is enclosed found and a
    // linear interpolation performed.
    //
    // Points exterior to the convex hull of the data points cannot
    // be interpolated and are set to std::numeric_limits<double>::quiet_NaN().
    //

    static void
        grid_dtli(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z,
            const std::vector<double> xg, const std::vector<double> yg, std::vector<double>& zg)
    {
        point* pin, * pgrid, * pt;
        size_t i, j, nptsg;

        pin = new point[x.size()];

        pt = pin;
        for (i = 0; i < x.size(); i++)
        {
            pt->x = x[i];
            pt->y = y[i];
            pt->z = z[i];
            pt++;
        }

        nptsg = xg.size() * yg.size();
        pgrid = new point[nptsg];

        pt = pgrid;
        for (j = 0; j < yg.size(); j++)
        {
            for (i = 0; i < xg.size(); i++)
            {
                pt->x = xg[i];
                pt->y = yg[j];
                pt++;
            }
        }
        delaunay* d = delaunay_build(static_cast<int>(x.size()), pin, 0, NULL, 0, NULL);

        lpi_interpolate_points(d, static_cast<int>(nptsg), pgrid);
        for (i = 0; i < xg.size(); i++)
        {
            for (j = 0; j < yg.size(); j++)
            {
                pt = &pgrid[j * xg.size() + i];
                zg[i + j * xg.size()] = pt->z;
            }
        }

        delete[] pin;
        delete[] pgrid;
    }

    //
    // Natural Neighbors using Pavel Sakov's nn package
    //
    // Points exterior to the convex hull of the data points cannot
    // be interpolated and are set to std::numeric_limits<double>::quiet_NaN().
    //

    static void
        grid_nni(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z,
            const std::vector<double> xg, const std::vector<double> yg, std::vector<double>& zg,
            double wtmin)
    {
        point* pin, * pgrid, * pt;
        size_t i, j, nptsg;
        nn_rule = NON_SIBSONIAN;

        if (wtmin == 0.f)  // only accept weights greater than wtmin
        {

            const char* errorMessage = "GridData: GRID_NNI: wtmin must be specified with 'data' arg. Using -std::numeric_limits<float>::max()";
#if defined(_CONSOLE)
            std::cerr << errorMessage.toUtf8().constData() << "\n";
#else
            QMessageBox::information(nullptr, "DeepSkyStacker", errorMessage);
#endif
            wtmin = -std::numeric_limits<float>::max();
        }

        pin = new point[x.size()];

        pt = pin;
        for (i = 0; i < x.size(); i++)
        {
            pt->x = x[i];
            pt->y = y[i];
            pt->z = z[i];
            pt++;
        }

        nptsg = xg.size() * yg.size();
        pgrid = new point[nptsg];

        pt = pgrid;
        for (j = 0; j < yg.size(); j++)
        {
            for (i = 0; i < xg.size(); i++)
            {
                pt->x = xg[i];
                pt->y = yg[j];
                pt++;
            }
        }

        delaunay* d = delaunay_build(static_cast<int>(x.size()), pin, 0, NULL, 0, NULL);

        nnpi_interpolate_points(d, wtmin, static_cast<int>(nptsg), pgrid);
        for (i = 0; i < xg.size(); i++)
        {
            for (j = 0; j < yg.size(); j++)
            {
                pt = &pgrid[j * xg.size() + i];
                zg[i + j * xg.size()] = pt->z;
            }
        }

        delete [] pin;
        delete [] pgrid;
    }
#endif // WITH_NN

    //
    // this function just calculates the K Nearest Neighbors of grid point
    // [gxvalue, gyvalue].
    //

    static void
        dist1(const double gxvalue, const double gyvalue, const std::vector<double> x, const std::vector<double> y, int knn_order)
    {
        double d, max_dist;
        size_t   max_slot, i, j;

        max_dist = std::numeric_limits<double>::max();
        max_slot = 0;

        for (i = 0; i < knn_order; i++)
        {
            items[i].dist = std::numeric_limits<double>::max();
            items[i].item = -1;
        }

        for (i = 0; i < x.size(); i++)
        {
            d = ((gxvalue - x[i]) * (gxvalue - x[i]) + (gyvalue - y[i]) * (gyvalue - y[i])); // save sqrt() time

            if (d < max_dist)
            {
                // found an item with a distance smaller than the
                // maximum distance found so far. Replace.
                //

                items[max_slot].dist = d;
                items[max_slot].item = static_cast<int>(i);

                // find new maximum distance
                max_dist = items[0].dist;
                max_slot = 0;
                for (j = 1; j < knn_order; j++)
                {
                    if (items[j].dist > max_dist)
                    {
                        max_dist = items[j].dist;
                        max_slot = j;
                    }
                }
            }
        }
        for (j = 0; j < knn_order; j++)
            items[j].dist = std::sqrt(items[j].dist); // now calculate the distance
    }

    //
    // This function searchs the 1-nearest neighbor in each quadrant around
    // the grid point.
    //

    static void
        dist2(const double gxvalue, const double gyvalue, 
            const std::vector<double> x, const std::vector<double> y)
    {
        double d;
        size_t   i, quad;

        for (i = 0; i < 4; i++)
        {
            items[i].dist = std::numeric_limits<double>::max();
            items[i].item = -1;
        }

        for (i = 0; i < x.size(); i++)
        {
            d = ((gxvalue - x[i]) * (gxvalue - x[i]) + (gyvalue - y[i]) * (gyvalue - y[i])); // save sqrt() time

            // trick to quickly compute a quadrant. The determined quadrants will be
            // miss-assigned, i.e., 1->2, 2->0, 3->1, 4->3, but that is not important,
            // speed is.

            quad = 2 * (x[i] > gxvalue) + (y[i] < gyvalue);

            // try to use the octants around the grid point, as it will give smoother
            // (and slower) results.
            // Hint: use the quadrant info plus x[i]/y[i] to determine the octant

            if (d < items[quad].dist)
            {
                items[quad].dist = d;
                items[quad].item = static_cast<int>(i);
            }
        }

        for (i = 0; i < 4; i++)
            if (items[i].item != -1)
                items[i].dist = sqrt(items[i].dist);
        // now calculate the distance
    }

#ifdef PLPLOT_NONN // another DTLI, based only on QHULL, not nn
    static void
        grid_adtli(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z,
            const std::vector<double> xg, const std::vector<double> yg, std::vector<double>& zg  PLF2OPS zops, PLPointer zgp)
    {
        coordT* points;          // array of coordinates for each point
        boolT   ismalloc = False; // True if qhull should free points
        char    flags[250];       // option flags for qhull
        facetT* facet;           // set by FORALLfacets
        vertexT* vertex, ** vertexp;
        facetT* neighbor, ** neighborp;
        int     curlong, totlong;  // memory remaining after qh_memfreeshort
        FILE* outfile = NULL;
        FILE* errfile = stderr; // error messages from qhull code

        int     exitcode;
        int     i, j, k, l;
        int     dim = 2;
        PLFLT   xt[3], yt[3], zt[3];
        PLFLT   A, B, C, D;
        coordT  point[3];
        boolT   isoutside;
        realT   bestdist;
        int     totpart = 0;
        int     numfacets, numsimplicial, numridges;
        int     totneighbors, numcoplanars, numtricoplanars;

        plwarn("plgriddata: GRID_DTLI, If you have QHull knowledge, FIXME.");

        // Could pass extra args to qhull through the 'data' argument of
        // plgriddata()
        strcpy(flags, "qhull d Qbb Qt", 250);

        if ((points = (coordT*)malloc(npts * (dim + 1) * sizeof(coordT))) == NULL)
        {
            plexit("grid_adtli: Insufficient memory");
        }

        for (i = 0; i < npts; i++)
        {
            points[i * dim] = x[i];
            points[i * dim + 1] = y[i];
        }

#if 1 // easy way
        exitcode = qh_new_qhull(dim, points, ismalloc,
            flags, outfile, errfile);
#else
        qh_init_A(stdin, stdout, stderr, 0, NULL);
        exitcode = setjmp(qh errexit);
        if (!exitcode)
        {
            qh_initflags(flags);
            qh PROJECTdelaunay = True;
            qh_init_B(points, dim, ismalloc);
            qh_qhull();
        }
#endif
        if (!exitcode)                // if no error

        {
#if 0   // print the triangles vertices
            printf("Triangles\n");
            FORALLfacets{
                if (!facet->upperdelaunay)
                {
                    FOREACHvertex_(facet->vertices)
                    printf(" %d", qh_pointid(vertex->point)); // vertices index
                    printf("\n");
                }
            }
#endif

#if 0   // print each triangle neighbors
            printf("Neigbors\n");

            qh_findgood_all(qh facet_list);
            qh_countfacets(qh facet_list, NULL, !qh_ALL, &numfacets, &numsimplicial,
                &totneighbors, &numridges, &numcoplanars, &numtricoplanars);

            FORALLfacets{
                if (!facet->upperdelaunay)
                {
                    FOREACHneighbor_(facet)
                    printf(" %d", neighbor->visitid ? neighbor->visitid - 1 : -neighbor->id);
                    printf("\n");
                }
            }
#endif

                // Without the setjmp(), Qhull will exit() after reporting an error
            exitcode = setjmp(qh errexit);
            if (!exitcode)
            {
                qh NOerrexit = False;
                for (i = 0; i < nptsx; i++)
                    for (j = 0; j < nptsy; j++)
                    {
                        l = 0;
                        point[0] = xg[i];
                        point[1] = yg[j];
                        qh_setdelaunay(3, 1, point);


                        // several ways to find the triangle given a point follow.
                        // None but brute force works
#if 0
                        facet = qh_findbestfacet(point, qh_ALL, &bestdist, &isoutside);
#endif

#if 0
                        facet = qh_findbest(point, qh facet_list, qh_ALL,
                            !qh_ISnewfacets, //qh_ALL
                            qh_NOupper,
                            &bestdist, &isoutside, &totpart);
#endif

#if 0
                        vertex = qh_nearvertex(facet, point, &bestdist);
#endif

                        // Until someone implements a working qh_findbestfacet(),
                        // do an exhaustive search!
                        //
                        // As far as I understand it, qh_findbestfacet() and
                        // qh_findbest() fails when 'point' does not belongs to
                        // the convex hull, i.e., when the search becomes blocked
                        // when a facet is upperdelaunay (although the error
                        // message says that the facet may be upperdelaynay or
                        // flipped, I never found a flipped one).
                        //
                        // Another possibility is to implement the 'walking
                        // triangle algorithm

                        facet = qh_findfacet_all(point, &bestdist, &isoutside, &totpart);

                        if (facet->upperdelaunay)
                            zops->set(zgp, i, j, std::numeric_limits<double>::quiet_NaN());
                        else
                        {
                            FOREACHvertex_(facet->vertices)
                            {
                                k = qh_pointid(vertex->point);
                                xt[l] = x[k];
                                yt[l] = y[k];
                                zt[l] = z[k];
                                l++;
                            }

                            // calculate the plane passing through the three points

                            A = yt[0] * (zt[1] - zt[2]) + yt[1] * (zt[2] - zt[0]) + yt[2] * (zt[0] - zt[1]);
                            B = zt[0] * (xt[1] - xt[2]) + zt[1] * (xt[2] - xt[0]) + zt[2] * (xt[0] - xt[1]);
                            C = xt[0] * (yt[1] - yt[2]) + xt[1] * (yt[2] - yt[0]) + xt[2] * (yt[0] - yt[1]);
                            D = -A * xt[0] - B * yt[0] - C * zt[0];

                            // and interpolate
                            zops->set(zgp, i, j, -xg[i] * A / C - yg[j] * B / C - D / C);
                        }
                    }
            }
            qh NOerrexit = True;
        }

        free(points);
        qh_freeqhull(!qh_ALL);               // free long memory
        qh_memfreeshort(&curlong, &totlong); // free short memory and memory allocator
        if (curlong || totlong)
            fprintf(errfile,
                "qhull: did not free %d bytes of long memory (%d pieces)\n",
                totlong, curlong);
    }
#endif // PLPLOT_NONN
} // anonymous namespace
