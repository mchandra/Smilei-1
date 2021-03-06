/*

Collisions class - Frederic Perez - 03/2015

This is based on the work described here
http://dx.doi.org/10.1063/1.4742167

Binary collisions, between macro-particles, are treated according to a scheme
first described by Nanbu (http://dx.doi.org/10.1103/PhysRevE.55.4642).

To include collisions in the simulations, add a block in the input file, 
similar to the following:

Collisions(
	species1 = ["ion1"],
	species2 = ["electron1"],
	coulomb_log = 2.0
)

Several collision types can be defined. For each type, add a group "Collisions()".

*/

#ifndef COLLISIONS_H
#define COLLISIONS_H

#include <vector>

#include "Tools.h"
#include "Params.h"
#include "Species.h"
#include "CollisionalIonization.h"
#include "H5.h"

class Patch;

class Collisions
{

public:
    //! Constructor for Collisions between two species
    Collisions(Patch*, unsigned int, std::vector<unsigned int>, std::vector<unsigned int>, double, bool, int, unsigned int, int, bool, int, double);
    //! Cloning Constructor
    Collisions(Collisions*, int);
    //! destructor
    ~Collisions();
    
    void createTimestep(int timestep);
    
    //! Method that creates a vector of Collisions objects: one for each group in the input file.
    static std::vector<Collisions*> create(Params&, Patch*, std::vector<Species*>&);
    //! Method that clones a vector of Collisions objects
    static std::vector<Collisions*> clone(std::vector<Collisions*>, Params&);
    
    //! Identification number of the Collisions object
    int n_collisions;
    
    //! Group of the species numbers that are associated for Collisions.
    std::vector<unsigned int> species_group1, species_group2;
    
    //! Coulomb logarithm (zero or negative means automatic)
    double coulomb_log;
    
    //! True if collisions inside a group of species, False if collisions between different groups of species
    bool intra_collisions;
    
    //! Number of timesteps between each dump of collisions debugging
    int debug_every;
    
    //! Method to calculate the Debye length in each cluster
    static void calculate_debye_length(Params&, std::vector<Species*>& vecSpecies);
    
    //! is true if any of the collisions objects need automatically-computed coulomb log
    static bool debye_length_required;
    
    //! Method called in the main smilei loop to apply collisions at each timestep
    void collide(Params&, Patch* ,int);
    
    //! CollisionalIonization object, created if ionization required
    CollisionalIonization * Ionization;
    
private:
    
    //! Contains the debye length in each cluster, computed each timestep
    static std::vector<double> debye_length_squared; 
    
    static double cos_chi(double);
    
    int atomic_number;
    
    //! Hdf5 file name
    std::string filename;
    //! Hdf5 file access
    hid_t file_access;
};


#endif
