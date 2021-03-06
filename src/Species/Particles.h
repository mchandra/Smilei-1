#ifndef PARTICLES_H
#define PARTICLES_H

#include <cmath>

#include <iostream>
#include <fstream>
#include <vector>

#include "Tools.h"
#include "TimeSelection.h"

class Particle;

class Params;
class Patch;



//----------------------------------------------------------------------------------------------------------------------
//! Particle class: holds the basic properties of a particle
//----------------------------------------------------------------------------------------------------------------------
class Particles {
public:

    //! Constructor for Particle
    Particles();

    //! Destructor for Particle
    ~Particles(){}

    //! Create nParticles null particles of nDim size
    void initialize(unsigned int nParticles, unsigned int nDim );

    //! Create nParticles null particles of nDim size
    void initialize(unsigned int nParticles, Particles &part );

    //! Set capacity of Particles vectors
    void reserve( unsigned int n_part_max, int nDim );

    //! Reset Particles vectors
    void clear();

    //! Get number of particules
    inline unsigned int size() const {
        return Weight.size();
    }

    //! Get number of particules
    inline unsigned int capacity() const {
        return Weight.capacity();
    }

    //! Get dimension of particules
    inline int dimension() const {
        return Position.size();
    }

    //! Copy particle iPart at the end of dest_parts
    void cp_particle(int iPart, Particles &dest_parts );

    //! Insert nPart particles starting at ipart to dest_id in dest_parts
    void cp_particles(int iPart, int nPart, Particles &dest_parts, int dest_id );
    //! Insert particle iPart at dest_id in dest_parts
    void cp_particle(int ipart, Particles &dest_parts, int dest_id );

    //! Suppress particle iPart
    void erase_particle(int iPart );
    //! Suppress nPart particles from iPart
    void erase_particle(int iPart, int nPart );

    //! Suppress all particles from iPart to the end of particle array
    void erase_particle_trail(int iPart );

    //! Print parameters of particle iPart
    void print(int iPart);

    friend std::ostream& operator << (std::ostream&, const Particles& particle);

    //! Exchange particles part1 & part2 memory location
    void swap_part(int part1,int part2);

    //! Exchange particles part1 & part2 memory location
    void swap_part(int part1,int part2, int N);

    //! Overwrite particle part1 into part2 memory location. Erasing part2
    void overwrite_part(int part1,int part2);

    //! Overwrite particle part1->part1+N into part2->part2+N memory location. Erasing part2->part2+N
    void overwrite_part(int part1,int part2,int N);

    //! Overwrite particle part1->part1+N into part2->part2+N of dest_parts memory location. Erasing part2->part2+N
    void overwrite_part(int part1, Particles &dest_parts, int part2,int N);

    //! Overwrite particle part1 into part2 of dest_parts memory location. Erasing part2
    void overwrite_part(int part1, Particles &dest_parts, int part2);


    //! Move iPart at the end of vectors
    void push_to_end(int iPart );

    //! Create new particle
    void create_particle();

    //! Create nParticles new particles
    void create_particles(int nParticles);

    //! Test if ipart is in the local patch
    bool is_part_in_domain(int ipart, Patch* patch);

    //! Method used to get the Particle position
    inline double  position( int idim, int ipart ) const {
        return Position[idim][ipart];
    }
    //! Method used to set a new value to the Particle former position
    inline double& position( int idim, int ipart )       {
        return Position[idim][ipart];
    }

    //! Method used to get the Particle position
    inline double  position_old( int idim, int ipart ) const {
        return Position_old[idim][ipart];
    }
    //! Method used to set a new value to the Particle former position
    inline double& position_old( int idim, int ipart )       {
        return Position_old[idim][ipart];
    }

    //! Method used to get the list of Particle position
    inline std::vector<double>  position(int idim) const {
        return Position[idim];
    }

    //! Method used to get the Particle momentum
    inline double  momentum( int idim, int ipart ) const {
        return Momentum[idim][ipart];
    }
    //! Method used to set a new value to the Particle momentum
    inline double& momentum( int idim, int ipart )       {
        return Momentum[idim][ipart];
    }
      //! Method used to get the Particle momentum
    inline std::vector<double>  momentum( int idim ) const {
        return Momentum[idim];
    }

    //! Method used to get the Particle weight
    inline double  weight(int ipart) const {
        return Weight[ipart];
    }
    //! Method used to set a new value to the Particle weight
    inline double& weight(int ipart)       {
        return Weight[ipart];
    }
    //! Method used to get the Particle weight
    inline std::vector<double>  weight() const {
        return Weight;
    }

    //! Method used to get the Particle charge
    inline short  charge(int ipart) const {
        return Charge[ipart];
    }
    //! Method used to set a new value to the Particle charge
    inline short& charge(int ipart)       {
        return Charge[ipart];
    }
    //! Method used to get the list of Particle charges
    inline std::vector<short>  charge() const {
        return Charge;
    }


    //! Method used to get the Particle Lorentz factor
    inline double lor_fac(int ipart) {
        return sqrt(1.+pow(momentum(0,ipart),2)+pow(momentum(1,ipart),2)+pow(momentum(2,ipart),2));
    }

    //! Partiles properties, respect type order : all double, all short, all unsigned int

    //! array containing the particle position
    std::vector< std::vector<double> > Position;

    //! array containing the particle former (old) positions
    std::vector< std::vector<double> >Position_old;

    //! array containing the particle moments
    std::vector< std::vector<double> >  Momentum;

    //! containing the particle weight: equivalent to a charge density
    std::vector<double> Weight;

    //! containing the particle weight: equivalent to a charge density
    std::vector<double> Chi;


    //! charge state of the particle (multiples of e>0)
    std::vector<short> Charge;

    //! Id of the particle
    std::vector<unsigned int> Id;

    // TEST PARTICLE PARAMETERS
    bool isTest;

    //! True if tracking the particles (activates one DiagTrack)
    bool tracked;
    //! Time selection for tracking particles
    TimeSelection * track_timeSelection;


    void setIds() {
        unsigned int s = Id.size();
        for (unsigned int iPart=0; iPart<s; iPart++) Id[iPart] = iPart+1;
    }
    void addIdOffsets(int offset) {
        unsigned int s = Id.size();
        for (unsigned int iPart=0; iPart<s; iPart++) Id[iPart] += offset;
    }


    //! Method used to get the Particle Id
    inline unsigned int id(int ipart) const {
        DEBUG(ipart << " of " << Id.size());
        return Id[ipart];
    }
    //! Method used to set the Particle Id
    inline unsigned int& id(int ipart) {
        return Id[ipart];
    }
    //! Method used to get the Particle Ids
    inline std::vector<unsigned int> id() const {
        return Id;
    }
    void sortById();


    // PARAMETERS FOR PARTICLES THAT ARE SUBMITTED TO A RADIATION REACTION FORCE (CED or QED)

    bool isRadReaction;


    //! Method used to get the Particle chi factor
    inline double  chi(int ipart) const {
        return Chi[ipart];
    }
    //! Method used to set a new value to the Particle chi factor
    inline double& chi(int ipart)       {
        return Chi[ipart];
    }
    //! Method used to get the Particle chi factor
    inline std::vector<double>  chi() const {
        return Chi;
    }

    std::vector< std::vector<double>* >       double_prop;
    std::vector< std::vector<short>* >        short_prop;
    std::vector< std::vector<unsigned int>* > uint_prop;


    //bool test_move( int iPartStart, int iPartEnd, Params& params );

    inline double dist2( int iPart ) {
    double dist(0.);
    for ( int iDim = 0 ; iDim < Position.size() ; iDim++ ) {
        double delta = position(iDim,iPart)-position_old(iDim,iPart);
        dist += delta*delta;
    }
    return dist;
    }
    inline double dist( int iPart, int iDim ) {
    double delta = abs( position(iDim,iPart)-position_old(iDim,iPart) );
    return delta;
    }

    Particle operator()(int iPart);


private:

};



#endif
