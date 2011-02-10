#ifndef _UTIL_H
#define _UTIL_H

#include <stdio.h>
#include <errno.h>
#include <stdarg.h>

void *xmalloc (size_t siz);
void *xrealloc (void *p, size_t siz);
char *xstrdup (const char *s);
int   is_asciiz (const char *buf, int lbound, int ubound);
char *vstrbuild (const char *fmt, va_list ap);
char *strbuild (const char *fmt, ...);
char *str_append_char (char* source, char c);
char *read_line (int fd);
char *fread_line (FILE *fp);
void ptr_list_append (void ***, int *, void *);
int  ptr_list_remove_first (void ***, int *, void *);
int  ptr_list_remove_all (void ***, int *, void *);

void errno_save (void);
void errno_restore (void);

#define _JOIN(a, b) a ## b
#define JOIN(a, b) _JOIN(a, b)

#define _STRINGIFY(x) #x
#define STRINGIFY(x) _STRINGIFY (x)

#define IN_BOUNDS(x, range) (((x) >= 0) && ((x) < (range)))

#define WARNING(fmt, arg...)                                   \
      do {                                                     \
        errno_save ();                                         \
        fprintf (stderr, " * LibSIM warning: "                 \
          __FILE__ ":" STRINGIFY (__LINE__) ": " fmt, ## arg); \
        errno_restore ();                                      \
      } while (0);

#define ERROR(fmt, arg...)                                     \
      do {                                                     \
        errno_save ();                                         \
        fprintf (stderr, " * LibSIM error: "                   \
          __FILE__ ":" STRINGIFY (__LINE__) ": " fmt, ## arg); \
        errno_restore ();                                      \
      } while (0);


#define PTR_LIST(type, name)                         \
  type ** name ## _list;                             \
  int     name ## _count;

#define PTR_LIST_INIT(where, name)                   \
  where->name ## _list = NULL;                       \
  where->name ## _count = 0;                 

#define FOR_EACH_PTR(this, where, name)              \
  int JOIN (_idx_, __LINE__);                             \
  for (JOIN (_idx_, __LINE__) = 0;                        \
       JOIN (_idx_, __LINE__) < where->name ## _count;    \
       JOIN (_idx_, __LINE__)++)                          \
    if ((this = where->name ## _list[                \
      JOIN (_idx_, __LINE__)]) != NULL)

#define PTR_LIST_APPEND(where, name, what)           \
  ptr_list_append ((void ***) &where->name ## _list, \
    &where->name ## _count, what)
    
    
#endif /* _UTIL_H */

