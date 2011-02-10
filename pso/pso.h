/*
 *    <one line to give the program's name and a brief idea of what it does.>
 *    Copyright (C) <year>  <name of author>
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>
 */
    
#ifndef _PSO_H
#define _PSO_H

#include "../common/util.h"
#include "../common/vector.h"

#include <sys/time.h>

#define INERTIA_UPPER_BOUND  1.0
#define INERTIA_LOWER_BOUND  0.4

#define INERTIA_ITERATIONS   1000
#define INERTIA_DELTA        ((INERTIA_UPPER_BOUND - INERTIA_LOWER_BOUND) / INERTIA_ITERATIONS)

#define DEFAULT_GLOBAL_DELTA 0.9
#define DEFAULT_LOCAL_DELTA  0.5


struct pso_particle
{
  vector_t position;
  vector_t velocity;
  
  double  value;
  
  double  best_local_value;
  vector_t best_local_position;   
};

struct pso_problem
{
  struct pso_particle ** particle_list;
  int                    particle_count;
  
  int                    t;
  int                    dimension;
  struct timeval         start;
  int                    quiet;
  double (*fitness_function) (const struct pso_problem *, vector_t *);
  
  double pos_min, pos_max;
  double vel_min, vel_max;
  
  double inertia;
  double global_delta;
  double local_delta;
  double best_global_value;
  
  double when;
  vector_t best_global_position; 
  
  vector_t temp_1, temp_2;
};

typedef struct pso_problem pso_problem_t;
typedef struct pso_particle pso_particle_t;

pso_problem_t *
pso_problem_new (int, int, 
                 double (*) (const struct pso_problem *, vector_t *),
                 double, double,
                 double, double);

pso_problem_t *
binary_pso_problem_new (int, int,
                 double (*) (const struct pso_problem *, vector_t *),
                 double, double);
                                  
void pso_evolve (pso_problem_t *);
void binary_pso_evolve (pso_problem_t *);

void pso_get_best (pso_problem_t *, vector_t *);

#endif /* _PSO_H */

