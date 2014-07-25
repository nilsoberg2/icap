GetUpstream

    //cout << "MILD" << endl;
    /*======================================================
                                 *           *
                               *            *
                          *               *
          (x, y)   * (x1, y1)          *
              o   /              *
                 /      *
                *

    The idea is that if x is left of the critical line, then
    use the value f_critical(discharge) = upstream gives.

    Also,
            _____               ____...
           /     \      ____----
          /       \*----   
         |   interp --->        ____...
         |              ____----
         |         *----
         |---------|
          \0000000/
           \00000/
             ---

    In this case, the water level in the larger tunnel is < than the
    downstream channel bottom elevation of the smaller tunnel, and the
    smaller tunnel has water in it.  In this case, we use the critical
    line to compute an upstream elevation f(Q) = upstream.

    ======================================================*/


    //cout << "STEEP" << endl;
    /*======================================================
                        *         *
                  *          *
               *        *
             *       *
           *      *
          *  ____* (x1, y1)
         *---  

          o (x, y)

        Where (x, y) is the desired point.

    The idea is that if y is below the critical line, then use
    the value that f_critical(discharge) = upstream gives.
?    In addition, if (x, y) is above the critical line, but
?    y < y1, then do a special interpolation in the "near-critical region"
?    using interpolateSteepCriticalRegion().

    Also,
            _____               ____...
           /     \      ____----
          /       \*----   
         |   interp --->        ____...
         |              ____----
         |         *----
         |---------|
          \0000000/
           \00000/
             ---

    In this case, the water level in the larger tunnel is < than the
    downstream channel bottom elevation of the smaller tunnel, and the
    smaller tunnel has water in it.  In this case, we use the critical
    line to compute an upstream elevation f(Q) = upstream.

    ======================================================*/





double HPG::interpolateSteep(const int& curve, const double& discharge, const double& y_down)
{
  /*======================================================
                    *         *
              *          *
           *  o     *   where o is (y_down, y)
         * f->|  *
       *      *
      *      * <- above
     * <- below

    The point we want is y.

    The idea is to get a scaling factor that we can multiply
    |y_below - y_above| by to add to the lower value to get y.

  ======================================================*/












double HPG::interpolateSteepCriticalRegion(const int & curve, const double& discharge, const double& y_down)
{
  /*======================================================
  The math behind this:

                                  o ...
      (x2, y2)       o          o
             +o   <--  above___/
           o +(y_down, y3)
         o    _____/
        o__+_/  <--  below (critical line)
       o/  (x1, y1)

          y3 is the unknown that we are solving for

          y_up(x) = m * x + b

          y2 - y1
      m = -------
          x2 - x1

          b = y2 - m * y_down

          y_up(y_down) = m * y_down + b = y3

  ======================================================*/



















double HPG::interpolateMild(const int& curve, const double& discharge, const double& y_down)
{
  /*======================================================
    The math behind this:

                               *           *
                           + *            *
                        *  |            *
        above -> *         o (y_down, y)
         y_c -> /       f->|   *
               /      *    +
     below -> *

    The point we want is y.

    The idea is to get a scaling factor that we can multiply
    |y_below - y_above| by to add to the lower value to get y.

  ======================================================*/













double HPG::GetCriticalUpstream(const double& discharge)
{
  sError = 0;
  /*======================================================
    The math behind this:

      y_up = f(Q)

                   y_up

                   ^
                   |
                   |
                   |                   +
                   |             +
                   |        + 
      y_up = f(q) >|----+o
                   |  +  |
                   |+    |
                 +--------------------> Q
                         ^
                         Q = q

  ======================================================*/
