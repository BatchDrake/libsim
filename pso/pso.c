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
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include <math.h>

#include "util.h"
#include "vector.h"

#include "pso.h"

static inline void
__pso_random_vector (vector_t *vec)
{
  int i;
  
  for (i = 0; i < vec->dimension; i++)
    vec->coef[i] = (rand () / (double) RAND_MAX);
}

static inline void
__pso_random_position_vector (pso_problem_t *problem, vector_t *vec)
{
  int i;
  __pso_random_vector (vec);
  
  for (i = 0; i < vec->dimension; i++)
    vec->coef[i] = vec->coef[i] *
      (problem->pos_max - problem->pos_min) + problem->pos_min;
}

static inline void
__pso_random_velocity_vector (pso_problem_t *problem, vector_t *vec)
{
  int i;
  __pso_random_vector (vec);
  
  for (i = 0; i < vec->dimension; i++)
    vec->coef[i] = vec->coef[i] *
      (problem->vel_max - problem->vel_min) + problem->vel_min;
}

static pso_particle_t *
pso_particle_new (pso_problem_t *problem)
{
  pso_particle_t *new;
  
  
  new = xmalloc (sizeof (pso_particle_t));
  
  vector_init (&new->position, problem->dimension);
  vector_init (&new->velocity, problem->dimension);
  
  __pso_random_position_vector (problem, &new->position);
  __pso_random_velocity_vector (problem, &new->velocity);
  
  vector_init (&new->best_local_position, problem->dimension);
  vector_copy (&new->best_local_position, &new->position);
  
  new->best_local_value = new->value = 
    (problem->fitness_function) (problem, &new->position);
    
  return new;
}

static pso_particle_t *
pso_get_best_particle (pso_problem_t *problem)
{
  int i;
  double best_value;
  pso_particle_t *best;
  
  best_value = 0;
  
  for (i = 0; i < problem->particle_count; i++)
    if (problem->particle_list[i]->value > best_value)
    {
      best = problem->particle_list[i];
      best_value = best->value;
    }
    
  return best;
}

static inline void
got_better (pso_problem_t *problem)
{
  struct timeval end;
  
  gettimeofday (&end, NULL);
  
  problem->when = (end.tv_sec - problem->start.tv_sec) + 
      (end.tv_usec - problem->start.tv_usec) / 1000000.0;
      
  if (!problem->quiet)
  {
    printf ("%-10lg: ", problem->when);
    vector_print (stdout, &problem->best_global_position);
    printf (", b: %lg\n", problem->best_global_value);
  }
}

pso_problem_t *
pso_problem_new (int particles, int dimension,
                 double (*fitness_function) (const struct pso_problem *, 
                                             vector_t *),
                 double pos_min, double pos_max,
                 double vel_min, double vel_max)
{
  pso_problem_t *new;
  pso_particle_t *best;
  
  int i;
  
  srand (time (NULL));
  
  new = xmalloc (sizeof (pso_problem_t));
  
  memset (new, 0, sizeof (pso_problem_t));
  
  new->fitness_function  = fitness_function;
  new->pos_min           = pos_min;
  new->pos_max           = pos_max;
  new->vel_min           = vel_min;
  new->vel_max           = vel_max;
  new->dimension         = dimension;
  
  gettimeofday (&new->start, NULL);
  
  new->inertia           = INERTIA_UPPER_BOUND;
  new->global_delta      = DEFAULT_GLOBAL_DELTA;
  new->local_delta       = DEFAULT_LOCAL_DELTA;
  
  new->particle_list     = xmalloc (sizeof (pso_problem_t *) * particles);
  new->particle_count    = particles;
  
  for (i = 0; i < particles; i++)
    new->particle_list[i] = pso_particle_new (new);
    
  vector_init (&new->best_global_position, new->dimension);
  
  vector_init (&new->temp_1, new->dimension);
  vector_init (&new->temp_2, new->dimension);
  
  best = pso_get_best_particle (new);
  
  vector_copy (&new->best_global_position, &best->position);
  new->best_global_value = best->value;
  
  return new;
}

pso_problem_t *
binary_pso_problem_new (int particles, int dimension,
                 double (*fitness_function) (const struct pso_problem *, 
                                             vector_t *),
                 double vel_min, double vel_max)
{
  pso_problem_t *new;
  int i, j;
  
  new = pso_problem_new (particles, dimension, fitness_function,
                         -1.0, 1.0, vel_min, vel_max);
                         
  for (i = 0; i < particles; i++)
    for (j = 0; j < dimension; j++)
      new->particle_list[i]->position.coef[j] = 
        (double) (new->particle_list[i]->position.coef[j] > 0);

  return new;
}

static inline void
pso_accel_simple (pso_problem_t *problem, pso_particle_t *this)
{
  vector_mul_by_const (&this->velocity,       /* Intertia Weight */
      problem->inertia);
      
  vector_sub_to (&problem->temp_1, 
    &problem->best_global_position,           /* Personal influence */
    &this->position);
    
  vector_sub_to (&problem->temp_2, 
    &this->best_local_position,
    &this->position);                         /* Social influence */
    
  vector_mul_by_const (&problem->temp_1,      /* Some ponderation here */
    (rand () / (double) RAND_MAX) * problem->global_delta);
    
  vector_mul_by_const (&problem->temp_2,      /* And some ponderation more */
    (rand () / (double) RAND_MAX) * problem->local_delta);
  
  
  vector_add (&this->velocity, &problem->temp_1); /* Personal influence */
  vector_add (&this->velocity, &problem->temp_2); /* Social influence */
}

static inline void
pso_move (pso_problem_t *problem, pso_particle_t *this)
{
  if (this->value > this->best_local_value)
  {
    this->best_local_value = this->value;
    vector_copy (&this->best_local_position, &this->position);
    
    if (this->value > problem->best_global_value)
    {
      problem->best_global_value = this->value;
      vector_copy (&problem->best_global_position, &this->position);
      
      got_better (problem);
    }
  }
  
  /* Recalculate position */
  vector_add (&this->position,
              &this->velocity);
              
  /* Value here? */
  this->value = (problem->fitness_function) (problem, &this->position);   
}


static inline void
pso_move_discrete (pso_problem_t *problem, pso_particle_t *this)
{
  int i;
  
  if (this->value > this->best_local_value)
  {
    this->best_local_value = this->value;
    vector_copy (&this->best_local_position, &this->position);
    
    if (this->value > problem->best_global_value)
    {
      problem->best_global_value = this->value;
      vector_copy (&problem->best_global_position, &this->position);
      
      got_better (problem);
    }
  }
  
  /* Discrete position update according to the algorithm described in
     http://tracer.uc3m.es/tws/pso/basics.html */
     
  for (i = 0; i < problem->dimension; i++)
    if ((double) rand () / (double) RAND_MAX < 
      (1.0 / (1.0 + exp (-this->velocity.coef[i]))))
      this->position.coef[i] = 1.0;
    else
      this->position.coef[i] = 0.0;
      
  /* Value here? */
  this->value = (problem->fitness_function) (problem, &this->position);   
}

void
pso_evolve (pso_problem_t *problem)
{
  int i;
  pso_particle_t *this;
  
  for (i = 0; i < problem->particle_count; i++)
  {
    this = problem->particle_list[i];

    pso_accel_simple (problem, this);
    pso_move (problem, this);
  }
  
  if (problem->inertia > INERTIA_LOWER_BOUND)
    problem->inertia -= INERTIA_DELTA;
    
  problem->t++;
}

void
binary_pso_evolve (pso_problem_t *problem)
{
  int i;
  pso_particle_t *this;
  
  for (i = 0; i < problem->particle_count; i++)
  {
    this = problem->particle_list[i];

    pso_accel_simple (problem, this);
    pso_move_discrete (problem, this);
  }
  
  if (problem->inertia > INERTIA_LOWER_BOUND)
    problem->inertia -= INERTIA_DELTA;
    
  problem->t++;
}


void
pso_get_best (pso_problem_t *problem, vector_t *dest)
{
  vector_copy (dest, &problem->best_global_position);
}



