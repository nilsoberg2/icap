
#include <algorithm>

#include "normcrit.h"
#include "mannings_math.h"



// This code is ported from the following code:
//      JM Mier, October 2010, UIUC
//      Based on previous version by JM Mier, November 2007, UIUC
//      JM Mier, UIUC, September 2013 - Modified to include English units


const int maxIter = 20;


int ComputeNormalDepth(xs::Reach reach, double Q, double g, double ks, double& yN)
{
    double n = reach.getRoughness();
    double d = reach.getMaxDepth();
    double Ss = sqrt(reach.getSlope());

    std::shared_ptr<xs::CrossSection> xs = reach.getXs();

    double initialGuess = d / 2; // ((n / ks) * Q * pow(M_PI, TWOTHIRDS)) / (pow(0.75 * d, FIVETHIRDS) * Ss);
    yN = initialGuess;

    bool converged = false;
    int iterCount = 0;
    while (!converged && iterCount++ < maxIter)
    {
        double y = std::max(std::min(yN, 0.9999 * d), 0.0001 * d);

        double A = xs->computeArea(y);
        double P = xs->computeWettedPerimiter(y);
        double T = xs->computeTopWidth(y);
        double dPdy = xs->computeDpDy(y);
        double Rp = pow(A / P, TWOTHIRDS);

        double F = A * Rp - Q * (n/ks) / Ss;
        double dF = FIVETHIRDS * Rp * T - TWOTHIRDS * pow(A / P, FIVETHIRDS) * dPdy;

        double yp = y - F / dF;

        if (std::abs(yp - y) < 1e-6)
        {
            converged = true;
            yN = std::max(std::min(yp, 0.9999 * d), 0.0001 * d);
        }
        else
        {
            yN = yp;
        }
    }

    if (iterCount >= maxIter)
    {
        yN = d;
        return iterCount;
    }
    else
    {
        return 0;
    }
}


int ComputeCriticalDepth(xs::Reach reach, double Q, double g, double& yC)
{
    double d = reach.getMaxDepth();

    std::shared_ptr<xs::CrossSection> xs = reach.getXs();

    double initialGuess = std::sqrt(Q / std::sqrt(d * g));
    yC = initialGuess;

    bool converged = false;
    int iterCount = 0;
    while (!converged && iterCount++ < 20)
    {
        double y = std::max(std::min(yC, 0.9999 * d), 0.0001 * d);

        double A = xs->computeArea(y);
        double P = xs->computeWettedPerimiter(y);
        double T = xs->computeTopWidth(y);
        double dTdy = xs->computeDtDy(y);

        double F = Q*Q * T / (g * A*A*A) - 1;
        double dF = (Q*Q / g) * ( -3. * T*T / (A*A*A*A) + dTdy / (A*A*A) );

        double yp = y - F / dF;

        if (std::abs(yp - y) < 1e-6)
        {
            converged = true;
            yC = std::max(std::min(yp, 0.9999 * d), 0.0001 * d);
        }
        else
        {
            yC = yp;
        }
    }

    if (iterCount >= 20)
    {
        yC = d;
        return iterCount;
    }
    else
    {
        return 0;
    }
}

