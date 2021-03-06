#include "MonteCarlo.h"

void MonteCarlo::diagramOrder2 (double length, unsigned int index) {
  Vector3d P0 = this->externalMomentum;

  for (unsigned int diagramType = 0; diagramType != 3; diagramType++) {
    double value = 0;

    if (this->irreducibleDiagrams && diagramType == 1) {
      continue;
    }
    
    for (long unsigned int i = 0; i != this->numIterations; i++) {

      double t1 = 0, t2 = 0, t3 = 0, t4 = 0, wInv_ts = 0, std1 = 0, std2 = 0;

      if (this->externalLegs) {
        // sample times
        t1 = this->Udouble(0, length);
        t2 = this->Udouble(t1, length);
        t3 = this->Udouble(t2, length);
        t4 = this->Udouble(t3, length);
        wInv_ts = length * (length - t1) * (length - t2) * (length - t3);

        // integrand value
        if (diagramType == 1) {
          std1 = pow(t2 - t1, -0.5);
          std2 = pow(t4 - t3, -0.5);
        } else if (diagramType == 2) {
          std1 = pow(t3 - t1, -0.5);
          std2 = pow(t4 - t2, -0.5);
        } else {
          std1 = pow(t4 - t1, -0.5);
          std2 = pow(t3 - t2, -0.5);
        }
      } else {
        // sample times
        t1 = this->Udouble(0, length),
        t2 = this->Udouble(t1, length),
        wInv_ts = length * (length - t1);

        // integrand value
        if (diagramType == 1) {
          std1 = pow(t1, -0.5);
          std2 = pow(length - t2, -0.5);
        } else if (diagramType == 2) {
          std1 = pow(t2, -0.5);
          std2 = pow(length - t1, -0.5);
        } else {
          std1 = pow(length, -0.5);
          std2 = pow(t2 - t1, -0.5);
        }
      }

      // sample momentum
      double
        q1 = abs(this->Ndouble(std1)),
        theta1 = this->Udouble(0, M_PI),
        phi1 = this->Udouble(0, 2*M_PI),
        wInv_Q1 = 2*M_PI*M_PI * sqrt(0.5*M_PI*std1*std1) * exp(0.5*pow(q1/std1, 2.0));

      Vector3d Q1{
        q1*sin(theta1)*cos(phi1),
        q1*sin(theta1)*sin(phi1),
        q1*cos(theta1)
      };

      // sample momentum
      double
        q2 = abs(this->Ndouble(std2)),
        theta2 = this->Udouble(0, M_PI),
        phi2 = this->Udouble(0, 2*M_PI),
        wInv_Q2 = 2*M_PI*M_PI * sqrt(0.5*M_PI*std2*std2) * exp(0.5*pow(q2/std2, 2.0));

      Vector3d Q2{
        q2*sin(theta2)*cos(phi2),
        q2*sin(theta2)*sin(phi2),
        q2*cos(theta2)
      };

      // chemical potential factor
      double
        integrand,
        chemFac = exp(this->mu*length);

      if (this->externalLegs) {
        // integrand value
        if (diagramType == 1) {
          integrand = this->G(P0, 0, t1)
                    * this->G(P0 - Q1, t1, t2) * this->D(Q1, theta1, t1, t2)
                    * this->G(P0, t2, t3)
                    * this->G(P0 - Q2, t3, t4) * this->D(Q2, theta2, t3, t4)
                    * this->G(P0, t4, length);
        } else if (diagramType == 2) {
          integrand = this->G(P0, 0, t1)
                    * this->G(P0 - Q1, t1, t2) * this->D(Q1, theta1, t1, t3)
                    * this->G(P0 - Q1 - Q2, t2, t3) * this->D(Q2, theta2, t2, t4)
                    * this->G(P0 - Q2, t3, t4)
                    * this->G(P0, t4, length);
        } else {
          integrand = this->G(P0, 0, t1)
                    * this->G(P0 - Q1, t1, t2) * this->D(Q1, theta1, t1, t4)
                    * this->G(P0 - Q1 - Q2, t2, t3) * this->D(Q2, theta2, t2, t3)
                    * this->G(P0 - Q1, t3, t4)
                    * this->G(P0, t4, length);
        }
      } else {
        // integrand value
        if (diagramType == 1) {
          integrand = this->G(P0 - Q1, 0, t1) * this->D(Q1, theta1, 0, t1)
                    * this->G(P0, t1, t2)
                    * this->G(P0 - Q2, t2, length) * this->D(Q2, theta2, t2, length);
        } else if (diagramType == 2) {
          integrand = this->G(P0 - Q1, 0, t1) * this->D(Q1, theta1, 0, t2)
                    * this->G(P0 - Q1 - Q2, t1, t2) * this->D(Q2, theta2, t1, length)
                    * this->G(P0 - Q2, t2, length);
        } else {
          integrand = this->G(P0 - Q1, 0, t1) * this->D(Q1, theta1, 0, length)
                    * this->G(P0 - Q1 - Q2, t1, t2) * this->D(Q2, theta2, t1, t2)
                    * this->G(P0 - Q1, t2, length);
        }
      }

      value += integrand * wInv_ts * wInv_Q1 * wInv_Q2;
    }
    this->values[index] += value/this->numIterations;
  }
}