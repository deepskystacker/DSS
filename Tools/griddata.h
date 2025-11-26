#pragma once
/****************************************************************************
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
// griddata.h : header file
//
namespace DSS
{
	class GridData : public QObject
    {
        Q_OBJECT
        friend class QualityChart;
    public:
        enum class InterpolationType
        {
            GRID_CSA = 0,   // Bivariate Cubic Spline Approximation
            GRID_NNAIDW,    // Natural Neighbors with Inverse Distance Weighting
            GRID_NNLI,      // Natural Neighbors with Linear Interpolation
            GRID_NNIDW,     // Natural Neighbors with Inverse Distance Weighting (KNN)
            GRID_DTLI,      // Delaunay Triangulation Linear Interpolation
            GRID_NNI        // Natural Neighbors Interpolation
        };

        GridData(QObject* parent = nullptr) : QObject(parent)
        {
		};

        GridData(const GridData&) = delete;
        GridData& operator=(const GridData&) = delete;
        GridData(GridData&&) = delete;

        ~GridData()
        {
        };

        void
            interpolate(
                const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z,
                const std::vector<double>& xg, const std::vector<double>& yg, std::vector<double>& zg,
                InterpolationType type, float data = 0.0);

    signals:
        void
            setProgressRange(int min, int max);
        void
            setProgressValue(int amount);

    private:
        inline constexpr static size_t KNN_MAX_ORDER = 100;

        typedef struct pt
        {
            double dist;
            int item;
        }PT;

        inline static thread_local PT items[KNN_MAX_ORDER];

        void
            grid_nnaidw(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z,
                const std::vector<double> xg, const std::vector<double> yg, std::vector<double>& zg);

        void
            grid_nnli(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z,
                const std::vector<double> xg, const std::vector<double> yg, std::vector<double>& zg,
                double threshold);

        void
            grid_nnidw(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z,
                const std::vector<double> xg, const std::vector<double> yg, std::vector<double>& zg,
                int knn_order);

#ifdef WITH_CSA
        void
            grid_csa(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z,
                const std::vector<double> xg, const std::vector<double> yg, std::vector<double>& zg);
#endif

#ifdef WITH_NN
        void
            grid_nni(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z,
                const std::vector<double> xg, const std::vector<double> yg, std::vector<double>& zg,
                double wtmin);

        void
            grid_dtli(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z,
                const std::vector<double> xg, const std::vector<double> yg, std::vector<double>& zg);
#endif

        void
            dist1(const double gxvalue, const double gyvalue, const std::vector<double> x, const std::vector<double> y, int knn_order);
        void
            dist2(const double gxvalue, const double gyvalue, const std::vector<double> x, const std::vector<double> y);

        bool cancel{ false };

    };
}