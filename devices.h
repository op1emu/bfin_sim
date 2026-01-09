#pragma once

#include "bfin-sim.h"
#include "ansidecl.h"
#include <stdlib.h>

static inline bu8 dv_load_1 (const void *ptr)
{
  const unsigned char *c = (const unsigned char *)ptr;
  return c[0];
}

static inline void dv_store_1 (void *ptr, bu8 val)
{
  unsigned char *c = (unsigned char *)ptr;
  c[0] = val;
}

static inline bu16 dv_load_2 (const void *ptr)
{
  const unsigned char *c = (const unsigned char *)ptr;
  return (c[1] << 8) | dv_load_1 (ptr);
}

static inline void dv_store_2 (void *ptr, bu16 val)
{
  unsigned char *c = (unsigned char *)ptr;
  c[1] = val >> 8;
  dv_store_1 (ptr, val);
}

static inline bu32 dv_load_4 (const void *ptr)
{
  const unsigned char *c = (const unsigned char *)ptr;
  return (c[3] << 24) | (c[2] << 16) | dv_load_2 (ptr);
}

static inline void dv_store_4 (void *ptr, bu32 val)
{
  unsigned char *c = (unsigned char *)ptr;
  c[3] = val >> 24;
  c[2] = val >> 16;
  dv_store_2 (ptr, val);
}

#define dv_w1c(ptr, val, bits) (*(ptr) &= ~((val) & (bits)))
static inline void dv_w1c_4 (bu32 *ptr, bu32 val, bu32 bits)
{
  dv_w1c (ptr, val, bits);
}

struct hw;

/* disposition of an object when things are reset */

typedef enum {
  permanent_object,
  temporary_object,
} object_disposition;


/* Memory transfer types */

typedef enum _transfer_type {
  read_transfer,
  write_transfer,
} transfer_type;


/* directions */

typedef enum {
  bidirect_port,
  input_port,
  output_port,
} port_direction;

 #include "hw-ports.h"

typedef struct _hw_unit
{
  int nr_cells;
  unsigned_cell cells[4]; /* unused cells are zero */
} hw_unit;

typedef struct _reg_property_spec
{
  hw_unit address;
  hw_unit size;
} reg_property_spec;

/* An IO operation from a parent to a child via the conecting bus.

   The SPACE:ADDR pair specify an address on the bus shared between
   the parent and child devices. */

typedef unsigned (hw_io_read_buffer_method)
     (struct hw *me,
      void *dest,
      int space,
      unsigned_word addr,
      unsigned nr_bytes);

#define hw_io_read_buffer(hw, dest, space, addr, nr_bytes) \
((hw)->to_io_read_buffer (hw, dest, space, addr, nr_bytes))

#define set_hw_io_read_buffer(hw, method) \
((hw)->to_io_read_buffer = (method))

typedef unsigned (hw_io_write_buffer_method)
     (struct hw *me,
      const void *source,
      int space,
      unsigned_word addr,
      unsigned nr_bytes);

#define hw_io_write_buffer(hw, src, space, addr, nr_bytes) \
((hw)->to_io_write_buffer (hw, src, space, addr, nr_bytes))

#define set_hw_io_write_buffer(hw, method) \
((hw)->to_io_write_buffer = (method))


/* As the bus that the device is attached too, to translate a devices
   hw_unit address/size into a form suitable for an attach address
   call.

   Return a zero result if the address should be ignored when looking
   for attach addresses. */

typedef int (hw_unit_address_to_attach_address_method)
     (struct hw *bus,
      const hw_unit *unit_addr,
      int *attach_space,
      unsigned_word *attach_addr,
      struct hw *client);

#define hw_unit_address_to_attach_address(bus, unit_addr, attach_space, attach_addr, client) \
((bus)->to_unit_address_to_attach_address (bus, unit_addr, attach_space, attach_addr, client))

#define set_hw_unit_address_to_attach_address(hw, method) \
((hw)->to_unit_address_to_attach_address = (method))

typedef int (hw_unit_size_to_attach_size_method)
     (struct hw *bus,
      const hw_unit *unit_size,
      unsigned *attach_size,
      struct hw *client);

#define hw_unit_size_to_attach_size(bus, unit_size, attach_size, client) \
((bus)->to_unit_size_to_attach_size (bus, unit_size, attach_size, client))

#define set_hw_unit_size_to_attach_size(hw, method) \
((hw)->to_unit_size_to_attach_size = (method))


struct hw
{
  /* our relatives */
  struct hw *parent_of_hw;
  /* our data */
  void *data_of_hw;
  struct sim_state *system_of_hw;

  /* Basic callbacks */
  hw_io_read_buffer_method *to_io_read_buffer;
  hw_io_write_buffer_method *to_io_write_buffer;

  /* address callbacks */
  hw_unit_address_to_attach_address_method *to_unit_address_to_attach_address;
  hw_unit_size_to_attach_size_method *to_unit_size_to_attach_size;

  struct hw_port_data *ports_of_hw;

  void *host;
};

typedef void (hw_finish_method)
     (struct hw *me);

struct hw_descriptor
{
  const char *family;
  hw_finish_method *to_finish;
};

#define hw_system(hw) ((hw)->system_of_hw)
#define hw_parent(hw) ((hw)->parent_of_hw + 0)
#define hw_data(hw) ((hw)->data_of_hw)
#define set_hw_data(hw, value) \
((hw)->data_of_hw = (value))
#define HW_ZALLOC(hw, type) (type*)calloc(1, sizeof(type))

#define DV_STATE_CACHED(cpu, dv) \
  ({ \
    struct bfin_##dv *__##dv = BFIN_CPU_STATE.dv##_cache; \
    __##dv; \
  })

#define HW_TRACE_WRITE()
#define HW_TRACE_READ()

#define hw_find_property(...) NULL
#define hw_find_reg_array_property(...) 0
#define hw_attach_address(...)
#define hw_abort(...)
#define sim_engine_halt(...)
#define sim_engine_restart(...)
#define hw_free(hw, ptr) free(ptr)


struct hw_event {

};
typedef void (hw_event_callback) (struct hw *me, void *data);

struct hw_event *hw_event_queue_schedule
(struct hw *me,
 int64_t delta_time,
 hw_event_callback *handler,
 void *data);

void hw_event_queue_deschedule(struct hw *me, struct hw_event *event);