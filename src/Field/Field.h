#ifndef FIELD_H
#define FIELD_H

#include <cmath>

#include <vector>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>

#include "Tools.h"
#include "SpeciesMPI.h"

//! Structure containing the fields at a given position (e.g. at a Particle position)
struct LocalFields
{
    //! value of the field component along the x-direction
    double x;
    //! value of the field component along the y-direction
    double y;
    //! value of the field component along the z-direction
    double z;

};

//! Class Field: generic class allowing to define vectors
class Field
{
public:
    SpeciesMPI specMPI;

    //! name of the field
    std::string name;

    //! Constructor for Field: with no input argument
    Field() {
      specMPI.init();
    };

    //! Constructor for Field: with the Field dimensions as input argument
    Field( std::vector<unsigned int> dims ) {
        specMPI.init();
    };
    //! Constructor, isPrimal define if mainDim is Primal or Dual
    Field( std::vector<unsigned int> dims, unsigned int mainDim, bool isPrimal ) {
        specMPI.init();
    };

    //! Constructor for Field: with the Field dimensions and dump file name as input argument
    Field( std::vector<unsigned int> dims, std::string name_in ) : name(name_in) {
        specMPI.init();
    } ;
    
    //! Constructor for Field: isPrimal define if mainDim is Primal or Dual
    Field( std::vector<unsigned int> dims, unsigned int mainDim, bool isPrimal, std::string name_in ) : name(name_in) {
       specMPI.init() ;
    } ;

    //! Destructor for Field
    virtual ~Field() {
        ;
    } ;

    //! Virtual method used to allocate Field
    virtual void allocateDims(std::vector<unsigned int> dims) = 0;
    virtual void deallocateDims() = 0;
    
    //! Virtual method used to allocate Field, isPrimal define if mainDim is Primal or Dual
    virtual void allocateDims(std::vector<unsigned int> dims, unsigned int mainDim, bool isPrimal) = 0;

    //! Virtual method used to make a dump of the Field data
    virtual void dump(std::vector<unsigned int> dims) = 0;

    //! Virtual method to shift field in space
    virtual void shift_x(unsigned int delta) = 0;

    //! vector containing the dimensions of the Field
    //! \todo private/friend/modify (JD)
    std::vector<unsigned int> dims_;
    
    //! keep track ofwich direction of the Field is dual
    std::vector<unsigned int> isDual_;

    //! Return 0 if direction i is primal, 1 if dual
    inline unsigned int isDual(unsigned int i) {
        if (i<dims_.size())
            return isDual_[i];
        else
            return 0;
    }

    //! returns the dimension of the Field
	inline std::vector<unsigned int> dims () {return dims_;}
    //! All arrays may be viewed as a 1D array
    //! Linearized diags
    unsigned int globalDims_;
    //! pointer to the linearized array
    double* data_;
    
    inline double* data() {return data_;}
    //! reference access to the linearized array (with check in DEBUG mode)
    inline double& operator () (unsigned int i)
    {
        DEBUGEXEC(if (i>=globalDims_) ERROR(name << " Out of limits "<< i << " < " <<dims_[0] ));
        DEBUGEXEC(if (!std::isfinite(data_[i])) ERROR(name << " Not finite "<< i << " = " << data_[i]));
        return data_[i];
    };
    //! access to the linearized array (with check in DEBUG mode)
    inline double operator () (unsigned int i) const
    {
        DEBUGEXEC(if (i>=globalDims_) ERROR(name << " Out of limits "<< i));
        DEBUGEXEC(if (!std::isfinite(data_[i])) ERROR(name << " Not finite "<< i << " = " << data_[i]));
        return data_[i];
    };
    //! method used to put all entry of a field at a given value val
    inline void put_to(double val)
    {
        if (data_)
            for (unsigned int i=0; i<globalDims_; i++) data_[i] = val;
    }
    
    //! method used to put all entry of a field at a given value val
    inline void multiply(double val)
    {
        if (data_)
            for (unsigned int i=0; i<globalDims_; i++) data_[i] *= val;
    }
    

    //! 2D reference access to the linearized array (with check in DEBUG mode)
    inline double& operator () (unsigned int i,unsigned int j)
    {
	int unsigned idx = i*dims_[1]+j;
        DEBUGEXEC(if (idx>=globalDims_) ERROR("Out of limits & "<< i << " " << j));
        DEBUGEXEC(if (!std::isfinite(data_[idx])) ERROR("Not finite "<< i << " " << j << " = " << data_[idx]));
        return data_[idx];
    };
    //! 2D access to the linearized array (with check in DEBUG mode)
    inline double operator () (unsigned int i, unsigned int j) const
    {
        unsigned int idx = i*dims_[1]+j;
        DEBUGEXEC(if (idx>=globalDims_) ERROR("Out of limits "<< i << " " << j));
        DEBUGEXEC(if (!std::isfinite(data_[idx])) ERROR("Not finite "<< i << " " << j << " = " << data_[idx]));
        return data_[idx];
    };

    virtual double norm2(unsigned int istart[3][2], unsigned int bufsize[3][2]) = 0;

    inline long double norm() {
        long double sum(0.);
        for (unsigned int i=0;i<globalDims_;i++) sum+= data_[i]*data_[i];
        return sum;
    }
    
    inline void copyFrom(Field *from_field) {
        DEBUGEXEC(if (globalDims_!=from_field->globalDims_) ERROR("Field size do not match "<< name << " " << from_field->name));
        for (unsigned int i=0;i< globalDims_; i++) {
            (*this)(i)=(*from_field)(i);
        }
    }


protected:

private:

};

#endif
