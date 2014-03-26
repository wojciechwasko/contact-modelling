#ifndef MESHREGULARSQUARE_HPP
#define MESHREGULARSQUARE_HPP

#include <cstddef>
#include <vector>
#include <cmath>

#include "MeshNatural.hpp"
#include "MeshInterface.hpp"

/**
 * \brief   Regular mesh with square base element.
 *
 * \cond PRIVATE
 * Later on, it'd be nice to implement custom iterators instead
 * of having to populate some internal std::vector. After all,
 * (x,y) positions are very easy to calculate on-the-fly.
 */
template <class TNode>
class MeshRegularSquare : public MeshInterface<MeshRegularSquare<TNode> >
{
  friend class MeshInterface<MeshRegularSquare<TNode> >;
  typedef MeshInterface<MeshRegularSquare<TNode> > interface_type;
  
  public:
    INJECT_MESH_TRAITS_TYPEDEFS(MeshRegularSquare<TNode>)

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
      : interface_type(calculate_no_nodes(x0, y0, x1, y1, d))
    {
      nodes.reserve(calculate_no_nodes(x0,y0,x1,y1,d));

      const size_t nx       = calculate_no_nodes_1D(x0, x1, d);
      const double diff_x   = nx * d - (x1 - x0);
      const double x_origin = x0 - diff_x / 2.;

      const size_t ny       = calculate_no_nodes_1D(y0, y1, d);
      const double diff_y   = ny * d - (y1 - y0);
      const double y_origin = y0 - diff_y / 2.;

      size_t v_ind = 0;
      double y = y_origin;
      for (size_t iy = 0; iy < ny; ++iy) {
        double x = x_origin;
        for (size_t ix = 0; ix < nx; ++ix) {
          node_type n;
          n.x = x;
          n.y = y;
          x += d;
          for (size_t i = 0; i < n.val_dimensionality; ++i) {
            n.vals[i] = &(this->values_[v_ind]);
            ++v_ind;
          }
          nodes.push_back(n);
        }
        y += d;
      }
    }

  private:
    container_type nodes;
    iterator impl_begin() { return nodes.begin(); };
    iterator impl_end()   { return nodes.end();   };
    const_iterator impl_cbegin() const { return nodes.cbegin(); };
    const_iterator impl_cend()   const { return nodes.cend();   };
    reference       impl_ra_nobounds(size_t n)       { return nodes[n]; };
    const_reference impl_ra_nobounds(size_t n) const { return nodes[n]; };

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

/**
 * \brief type traits for Regular Square mesh.
 *
 * \note Sorry for the weird naming, but 14.6.1/7:
 * > A template-parameter shall not be redeclared within its scope
 * > (including nested scopes). A template-parameter shall not have
 * > the same name as the template name.
 */
template <class TNode>
struct MeshImpl_traits<MeshRegularSquare<TNode> > {
  typedef TNode                                  node_type;
  typedef node_type      &                       reference;
  typedef node_type const&                 const_reference;
  typedef typename std::vector<node_type>            container_type;
  typedef container_type      &        container_reference;
  typedef container_type const&  container_const_reference;
  typedef typename container_type::iterator                iterator;
  typedef typename container_type::const_iterator    const_iterator;
};

#endif /* MESHREGULARSQUARE_HPP */
