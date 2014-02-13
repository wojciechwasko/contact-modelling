#ifndef MAIN_HPP
#define MAIN_HPP

#include "skin_object.h"
#include "skin_reader.h"

/**
 * \return  false on success, true on failure.
 *
 * Messages are written to std::cout/cerr and the streams are flushed.
 */
bool init_skinware(skin_object* so, skin_reader* sr);

#endif /* MAIN_HPP */
