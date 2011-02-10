#ifndef _VECTOR_H
#define _VECTOR_H

#ifndef CACHE_LINE_SIZE
# define CACHE_LINE_SIZE 64 /* Cambiar en función de la CPU */
#endif

typedef double vectcoef_t;

struct vector
{
  int dimension;
  void *base;
  vectcoef_t *coef;
} __attribute__ ((aligned (CACHE_LINE_SIZE)));

typedef struct vector vector_t;

void vector_init (vector_t *, int);
void vector_free (vector_t *);
void vector_add (vector_t *, const vector_t *);
void vector_sub (vector_t *, const vector_t *);
void vector_sub_to (vector_t *, const vector_t *, const vector_t *);

void vector_add_ponderated (vector_t *, const vector_t *, vectcoef_t);
void vector_mul_by_const (vector_t *, vectcoef_t);
void vector_mul_by_const_to (vector_t *, const vector_t *, vectcoef_t);
void vector_zero (vector_t *);
void vector_copy (vector_t *, const vector_t *);

vectcoef_t vector_point_product (const vector_t *, const vector_t *);
void vector_cross_product (vector_t *, const vector_t *, const vector_t *);
void vector_cross_product_ponderated (vector_t *,
                                       const vector_t *,
                                       const vector_t *,
                                       vectcoef_t);
                                       
void vector_print (FILE *, vector_t *);
vectcoef_t vector_norm (const vector_t *);
vectcoef_t vector_eucdist (const vector_t *, const vector_t *);
#endif /* _VECTOR_H */

