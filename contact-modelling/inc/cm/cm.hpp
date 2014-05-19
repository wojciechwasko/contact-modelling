#ifndef CM_HPP
#define CM_HPP
/**
 * \file
 * A convenience header which includes all the possible headers (bar the implementation details, namely the 'details' namespace) of the Contact-modelling library.
 */

// algorithms
#include "cm/algorithm/interface.hpp"
#include "cm/algorithm/displacements_to_forces.hpp"
#include "cm/algorithm/displacements_to_nonnegative_normal_forces.hpp"
#include "cm/algorithm/displacements_to_nonnegative_pressures.hpp"
#include "cm/algorithm/displacements_to_pressures.hpp"
#include "cm/algorithm/forces_to_displacements.hpp"
#include "cm/algorithm/pressures_to_displacements.hpp"

// interpolators
#include "cm/interpolator/interface.hpp"
#include "cm/interpolator/linear_delaunay.hpp"

// logging
#include "cm/log/log.hpp"

// grids
#include "cm/grid/cell.hpp"
#include "cm/grid/cell_shapes.hpp"
#include "cm/grid/grid.hpp"
#include "cm/grid/plot.hpp"

// grides - other files
#include "cm/grid/cell.hpp"
#include "cm/grid/plot.hpp"

// skin attributes
#include "cm/skin/attributes.hpp"

// skin providers
#include "cm/skin_provider/interface.hpp"
#include "cm/skin_provider/yaml.hpp"

/**
 * \brief   Main namespace for the contact-modelling library
 */
namespace cm {

}

#endif /* CM_HPP */
