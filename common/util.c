#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h> // strbuild()
#include <string.h> // strsep()
#include <unistd.h>

#include "util.h"

#define STRBUILD_BSIZ           16
#define DEBUG_BACKTRACE_NFUNCS  48

int saved_errno;

void errno_save ()
{
  saved_errno = errno;
}

void errno_restore ()
{
  errno = saved_errno;
}

/* Prototipos de funciones estaticas */
static void xalloc_die (void);


int
is_asciiz(const char *buf, int lbound, int ubound)
{
  register int i;

  for (i = lbound; i < ubound; i++)
    if (!buf[i])
      return i;
  return 0;
}

/* ESCRIBIRLA */
char*
vstrbuild (const char *fmt, va_list ap)
{
  char *out;
  void *p;
  int size, zeroindex;
  int last;

  last = 0;
  
  if (fmt != NULL)
  {
    if (!*fmt) /* Yo no hago trabajo extra */
    {
      out = xmalloc (1);
      out[0] = '\0';
      return out;
    }
      
    size = vsnprintf (NULL, 0, fmt, ap) + 1;
    out = xmalloc (size);
    vsnprintf (out, size, fmt, ap);
    
    for(;;)
    {
      
      if ((zeroindex = is_asciiz (out, last, size)) != 0)
      {

          /* Esto quiere decir que hemos encontrado un NUL en el
	     intervalo [last, size) */
          /* @zeroindex: El offset del cero en ese intervalo */

        if (zeroindex + 1 == size) /* Evitemos perogrulladas */
          return out;
          
        p = xrealloc (out, zeroindex + 1);
          
        return (char *) p;
      }
      
      /* Oh, algo ha ocurrido. No hay un cero en ese
	 intervalo. Tenemos que buscarlo */
      
      /* Estamos seguros de que en los intervalos anteriores NO hay
	 cero, as� que buscaremos a partir del size anterior */

      /* Incrementaremos el tama�o en STRBUILD_BSIZ bytes */
      last = size;
      size += STRBUILD_BSIZ;
      
      p = xrealloc (out, size); /* Reasignamos */
      
      out = (char *) p;
      vsnprintf (out, size, fmt, ap);
    }
  }
  else
    out = NULL;
  
  return out;
}


/* Construye una cadena mediante el formato printf y devuelve un
   puntero a la cadena resultado. DEBES liberar tu mismo la salida. */

/* FIXME: Buscar alguna alternativa mas portable */
char*
strbuild (const char *fmt, ...)
{
  char *out;
  va_list ap;

  va_start (ap, fmt);
  out =  vstrbuild (fmt, ap);
  va_end (ap);

  return out;
}

/* Wrapper para malloc que autocomprueba el valor de retorno */
void*
xmalloc (size_t size)
{
  void* m = malloc (size);

  if (m == NULL)
    xalloc_die ();
  
  return m;
}

/* Wrapper para realloc */
void*
xrealloc (void* ptr, size_t new_size)
{
  void* m = realloc (ptr, new_size);
  
  if (m == NULL)
    xalloc_die ();

  return m;
}


/* Wrapper para strdup */
char *
xstrdup (const char *str)
{
  char *ret;

  if (str != NULL)
    {
      ret = xmalloc (strlen (str) + 1);
      strcpy (ret, str);
    }
  else
    ret = NULL;
  
  return ret;
}

/* Cuando nos quedamos sin memoria... */
static void
xalloc_die (void)
{
  fprintf (stderr,  "Memoria insuficiente (interno)\n");
  exit (EXIT_FAILURE);
}

/* Para manipular arrays de punteros */
void
ptr_list_append (void ***list, int *count, void *new)
{
  int i;
  
  for (i = 0; i < *count; i++)
    if ((*list)[i] == NULL)
      break;
      
  if (i == *count)
    *list = xrealloc (*list, ++*count * sizeof (void *));
    
  (*list)[i] = new;
}

int
ptr_list_remove_first (void ***list, int *count, void *ptr)
{
  int i;
  int found;
  
  found = 0;
  
  for (i = 0; i < *count; i++)
    if ((*list)[i] == ptr || ptr == NULL)
    {
      (*list)[i] = NULL;
      found++;
      
      break;
    }
    
  return found;
}


int
ptr_list_remove_all (void ***list, int *count, void *ptr)
{
  int i;
  int found;
  
  found = 0;
  
  for (i = 0; i < *count; i++)
    if ((*list)[i] == ptr || ptr == NULL)
    {
      (*list)[i] = NULL;
      found++;
    }
    
  return found;
}


char *
str_append_char (char* source, char c)
{
  int strsiz;
  char *nptr;

  strsiz = source == NULL ? 1 : strlen (source) + 1;

  nptr = (char *) xrealloc ( (void *) source, strsiz + 1);

  if (nptr == NULL)
    return NULL;

  nptr[strsiz - 1] = c;
  nptr[strsiz]     = '\0';

  return nptr;
}

char *
read_line (int fd)
{
  char c;
  char *line;
  char *nptr;

  line = NULL;

  for (;;)
  {
    if (read (fd, &c, 1) < 1)
      return line;

    if (c == '\r')
      continue;

    if (c == '\n')
    {
      if (line == NULL)
        line = strdup ("");
      return line;
    }

    if ( (nptr = str_append_char (line, c)) == NULL)
    {
      free (line);
      return NULL;
    }

    line = nptr;
  }

  return NULL;
}


char *
fread_line (FILE *fp)
{
  char c;
  char *line;
  char *nptr;

  line = NULL;

  for (;;)
  {
    if ((c = fgetc (fp)) == EOF)
      return line;

    if (c == '\r')
      continue;

    if (c == '\n')
    {
      if (line == NULL)
        line = strdup ("");
      return line;
    }

    if ( (nptr = str_append_char (line, c)) == NULL)
    {
      free (line);
      return NULL;
    }

    line = nptr;
  }

  return NULL;
}

