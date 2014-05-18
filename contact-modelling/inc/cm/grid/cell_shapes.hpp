#ifndef CELL_SHAPES_HPP
#define CELL_SHAPES_HPP

/**
 * \file
 * \brief   All shapes for a grid's cells.
 */

namespace cm {

/**
 * \brief   A base shape class, defining the interface for shapes to implement.
 */
class GridCellShape {
public:
  /**
   * \brief   Return dimension in x direction. May be an approximation.
   *
   * If the shape is not a rectangle, return an approximation (based on a
   * square which would cover the same area.
   */
  virtual double dx()   const = 0;
  /**
   * \brief   Return dimension in y direction. May be an approximation.
   * \sa dx()
   */
  virtual double dy()   const = 0;
  /**
   * \brief   Return radius of the circle. May be an approximation.
   *
   * If the shape is not a circle, return an approximation (based on a
   * circle which would cover the same area.
   */
  virtual double  r()   const = 0;
  /**
   * \brief   Return area of the shape.
   *
   */
  virtual double area() const = 0;
  /**
   * \brief   Return a new instance of the same shape
   */
  virtual GridCellShape* clone() const = 0;
  /**
   * \brief   Whether the shape is a circle
   */
  virtual bool isCircular()    const { return false; }
  /**
   * \brief   Whether the shape is a rectangle
   */
  virtual bool isRectangular() const { return false; }
  /**
   * \brief   Whether the shape is a square
   */
  virtual bool isSquare()      const { return false; }

  virtual ~GridCellShape();
};

/**
 * \brief   A rectangle, duh.
 *
 * Sides of the rectangle are aligned with the (x,y) axes.
 */
class Rectangle : public GridCellShape {
  /**
   * \brief   Length of the x side of the rectangle
   */
  double dx_;
  /**
   * \brief   Length of the y side of the rectangle
   */
  double dy_;
  /**
   * \brief   Precomputed approximation of the radius
   */
  double r_approx_;

public:
  /**
   * \brief   Construct from two lengths of the sides
   * \param   dx  length of the x side of the rectangle
   * \param   dy  length of the y side of the rectangle
   */
  Rectangle(const double dx, const double dy);
  double   dx() const final;
  double   dy() const final;
  /**
   * \brief   Approximation via a circle of the same area
   */
  double    r() const final;
  double area() const final;
  GridCellShape* clone() const;
  bool isCircular()    const final  { return false; }
  bool isRectangular() const final  { return true;  }
  bool isSquare()      const        { return false; }
};

/**
 * \brief   A square, as in: Rectangle with dx == dy.
 *
 * Sides of the square are aligned with the (x,y) axes.
 */
class Square : public Rectangle {
public:
  /**
   * \brief   Construct from the lenght of the sides.
   * \param   d   Length of each side
   */
  Square(const double d);
  GridCellShape* clone() const;

  bool isSquare()     const final   { return true; }
};

/**
 * \brief   A circle.
 */
class Circle : public GridCellShape {
  /**
   * \brief   Precomputed approximation of the x side (dx == dy)
   */
  double dx_approx_;
  /**
   * \brief   Precomputed approximation of the y side (dx == dy)
   */
  double dy_approx_;
  /**
   * \brief   Radius of the circle.
   */
  double r_;

public:
  /**
   * \brief   Construct from the radius of the circle
   * \param   r   Radius of the circle
   */
  Circle(const double r);
  /**
   * \brief   Approximation via a square of the same area
   */
  double   dx() const final;
  /**
   * \brief   Approximation via a square of the same area
   */
  double   dy() const final;
  double    r() const final;
  double area() const final;
  GridCellShape* clone() const;

  bool isCircular()    const final  { return true;  }
  bool isRectangular() const final  { return false; }
  bool isSquare()      const final  { return false; }
};

} /* namespace cm */

#endif /* CELL_SHAPES_HPP */
