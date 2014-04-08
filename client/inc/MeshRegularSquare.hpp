#ifndef MESHREGULARSQUARE_HPP
#define MESHREGULARSQUARE_HPP

#include <cstddef>
#include <vector>
#include <cmath>

#include "MeshNatural.hpp"
#include "MeshInterface.hpp"

template <size_t dim>
class MeshRegularSquare;

/**
 * \brief type traits for Regular Square mesh.
 */
template <size_t dim>
struct MeshImpl_traits<MeshRegularSquare<dim> > {

};

/**
 * \brief   Regular mesh with square base element.
 *
 * \cond PRIVATE
 * Later on, it'd be nice to implement custom iterators instead
 * of having to populate some internal std::vector. After all,
 * (x,y) positions are very easy to calculate on-the-fly.
 */
template <size_t dim>
class MeshRegularSquare : public MeshInterface<MeshRegularSquare<dim>,dim>
{
  friend class MeshInterface<MeshRegularSquare<dim>,dim>;
  typedef MeshInterface<MeshRegularSquare<dim>,dim> interface_type;
  
  public:
    INJECT_MESH_INTERFACE_TYPES(MeshRegularSquare<dim> COMMA dim)

    /**
     * \brief   Constructor for a regular square mesh spanning a mesh of sensors -- from a "Natural
     *          Mesh" instance.
     * \param   natural_mesh
     * \param   d               delta, distance in x/y direction between two neighbouring nodes
     *
     * This is merely a delegating constructor.
     */
    template <class TMesh>
    MeshRegularSquare(const TMesh& base_mesh, double d)
      : MeshRegularSquare(
          base_mesh.minX(), 
          base_mesh.minY(),
          base_mesh.maxX(),
          base_mesh.maxY(),
          d
        )
    {
    }
    
    /**
     * \brief   Constructor for a regular square mesh going from x0 to x1 and from y0 to y1
     * \param   x0    origin of the mesh (x direction)
     * \param   y0    origin of the mesh (y direction)
     * \param   x1    end of the mesh (x direction)
     * \param   y1    end of the mesh (y direction)
     * \param   d     delta, distance in x/y direction between two neighbouring nodes
     *
     * The constructed mesh is guaranteed to contain the (x0,y0) and (x1,y1) points. In extreme
     * case, both of them might be on the boundaries of the mesh (if (x1-x0)/d or (y1-y0)/d is a
     * natural number). Otherwise, the rectangular mesh will be placed symmetrically, i.e. it will
     * extend the same distance "before" x0/y0 and "after" x1/y1.
     */
    MeshRegularSquare(double x0, double y0, double x1, double y1, double d)
      : interface_type(calculate_no_nodes(x0, y0, x1, y1, d)), d_(d)
    {
      const size_t nx       = calculate_no_nodes_1D(x0, x1, d);
      const double diff_x   = nx * d - (x1 - x0);
      const double x_origin = x0 - diff_x / 2.;

      const size_t ny       = calculate_no_nodes_1D(y0, y1, d);
      const double diff_y   = ny * d - (y1 - y0);
      const double y_origin = y0 - diff_y / 2.;

      size_t in = 0;
      double x = x_origin;
      for (size_t ix = 0; ix < nx; ++ix) {
        double y = y_origin;
        for (size_t iy = 0; iy < ny; ++iy) {
          this->node(in).x = x;
          this->node(in).y = y;
          y += d;
          ++in;
        }
        x += d;
      }
    }

  protected:
    const double impl_node_area(size_t i) const
    {
      return d_*d_;
    }

  private:
    /**
     * \brief   "delta", distance between two neighbouring nodes in x or y direction (square base)
     */
    double d_;

    /**
     * \brief   How many nodes in total. The mesh is supposed to contain the x0,x1,y0,y1 points
     */
    static size_t calculate_no_nodes(double x0, double y0, double x1, double y1, double d) {
      if (!(x1 > x0)) throw std::runtime_error("Error constructing regular square mesh, !(x1 > x0)");
      if (!(y1 > y0)) throw std::runtime_error("Error constructing regular square mesh, !(y1 > y0)");
      return calculate_no_nodes_1D(x0, x1, d) * calculate_no_nodes_1D(y0, y1, d);
    };

    /**
     * \brief   ceil((t1-t0)/d) ; no of nodes that wrap t0 and t1, with d distance between each pair
     */
    static size_t calculate_no_nodes_1D(double t0, double t1, double d) {
      if (!(t1 > t0)) throw std::runtime_error("Error constructing regular square mesh, !(t1 > t0)");
      if (!(d > 0)) throw std::runtime_error("Error constructing regular square mesh, !(d > 0)");
      return std::ceil((t1-t0)/d);
    };
};
#endif /* MESHREGULARSQUARE_HPP */
