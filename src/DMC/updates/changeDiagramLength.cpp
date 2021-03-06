#include "../DiagrammaticMonteCarlo.h"

void DiagrammaticMonteCarlo::changeDiagramLength (double param) {

  double
    l = 0.5*this->FD.end->G[0]->momentum.squaredNorm() - this->mu,
    r = this->Udouble(0, 1),
    tmin = this->FD.end->G[0]->start->position;

  // phonon contribution
  if ( ! this->externalLegs && this->FD.Ds.size() > 0) {
    l += this->FD.phononEnergy(this->FD.end->D[0]->q);
  }

  double dt = -log(1 - r + r*exp(-l*(this->maxLength - tmin)))/l;

  double oldVal = 0, oldwInvt = 0, wInvt = 0;
  if (this->debug) {
    oldVal = this->evaluateDiagram();
    oldwInvt = exp(l*(this->FD.end->position - tmin))*(1 - exp(-l*(this->maxLength - tmin)))/l;
    wInvt = exp(l*dt)*(1 - exp(-l*(this->maxLength - tmin)))/l;
  }

  double boldContribution = 1;
  bool accepted = false;
  if (this->bold && this->boldIteration > 0) {
    // contribution from boldification
    shared_ptr<Electron> g = this->FD.Gs.back();

    boldContribution = exp(
                         this->additionalPhase(g->p, dt)
                         - this->additionalPhase(g)
                       );

    // the rest of the acceptance ratio is unity
    if (boldContribution > this->Udouble(0, 1)) {
      this->FD.setLength(tmin + dt);
      this->FD.end->setPosition(tmin + dt);
      accepted = true;
    }

  } else {
    // is always accepted
    this->FD.setLength(tmin + dt);
    this->FD.end->setPosition(tmin + dt);
    accepted = true;
  }


  //   shared_ptr<Electron> g = this->FD.Gs.back();
  // cout << "--------------" << endl
  //      << "changeDiagramLength" << endl
  //      << "\tn=" << this->FD.Ds.size() << endl
  //      << "\tt=" << this->FD.length << endl
  //      << "\tp=" << this->FD.externalMomentum << endl
  //      << "\tdt=" << dt << endl
  //      << "\tboldContribution=" << boldContribution << endl
  //      << "\taccepted=" << accepted << endl
  //      << "\tadd1=" << this->additionalPhase(g->p, tmin + dt) << endl
  //      << "\tadd2=" << this->additionalPhase(g) << endl
  //      << "--------------" << endl;


  if ( ! isfinite(dt)) {
    cout
      << "-------------------------" << endl
      << "DMC::changeDiagramLength: nan encountered" << endl
      << "dt=" << dt << endl
      << "tmin=" << tmin << endl
      << "r=" << r << endl
      << "l=" << l << endl
      << "-------------------------" << endl;
  }

  if (this->debug) {
    double val = this->evaluateDiagram();

    double a;
    if (val == 0) {
      a = 0;
    } else if (oldwInvt == 0 || oldVal == 0) {
      a = 1;
    } else {
      a = boldContribution * oldwInvt/wInvt / (val/oldVal);
    }

    if (accepted) {
      this->checkAcceptanceRatio(a, "changeDiagramLength");
    }

    if (a < 0 || ! isfinite(a)) {
      cout << "--------------------------------------------------------------------" << endl
           << "overflow at DMC::changeDiagramLength " << endl
           << "a=" << a << endl
           << "order=" << this->FD.Ds.size() << endl
           << "val=" << val << endl
           << "oldVal=" << oldVal << endl
           << "wInvt=" << wInvt << endl
           << "oldwInvt=" << oldwInvt << endl
           << "--------------------------------------------------------------------" << endl;
    } else if (this->loud) {
      cout << "changeDiagramLength: " << a << endl;
    }
  }
}