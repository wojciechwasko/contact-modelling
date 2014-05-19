#ifndef CELL_HPP
#define CELL_HPP

/**
 * \file
 * \brief   Definition of GridCell
 */

namespace cm {

/**
 * \brief   A "POD" struct to keep x,y coordinates of a cell in the grid
 */
struct GridCell {
  double x;
  double y;
};

} /* namespace cm */

#endif /* CELL_HPP */
