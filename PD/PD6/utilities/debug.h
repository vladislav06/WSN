//
// Created by vm on 25.30.4.
//

#ifndef PD6_DEBUG_H
#define PD6_DEBUG_H
#ifdef DEBUG
#define DEB(...) PRINTF(__VA_ARGS__)
#else
#define DEB(...)
#endif


#endif //PD6_DEBUG_H
