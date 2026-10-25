#ifndef EXPERIMENTALSIMULATION_H
#define EXPERIMENTALSIMULATION_H

namespace ExperimentalSimulation {
struct Parameters {bool active=false;double takeoff=1,flight=1,wind=1;};
Parameters current();
class Scoped {
    Parameters previous;
public:
    Scoped(double takeoff,double flight,double wind);
    ~Scoped();
    Scoped(const Scoped &)=delete;
    Scoped &operator=(const Scoped &)=delete;
};
}
#endif
