
// -------------------
// Some HDF5 overlays
// -------------------

#ifndef H5_H
#define H5_H

#include <hdf5.h>
#include <string>

//! HDF5 help functions
class H5 {
    
    public:
    
    //! Make an empty group
    // Returns the group ID
    static hid_t group(hid_t locationId, std::string group_name) {
        
        return H5Gcreate(locationId, group_name.c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    }
    
    //! write a string as an attribute
    static void attr(hid_t locationId, std::string attribute_name, std::string attribute_value) {
        hid_t atype = H5Tcopy(H5T_C_S1);
        H5Tset_size(atype, attribute_value.size());
        H5Tset_strpad(atype,H5T_STR_NULLTERM);
        
        attr(locationId, attribute_name, *(attribute_value.c_str()), atype);
        
        H5Tclose(atype);
    }
        
    //! write an unsigned int as an attribute
    static void attr(hid_t locationId, std::string attribute_name, unsigned int attribute_value) {
        attr(locationId, attribute_name, attribute_value, H5T_NATIVE_UINT);}
    
    //! write size_t as an attribute
    static void attr(hid_t locationId, std::string attribute_name, size_t attribute_value) {
        attr(locationId, attribute_name, (unsigned int) attribute_value);}
    
    //! write an int as an attribute
    static void attr(hid_t locationId, std::string attribute_name, int attribute_value) {
        attr(locationId, attribute_name, attribute_value, H5T_NATIVE_INT);}
    
    //! write a double as an attribute
    static void attr(hid_t locationId, std::string attribute_name, double attribute_value) {
        attr(locationId, attribute_name, attribute_value, H5T_NATIVE_DOUBLE);}
    
    //! write anything as an attribute
    template<class T>
    static void attr(hid_t locationId, std::string attribute_name, T & attribute_value, hid_t type) {
        hid_t sid = H5Screate(H5S_SCALAR);
        hid_t aid = H5Acreate(locationId, attribute_name.c_str(), type, sid, H5P_DEFAULT, H5P_DEFAULT);
        H5Awrite(aid, type, &attribute_value);
        H5Sclose(sid);
        H5Aclose(aid);
    }
    
    
    //! write a vector<anything> as an attribute
    template<class T>
    static void attr(hid_t locationId, std::string attribute_name, std::vector<T>& attribute_value, hid_t type) {
        hsize_t dims = attribute_value.size();
        hid_t sid = H5Screate_simple(1, &dims, NULL);
        hid_t aid = H5Acreate (locationId, attribute_name.c_str(), type, sid, H5P_DEFAULT, H5P_DEFAULT);
        H5Awrite(aid, type, &(attribute_value[0]));
        H5Aclose(aid);
        H5Sclose(sid);        
    }
    
    //! write an vector<unsigned int> as an attribute
    static void attr(hid_t locationId, std::string attribute_name, std::vector<unsigned int> attribute_value) {
        attr(locationId, attribute_name, attribute_value, H5T_NATIVE_UINT);
    }
    
    //! write an vector<double> as an attribute
    static void attr(hid_t locationId, std::string attribute_name, std::vector<double> attribute_value) {
        attr(locationId, attribute_name, attribute_value, H5T_NATIVE_DOUBLE);
    }
    
    
    //READ ATTRIBUTES
    
    //! retrieve a double attribute
    static void getAttr(hid_t locationId, std::string attribute_name, double &attribute_value) {
        getAttr(locationId, attribute_name, attribute_value, H5T_NATIVE_DOUBLE);
    }
    
    //! retrieve a unsigned int attribute
    static void getAttr(hid_t locationId, std::string attribute_name, unsigned int &attribute_value) {
        getAttr(locationId, attribute_name, attribute_value, H5T_NATIVE_UINT);
    }
    
    //! retrieve a string attribute
    static void getAttr(hid_t locationId, std::string attribute_name, std::string &attribute_value) {
        if (H5Aexists(locationId,attribute_name.c_str())>0) {
            hid_t attr_id = H5Aopen_name(locationId, attribute_name.c_str());
            hid_t attr_type = H5Aget_type(attr_id);
            int sdim = H5Tget_size(attr_type);
            hid_t mem_type = H5Tcopy(H5T_C_S1);
            H5Tset_size(mem_type, sdim);
            std::vector<char> tmpchar(sdim);
            // line below would crash (don't know why)
            // char* tmpchar= new char(sdim);
            if (H5Aread(attr_id, mem_type, &tmpchar[0]) < 0) {
                WARNING("Can't read string "<< attribute_name);
            } else {
                attribute_value = std::string(tmpchar.begin(),tmpchar.end());
            }
            H5Tclose(mem_type);
            H5Tclose(attr_type);
            H5Aclose(attr_id);
        } else {
            WARNING("Cannot find attribute " << attribute_name);
        }
    }
    
    template<class T>
    static void getAttr(hid_t locationId, std::string attribute_name, T &attribute_value, hid_t type) {
        if (H5Aexists(locationId,attribute_name.c_str())>0) {
            hid_t aid = H5Aopen(locationId, attribute_name.c_str(), type);
            H5Aread(aid, type, &(attribute_value));
            H5Aclose(aid);
        } else {
            WARNING("Cannot find attribute " << attribute_name);
        }
    }
    //! write a vector of unsigned ints
    //! v is the vector
    //! size is the number of elements in the vector
    
    //! write a vector<int>
    static void vect(hid_t locationId, std::string name, std::vector<int> v, int deflate=0) {
        vect(locationId, name, v[0], v.size(), H5T_NATIVE_INT, deflate);
    }
    
    //! write a vector<unsigned int>
    static void vect(hid_t locationId, std::string name, std::vector<unsigned int> v, int deflate=0) {
        vect(locationId, name, v[0], v.size(), H5T_NATIVE_UINT, deflate);
    }
    
    //! write a vector<short>
    static void vect(hid_t locationId, std::string name, std::vector<short> v, int deflate=0) {
        vect(locationId, name, v[0], v.size(), H5T_NATIVE_SHORT, deflate);
    }
    
    //! write a vector<doubles>
    static void vect(hid_t locationId, std::string name, std::vector<double> v, int deflate=0) {
        vect(locationId, name, v[0], v.size(), H5T_NATIVE_DOUBLE, deflate);
    }
    
    
    //! write any vector
    //! type is the h5 type (H5T_NATIVE_DOUBLE, H5T_NATIVE_INT, etc.)
    template<class T>
    static void vect(hid_t locationId, std::string name, T & v, int size, hid_t type, int deflate=0) {
        // create dataspace for 1D array with good number of elements
        hsize_t dims = size;
        hid_t sid = H5Screate_simple(1, &dims, NULL);
        hid_t pid = H5Pcreate(H5P_DATASET_CREATE); // property list
        
        if (deflate>0) {
            H5Pset_chunk(pid, 1, &dims);
            H5Pset_deflate (pid, std::min(9,deflate));
        }
        
        // create dataset
        hid_t did = H5Dcreate(locationId, name.c_str(), type, sid, H5P_DEFAULT, pid, H5P_DEFAULT);
        // write vector in dataset
        H5Dwrite(did, type, sid, sid, H5P_DEFAULT, &v);
        // close all
        H5Dclose(did);
        H5Pclose(pid);
        H5Sclose(sid);
    }
    
    
    
    //! retrieve a double vector
    static void getVect(hid_t locationId, std::string vect_name,  std::vector<double> &vect) {
        getVect(locationId, vect_name, vect, H5T_NATIVE_DOUBLE);
    }
    
    //! retrieve an unsigned int vector
    static void getVect(hid_t locationId, std::string vect_name,  std::vector<unsigned int> &vect) {
        getVect(locationId, vect_name, vect, H5T_NATIVE_UINT);
    }
    
    //! retrieve a int vector
    static void getVect(hid_t locationId, std::string vect_name,  std::vector<int> &vect) {
        getVect(locationId, vect_name, vect, H5T_NATIVE_INT);
    }
    
    //! retrieve a short vector
    static void getVect(hid_t locationId, std::string vect_name,  std::vector<short> &vect) {
        getVect(locationId, vect_name, vect, H5T_NATIVE_SHORT);
    }
    
    //! template to read generic 1d vector
    template<class T>
    static void getVect(hid_t locationId, std::string vect_name, std::vector<T> &vect, hid_t type) {
        hid_t did = H5Dopen(locationId, vect_name.c_str(), H5P_DEFAULT);
        hid_t sid = H5Dget_space(did);
        int sdim = H5Sget_simple_extent_ndims(sid);
        if (sdim!=1) {
            ERROR("Reading vector " << vect_name << " is not 1D but " <<sdim << "D");
        }
        hsize_t dim[1];
        H5Sget_simple_extent_dims(sid,dim,NULL);
        if (dim[0] != vect.size()) {
            ERROR("Reading vector " << vect_name << " mismatch " << vect.size() << " != " << dim);
        }
        H5Sclose(sid);
        H5Dread(did, type, H5S_ALL, H5S_ALL, H5P_DEFAULT, &vect[0]);
        H5Dclose(did);
    }

    
    //! write a 2-D array of doubles in parallel (several MPI nodes)
    //! m is the matrix (2D array)
    //! sizex, sizey is the number of elements in both axes of the matrix
    //! offset is the x-location where the current node will start to write
    //! numel  is the x-number of elements for the current node
    
    static void matrix_MPI(hid_t locationId, std::string name, double& m,
                           int sizex, int sizey, int offset, int numel    ) {
        // Create a HDF5 memory space to hold the data
        hsize_t chunk_parts[2];
        chunk_parts[0] = numel;
        chunk_parts[1] = sizey;
        hid_t memspace = H5Screate_simple(2, chunk_parts, NULL);
        // Create the HDF5 filespace
        hsize_t dimsf[2];
        dimsf[1] = sizex;
        dimsf[0] = sizey;
        hid_t filespace = H5Screate_simple(2, dimsf, NULL);
        // Choose the hyperslab, which is the region where the current node will write
        hsize_t offs[2], stride[2], count[2], block[2];
        offs[1] = offset;
        offs[0] = 0;
        stride[0] = 1;
        stride[1] = 1;
        count[0] = 1;
        count[1] = 1;
        block[1] = numel;
        block[0] = sizey;
        H5Sselect_hyperslab(filespace, H5S_SELECT_SET, offs, stride, count, block);
        // Open the pre-existing group and write the data inside
        hid_t write_plist = H5Pcreate(H5P_DATASET_XFER);
        H5Pset_dxpl_mpio(write_plist, H5FD_MPIO_INDEPENDENT);
        hid_t plist_id = H5Pcreate(H5P_DATASET_CREATE);
        hid_t dset_id;
        htri_t status = H5Lexists( locationId, name.c_str(), H5P_DEFAULT ); 
        if (!status)
            dset_id  = H5Dcreate(locationId, name.c_str(), H5T_NATIVE_DOUBLE, filespace, H5P_DEFAULT, plist_id, H5P_DEFAULT);
        else
            dset_id = H5Dopen(locationId, name.c_str(), H5P_DEFAULT);
        H5Dwrite( dset_id, H5T_NATIVE_DOUBLE, memspace, filespace, write_plist, &m );
        // Close all
        H5Pclose(plist_id);
        H5Dclose(dset_id);
        H5Pclose( write_plist );
        H5Sclose(filespace);
        H5Sclose(memspace);
    }
    
    
    
    //! Write an N-D array of doubles in parallel (several MPI nodes)
    //! locationId : ID of the HDF5 location where to write the array
    //! name       : name to give to the new (big) array
    //! A          : the local (small) N-D array which will be inserted in A, at the right place
    //! total_size : a vector (length N) representing the N-D size of the big array
    //! local_size : a vector (length N) representing the N-D size of the local (small) array
    //! offset     : a vector (length N) representing the location where the current node will start to write
    
    static void array3D_MPI(hid_t locationId, std::string name, double& A,
                            std::vector<int> total_size,  std::vector<int> local_size, std::vector<unsigned int> offset ) {
        int dim = total_size.size(), i;
        // Create a HDF5 memory space to represent the local array shape
        hsize_t chunk_parts[dim];
        for(i=0; i<dim; i++) chunk_parts[i] = local_size[i];
        hid_t memspace = H5Screate_simple(dim, chunk_parts, NULL);
        // Create the HDF5 file space to represent the total array shape
        hsize_t dimsf[dim];
        for(i=0; i<dim; i++) dimsf[i] = total_size[i];
        hid_t filespace = H5Screate_simple(dim, dimsf, NULL);
        // Choose the hyperslab, which is the region where the current node will write
        hsize_t offs[dim], stride[dim], count[dim], block[dim];
        for(i=0; i<dim; i++) {
            offs[i] = offset[i];
            stride[i] = 1;
            count[i] = 1;
            block[i] = local_size[i];
        }
        H5Sselect_hyperslab(filespace, H5S_SELECT_SET, offs, stride, count, block);
        // Open the pre-existing group and write the data inside
        hid_t write_plist = H5Pcreate(H5P_DATASET_XFER);
        H5Pset_dxpl_mpio(write_plist, H5FD_MPIO_INDEPENDENT);
        hid_t plist_id = H5Pcreate(H5P_DATASET_CREATE);
        hid_t dset_id;
        htri_t status = H5Lexists( locationId, name.c_str(), H5P_DEFAULT ); 
        if (!status)
            dset_id  = H5Dcreate(locationId, name.c_str(), H5T_NATIVE_DOUBLE, filespace, H5P_DEFAULT, plist_id, H5P_DEFAULT);
        else
            dset_id = H5Dopen(locationId, name.c_str(), H5P_DEFAULT);
        H5Dwrite( dset_id, H5T_NATIVE_DOUBLE, memspace, filespace, write_plist, &A );
        // Close all
        H5Pclose(plist_id);
        H5Dclose(dset_id);
        H5Pclose( write_plist );
        H5Sclose(filespace);
        H5Sclose(memspace);
    }
    
    
};

#endif
