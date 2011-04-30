#ifndef STL_H_INCLUDED
# define STL_H_INCLUDED


typedef double double_type;

/* stl facet container */

typedef struct stl_list_elem
{
  struct stl_list_elem* next;
  double_type normal[3];
  double_type vertices[9]; /* ordered x0, y0 ... */
} stl_list_elem_t;


typedef struct stl_list
{
  unsigned int count;
  stl_list_elem_t* head;
  stl_list_elem_t* tail;
} stl_list_t;


#if defined(__cplusplus)
extern "C" {
#endif

int stl_read_file(const char*, stl_list_t*);
void stl_list_to_soa(stl_list_t*, double_type*);
void stl_free_list(stl_list_t*);

#if defined(__cplusplus)
}
#endif


#endif /* ! STL_H_INCLUDED */
