#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "stl.h"


static inline void init_list(stl_list_t* list)
{
  list->count = 0;
  list->head = NULL;
  list->tail = NULL;
}

static void free_list(stl_list_t* list)
{
  stl_list_elem_t* tmp;
  stl_list_elem_t* pos;

  pos = list->head;
  while (pos != NULL)
  {
    tmp = pos;
    pos = pos->next;
    free(tmp);
  }

  init_list(list);
}

static void push_list_elem(stl_list_t* list, stl_list_elem_t* elem)
{
  elem->next = NULL;

  if (list->head == NULL)
    list->head = elem;
  else
    list->tail->next = elem;

  list->tail = elem;

  ++list->count;
}


/* internal parser state */

typedef struct stl_parser
{
  /* current data pointer */
  const unsigned char* data;
  unsigned int size;

  /* facet list */
  stl_list_t* list;

  /* current line count */
  unsigned int line;

} stl_parser_t;


static inline void init_parser
(stl_parser_t* parser, const void* data, size_t size)
{
  parser->data = (const unsigned char*)data;
  parser->size = size;
  parser->line = 0;
}

static inline int is_eol(unsigned char c)
{
  return c == '\n';
}

static int skip_next_line(stl_parser_t* parser)
{
  if (parser->size == 0) return -1;
  if (is_eol(*parser->data) == 0) return -1;

  ++parser->line;
  ++parser->data;
  --parser->size;

  return 0;
}

static inline int is_blank(unsigned char c)
{
  return c == ' ';
}

static void skip_whites(stl_parser_t* parser)
{
  while (parser->size && is_blank(*parser->data))
  {
    ++parser->data;
    --parser->size;
  }
}

static int check_keyword
(stl_parser_t* parser, const char* kw, unsigned int size)
{
  skip_whites(parser);

  if (parser->size < size)
    return -1;
  if (memcmp(parser->data, kw, size))
    return -1;

  return 0;
}

static int parse_keyword
(stl_parser_t* parser, const char* kw, unsigned int size)
{
  if (check_keyword(parser, kw, size) == -1)
    return -1;

  parser->data += size;
  parser->size -= size;

  return 0;
}

static int parse_value(stl_parser_t* parser, double_type* value)
{
  unsigned int diff;
  unsigned char* endptr;

  *value = strtod((const char*)parser->data, (char**)&endptr);

  diff = endptr - parser->data;
  parser->data += diff;
  parser->size -= diff;

  return 0;
}

static int parse_triangle(stl_parser_t* parser, double_type* values)
{
  unsigned int i;

  for (i = 0; i < 3; ++i)
  {
    skip_whites(parser);

    if (parse_value(parser, values + i) == -1)
      return -1;
  }

  return 0;
}

static int parse_vertex_line(stl_parser_t* parser, double* values)
{
#define VERTEX_STRING "vertex"
#define VERTEX_LENGTH (sizeof(VERTEX_STRING) - 1)
  if (parse_keyword(parser, VERTEX_STRING, VERTEX_LENGTH) == -1)
    return -1;

  skip_whites(parser);

  if (parse_triangle(parser, values) == -1) return -1;

  skip_whites(parser);

  return skip_next_line(parser);
}

static int parse_outer_loop
(stl_parser_t* parser, stl_list_elem_t* elem)
{
  unsigned int i;

  skip_whites(parser);

#define OUTER_LOOP_STRING "outer loop"
#define OUTER_LOOP_LENGTH (sizeof(OUTER_LOOP_STRING) - 1)
  if (parse_keyword(parser, OUTER_LOOP_STRING, OUTER_LOOP_LENGTH) == -1)
    return -1;

  skip_whites(parser);
  skip_next_line(parser);

  for (i = 0; i < 3; ++i)
  {
    if (parse_vertex_line(parser, elem->vertices + 3 * i) == -1)
      return -1;
  }

  skip_whites(parser);

#define ENDLOOP_STRING "endloop"
#define ENDLOOP_LENGTH (sizeof(ENDLOOP_STRING) - 1)
  if (parse_keyword(parser, ENDLOOP_STRING, ENDLOOP_LENGTH) == -1)
    return -1;

  skip_whites(parser);

  return skip_next_line(parser);
}

static int parse_normal(stl_parser_t* parser)
{
  stl_list_elem_t* elem = NULL;

#define NORMAL_STRING "normal"
#define NORMAL_LENGTH (sizeof(NORMAL_STRING) - 1)
  if (parse_keyword(parser, NORMAL_STRING, NORMAL_LENGTH) == -1)
    return -1;

  skip_whites(parser);

  elem = (stl_list_elem_t*)malloc(sizeof(stl_list_elem_t));
  if (elem == NULL) return -1;

  if (parse_triangle(parser, elem->normal) == -1)
    goto on_error;

  skip_whites(parser);
  if (skip_next_line(parser) == -1)
    goto on_error;

  if (parse_outer_loop(parser, elem) == -1)
    goto on_error;

  push_list_elem(parser->list, elem);

  return 0;

 on_error:
  free(elem);
  return -1;
}

static int parse_facet(stl_parser_t* parser)
{
#define FACET_STRING "facet"
#define FACET_LENGTH (sizeof(FACET_STRING) - 1)
  if (parse_keyword(parser, FACET_STRING, FACET_LENGTH) == -1)
    return -1;

  if (parse_normal(parser) == -1) return -1;

  skip_whites(parser);

#define ENDFACET_STRING "endfacet"
#define ENDFACET_LENGTH (sizeof(ENDFACET_STRING) - 1)
  if (parse_keyword(parser, ENDFACET_STRING, ENDFACET_LENGTH) == -1)
    return -1;

  return skip_next_line(parser);
}

static inline int is_alpha_char(unsigned char c)
{
  if (c >= 'a') return c <= 'z';
  return c >= 'A' && c <= 'Z';
}

static int parse_string(stl_parser_t* parser)
{
  while (is_alpha_char(*parser->data))
  {
    ++parser->data;
    --parser->size;
  }

  return 0;
}

static int parse_solid(stl_parser_t* parser)
{
#define SOLID_STRING "solid"
#define SOLID_LENGTH (sizeof(SOLID_STRING) - 1)
  if (parse_keyword(parser, SOLID_STRING, SOLID_LENGTH) == -1)
    return -1;

  skip_whites(parser);
  parse_string(parser);

  skip_whites(parser);

  if (skip_next_line(parser) == -1)
    return -1;

  while (1)
  {
    if (parse_facet(parser) == -1)
      return -1;

    if (check_keyword(parser, FACET_STRING, FACET_LENGTH) == -1)
      break ;
  }

#define ENDSOLID_STRING "endsolid"
#define ENDSOLID_LENGTH (sizeof(ENDSOLID_STRING) - 1)
  return parse_keyword(parser, ENDSOLID_STRING, ENDSOLID_LENGTH);
}

static int map_file
(const char* path, void** addr, size_t* size)
{
  int error = -1;
  int fd;
  struct stat st;

  fd = open(path, O_RDONLY);
  if (fd == -1) return -1;

  if (fstat(fd, &st) == -1) goto on_error;

  *addr = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
  if (*addr == MAP_FAILED) goto on_error;

  *size = st.st_size;
  error = 0;

 on_error:
  close(fd);
  return error;
}

static inline void unmap_file(void* addr, size_t size)
{ munmap(addr, size); }


/* exported */

int stl_read_ascii_file(const char* path, stl_list_t* list)
{
  /* assume ascii format */

  int error = -1;
  void* addr;
  size_t size;
  stl_parser_t parser;

  if (map_file(path, &addr, &size) == -1)
    return -1;

  init_parser(&parser, addr, size);
  init_list(list);
  parser.list = list;

  error = parse_solid(&parser);
  unmap_file(addr, size);

  return error;
}


/* read a binary file */

static inline uint16_t read_uint16_le(const void* p)
{ return *(const uint16_t*)p; }

static inline uint32_t read_uint32_le(const void* p)
{ return *(const uint32_t*)p; }

static inline float read_real32_le(const void* p)
{ return *(const float*)p; }

static void read_vertex(uintptr_t p, double_type* v)
{
#define SIZEOF_REAL32 4
  v[0] = read_real32_le((const void*)(p + 0 * SIZEOF_REAL32));
  v[1] = read_real32_le((const void*)(p + 1 * SIZEOF_REAL32));
  v[2] = read_real32_le((const void*)(p + 2 * SIZEOF_REAL32));
}

int stl_read_binary_file(const char* path, stl_list_t* list)
{
  /* assume binary format */

  /* this is a temporary version of the routine,
     the final one should not work on a list to
     avoid all the allocations.
   */

  int error = -1;
  uintptr_t addr;
  size_t size;
  unsigned int j;
  uint32_t i;
  uint32_t count;
  stl_list_elem_t* elem;

  init_list(list);

  if (map_file(path, (void**)&addr, &size) == -1)
    return -1;

  /* skip the 80 bytes header */
  if (size < 80) goto on_error;
  addr += 80;
  size -= 80;

  /* triangle count */
  count = read_uint32_le((const void*)addr);
  addr += sizeof(uint32_t);
  size -= sizeof(uint32_t);

  /* foreach triangle */
  for (i = 0; i < count; ++i)
  {
    if (size < (4 * 3 * SIZEOF_REAL32 + sizeof(uint16_t)))
    {
      /* unexpected eof. not an error. */
      break ;
    }

    elem = (stl_list_elem_t*)malloc(sizeof(stl_list_elem_t));
    if (elem == NULL) goto on_error;

    /* read normal */
    read_vertex(addr, elem->normal);
#define SIZEOF_VERTEX (3 * SIZEOF_REAL32)
    addr += SIZEOF_VERTEX;

    /* read vertices */
    for (j = 0; j < 3; ++j)
    {
      read_vertex(addr, elem->vertices + j);
      addr += SIZEOF_VERTEX;
      size -= SIZEOF_VERTEX;
    }

    /* skip attribute, assumed 0 */
    addr += sizeof(uint16_t);
    size -= sizeof(uint16_t);

    push_list_elem(list, elem);
  }

  /* success */
  error = 0;

 on_error:
  unmap_file((void*)addr, size);

  if (error) free_list(list);

  return error;
}

void stl_list_to_soa(stl_list_t* list, double_type* soa)
{
  /* convert to a structure of arrays */
  /* do not include normals to the output */
  /* assume soa size is at least 9 * sizeof(double) */

  const stl_list_elem_t* pos;

  for (pos = list->head; pos != NULL; pos = pos->next)
  {
    soa[0] = pos->vertices[0];
    soa[1] = pos->vertices[3];
    soa[2] = pos->vertices[6];

    soa[3] = pos->vertices[1];
    soa[4] = pos->vertices[4];
    soa[5] = pos->vertices[7];

    soa[6] = pos->vertices[2];
    soa[7] = pos->vertices[5];
    soa[8] = pos->vertices[8];

    soa += 9;
  }
}

void stl_free_list(stl_list_t* list)
{
  free_list(list);
}


#if STL_CONFIG_UNIT /* unit */

#include <stdio.h>

static inline void print_triangle(const double_type* v)
{
  printf("%lf %lf %lf", v[0], v[1], v[2]);
}

void print_list(const stl_list_t* list)
{
  const stl_list_elem_t* pos;
  unsigned int i;

  printf("solid UNAMED\n");

  for (pos = list->head; pos != NULL; pos = pos->next)
  {
    printf("facet normal ");
    print_triangle(pos->normal);
    printf("\n");

    printf("outer loop\n");
    for (i = 0; i < 3; ++i)
    {
      printf("vertex ");
      print_triangle(pos->vertices + i * 3);
      printf("\n");
    }

    printf("endloop\n");
    printf("endfacet\n");
  }

  printf("endsolid UNAMED\n");
}

int main(int ac, char** av)
{
  stl_list_t list;
  unsigned int count;
  double* soa = NULL;

  if (stl_read_binary_file("../data/binary/porsche.stl", &list))
    return -1;

  print_list(&list);

  count = list.count;
  soa = malloc(count * 9 * sizeof(double));
  if (soa == NULL) goto on_error;
  stl_list_to_soa(&list, soa);

 on_error:
  if (soa != NULL) free(soa);
  stl_free_list(&list);
  
  return 0;
}

#endif /* STL_CONFIG_UNIT */
