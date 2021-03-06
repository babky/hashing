#include <fcntl.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define VALUE_OFFSET		// Value of an entity is the offset in its temporary file

#define IO_BUFFER_SIZE 1048576		// Buffer size for file I/O
#define FAST_READ			// Replace fscanf() by inline code
#define INLINE_QSORT			// Replace qsort() by inline code
#define DEBUGGING			// Print debug messages to stderr

#ifdef VALUE_OFFSET
//#define BLOCK_SIZE 580000000		// Block size for presorting
#define BLOCK_SIZE 280000000		// Block size for presorting, using cca 3.2 GB RAM
//#define BLOCK_SIZE 2		// Block size for presorting, debugging
#else
#define BLOCK_SIZE 440000000		// Block size for presorting
#endif

/*** Auxiliary functions ***/

typedef unsigned int uint;
typedef uint64_t u64;

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

static void __attribute__((noreturn)) die(char *msg, ...)
{
  va_list args;
  va_start(args, msg);
  fprintf(stderr, "FATAL ERROR: ");
  vfprintf(stderr, msg, args);
  fprintf(stderr, "\n");
  exit(1);
}

u64
get_timestamp(void)
{
#if 0   // needs -lrt
  struct timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC, &ts) < 0)
    die("clock_gettime failed: %m");
  return (u64)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
#else
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (u64)tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
}

static u64 start_time;

#ifdef DEBUGGING

static void message(char *msg, ...)
{
  va_list args;
  va_start(args, msg);
  u64 t = get_timestamp() - start_time;
  fprintf(stderr, "[%6u.%03u]  ", (uint)(t/1000), (uint)(t%1000));
  vfprintf(stderr, msg, args);
  fprintf(stderr, "\n");
}
#define DEBUG(x...) message(x)

#else

#define DEBUG(x...) do { } while(0)

#endif

static void *xmalloc(size_t n)
{
  void *x = malloc(n);
  if (!x)
    die("Out of memory");
  return x;
}

/*** Temporary files ***/

struct kv {
  u64 k;
#ifdef VALUE_OFFSET
  uint o;
#else
  u64 v;
#endif
} __attribute__((__packed__));

struct file {
  struct file *next;
  char name[16];
  int fd;
  u64 items;
#ifdef VALUE_OFFSET
  u64 line_offset;
#endif
  struct kv *buffer, *rptr, *rend, *wptr, *wend;
  int eof;
};

static struct file *first_file, *last_file;
static uint num_files;
static uint file_counter;
static u64 line_counter;

#ifdef VALUE_OFFSET
static u64 file_first_line;
#endif


static struct file *new_file(void)
{
  struct file *f = xmalloc(sizeof(*f));
  memset(f, 0, sizeof(*f));
  if (last_file)
    last_file->next = f;
  else
    first_file = f;
  last_file = f;
  num_files++;

#ifdef VALUE_OFFSET
  f->line_offset = file_first_line;
  file_first_line = line_counter;
#endif

  sprintf(f->name, "tmp-%u", file_counter++);
  f->fd = open(f->name, O_RDWR | O_CREAT | O_TRUNC, 0666);
  if (f->fd < 0)
    die("Cannot open %s: %m", f->name);
  DEBUG("Created %s -> fd %d", f->name, f->fd);

  return f;
}

static void get_buffer(struct file *f)
{
  if (!f->buffer)
    {
      ssize_t s = (ssize_t) IO_BUFFER_SIZE * sizeof(struct kv);
      f->buffer = xmalloc(s);
    }
}

static void free_buffer(struct file *f)
{
  if (f->buffer)
    free(f->buffer);
  f->buffer = NULL;
}

static void write_buffer(struct file *f)
{
  uint n = f->wptr - f->buffer;
  if (n)
    {
      ssize_t s = (ssize_t) n * sizeof(struct kv);
      ssize_t l = write(f->fd, f->buffer, s);
      if (l < 0)
	die("Write error: %m");
      if (l < s)
	die("Short write");
      f->items += n;
    }
  else
    get_buffer(f);

  f->wptr = f->buffer;
  f->wend = f->buffer + IO_BUFFER_SIZE;
}

static void write_record(struct file *f, struct kv *x)
{
  if (f->wptr)
    {
      struct kv *prev = f->wptr - 1;
      if (prev->k == x->k)
	{
#ifdef VALUE_OFFSET
	  if (prev->o > x->o)
	    prev->o = x->o;
#else
	  if (prev->v > x->v)
	    prev->v = x->v;
#endif
	  return;
	}
    }

  if (f->wptr >= f->wend)
    write_buffer(f);
  *f->wptr++ = *x;
}

static void flush_file(struct file *f)
{
  write_buffer(f);
  DEBUG("Flushed %s (%jd items)", f->name, (intmax_t) f->items);
  free_buffer(f);
  close(f->fd);
  f->fd = -1;
}

static struct file *pick_file(void)
{
  struct file *f = first_file;
  first_file = f->next;
  if (!first_file)
    last_file = NULL;

  f->fd = open(f->name, O_RDONLY);
  if (f->fd < 0)
    die("Cannot open %s: %m", f->name);
  DEBUG("Reading %s (%jd items) -> fd %d", f->name, (intmax_t) f->items, f->fd);
  f->rptr = f->rend = NULL;
  get_buffer(f);
  return f;
}

static void read_record(struct file *f)
{
  if (f->rptr >= f->rend)
    {
      if (f->eof)
	return;
      ssize_t s = (size_t) IO_BUFFER_SIZE * sizeof(struct kv);
      ssize_t l = read(f->fd, f->buffer, s);
      if (l < 0)
	die("Read error: %m");
      if (l)
	{
	  l /= sizeof(struct kv);
	  f->rptr = f->buffer;
	  f->rend = f->buffer + l;
	}
      else
	f->eof = 1;
    }
}

static void drop_file(struct file *f)
{
  DEBUG("Dropping %s", f->name);
  free_buffer(f);
  close(f->fd);
  unlink(f->name);
  num_files--;
  free(f);
}

/*** Reading input ***/

#ifdef FAST_READ

static char stdio_buf[IO_BUFFER_SIZE];
static char *stdio_rptr, *stdio_rend;
static int fdin;

static int stdio_getc(void)
{
  if (stdio_rptr >= stdio_rend)
    {
      int l = read(fdin, stdio_buf, IO_BUFFER_SIZE);
      if (l < 0)
	die("Read error: %m");
      if (!l)
	return -1;
      stdio_rptr = stdio_buf;
      stdio_rend = stdio_buf + l;
    }
  return *stdio_rptr++;
}

static int read_u64(u64 *p)
{
  int c;
  while ((c = stdio_getc()) < '0' || c > '9')
    {
      if (c < 0)
	return 0;
    }

  u64 x = 0;
  while (c >= '0' && c <= '9')
    {
      x = 10*x + (c - '0');
      c = stdio_getc();
    }

  *p = x;
  return 1;
}

#else

static int read_u64(u64 *p)
{
  unsigned long long x;
  if (scanf("%llu", &x) == 1)
    {
      *p = x;
      return 1;
    }
  else
    return 0;
}

#endif

static struct kv *block;
static uint block_items;
static uint block_count;

static int read_block(void)
{
  DEBUG("Reading block %u", block_count, block_items);
  block_items = 0;
  while (block_items < BLOCK_SIZE)
    {
      u64 k;
      if (!read_u64(&k) )
	break;
#ifdef VALUE_OFFSET
      block[block_items++] = (struct kv) { .k = k, .o = ++line_counter - file_first_line};
#else
      block[block_items++] = (struct kv) { .k = k, .v = ++line_counter };
#endif
    }

  DEBUG("Read %u items", block_items);
  block_count++;
  return !!block_items;
}

/*** Pre-sorting ***/

#ifdef INLINE_QSORT

/* Adapted from <ucw/sorter/array-simple.h> */

#define ASORT_ELT(i) block[i]
#define ASORT_LT(x,y) ((x).k < (y).k)
#define ASORT_SWAP(i,j) do { struct kv tmp = ASORT_ELT(i); ASORT_ELT(i)=ASORT_ELT(j); ASORT_ELT(j)=tmp; } while (0)
#define ASORT_THRESHOLD 8

static void sort(void)
{
  struct stk { int l, r; } stack[8*sizeof(uint)];
  int l, r, left, right, m;
  uint sp = 0;
  struct kv pivot;

  if (block_items <= 1)
    return;

  left = 0;
  right = block_items - 1;
  for(;;)
    {
      l = left;
      r = right;
      m = (l+r)/2;
      if (ASORT_LT(ASORT_ELT(m), ASORT_ELT(l)))
	ASORT_SWAP(l,m);
      if (ASORT_LT(ASORT_ELT(r), ASORT_ELT(m)))
	{
	  ASORT_SWAP(m,r);
	  if (ASORT_LT(ASORT_ELT(m), ASORT_ELT(l)))
	    ASORT_SWAP(l,m);
	}
      pivot = ASORT_ELT(m);
      do
	{
	  while (ASORT_LT(ASORT_ELT(l), pivot))
	    l++;
	  while (ASORT_LT(pivot, ASORT_ELT(r)))
	    r--;
	  if (l < r)
	    {
	      ASORT_SWAP(l,r);
	      l++;
	      r--;
	    }
	  else if (l == r)
	    {
	      l++;
	      r--;
	    }
	}
      while (l <= r);
      if ((r - left) >= ASORT_THRESHOLD && (right - l) >= ASORT_THRESHOLD)
	{
	  /* Both partitions ok => push the larger one */
	  if ((r - left) > (right - l))
	    {
	      stack[sp].l = left;
	      stack[sp].r = r;
	      left = l;
	    }
	  else
	    {
	      stack[sp].l = l;
	      stack[sp].r = right;
	      right = r;
	    }
	  sp++;
	}
      else if ((r - left) >= ASORT_THRESHOLD)
	{
	  /* Left partition OK, right undersize */
	  right = r;
	}
      else if ((right - l) >= ASORT_THRESHOLD)
	{
	  /* Right partition OK, left undersize */
	  left = l;
	}
      else
	{
	  /* Both partitions undersize => pop */
	  if (!sp)
	    break;
	  sp--;
	  left = stack[sp].l;
	  right = stack[sp].r;
	}
    }

  /* Find minimal element which will serve as a barrier */
  r = MIN(block_items, ASORT_THRESHOLD);
  m = 0;
  for (l=1; l<r; l++)
    if (ASORT_LT(ASORT_ELT(l),ASORT_ELT(m)))
      m = l;
  ASORT_SWAP(0,m);

  /* Insertion sort */
  for (m=1; m<(int)block_items; m++)
    {
      l=m;
      while (ASORT_LT(ASORT_ELT(m),ASORT_ELT(l-1)))
	l--;
      while (l < m)
	{
	  ASORT_SWAP(l,m);
	  l++;
	}
    }
}

static void sort_block(void)
{
  sort();
  DEBUG("Sorted inline");
}

#else

static int scmp(const void *av, const void *bv)
{
  const struct kv *a = av, *b = bv;
  if (a->k < b->k)
    return -1;
  if (a->k > b->k)
    return 1;
  return 0;
}

static void sort_block(void)
{
  qsort(block, block_items, sizeof(struct kv), scmp);
  DEBUG("Sorted");
}

#endif

static void write_block(void)
{
  struct file *f = new_file();
  for (uint i=0; i<block_items; i++)
    write_record(f, &block[i]);
  flush_file(f);
}

/*** Multi-way merging ***/

struct mwstate {
  struct file *f;
  u64 key;
};

static FILE *fileout;

static void multiway_merge(void)
{
  uint ways = num_files;
  DEBUG("%d-way merge", ways);

  struct mwstate state[ways];
  for (uint i=0; i<ways; i++)
    state[i].f = pick_file();

	for (;;)
	{
		for (uint i=0; i<ways; i++)
		{
			struct mwstate *s = &state[i];
			read_record(s->f);
			if (s->f->eof)
			{
				drop_file(s->f);
				struct mwstate t = *s;
				*s = state[ways-1];
				state[ways-1] = t;
				ways--;
				i--;
				continue;
			}
			s->key = s->f->rptr->k;
		}
		if (!ways)
			return;
		
		uint rways = ways;
		while (rways)
		{
			uint best = 0;
			for (uint i=1; i<rways; i++)
				if (state[i].key < state[best].key)
					best = i;
			
			u64 min_key = state[best].key, min_value = UINT64_MAX;
			for (uint i=0; i<rways; i++)
				if(state[i].key == min_key)
				{
					struct mwstate *s = &state[i];
					struct file *f = s->f;
					struct kv *item = f->rptr++;
					u64 prev_key = item->k;

#ifdef VALUE_OFFSET
					u64 value = item->o + f->line_offset;
#else
					u64 value = item->v;
#endif
					min_value = min_value < value ? min_value : value;
					
					read_record(f);
					if (f->eof || f->rptr->k < prev_key)
					{
						struct mwstate t = *s;
						*s = state[rways-1];
						state[rways-1] = t;
						rways--;
						i--;
					}
					else
						s->key = f->rptr->k;
				}
			fprintf(fileout, "%llu %llu\n", (unsigned long long) min_key, (unsigned long long) min_value);
		}
	}
}

/*** Main ***/

int main(void)
{
  start_time = get_timestamp();
  DEBUG("Size of one entity %d", sizeof(struct kv));

	fdin = open("data.txt", O_RDONLY);
	if(fdin == -1)
	{
		perror("Cannot open input file");
		return 1;
	}

  DEBUG("### Pre-sorting ###");
  block = xmalloc(BLOCK_SIZE * sizeof(struct kv));
  while (read_block())
    {
      sort_block();
      write_block();
    }
  free(block);

  close(fdin);
  fileout = fopen("data.out", "w");
  if(!fileout)
  {
	  perror("Cannot open output file");
	  return 1;
  }

      DEBUG("### Main loop: %u files remain ###", num_files);
	multiway_merge();

  fclose(fileout);
  DEBUG("### Done ###");
  return 0;
}
