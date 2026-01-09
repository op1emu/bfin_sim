#pragma once

#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// definitions for the simulator

#define PROFILE_COUNT_INSN(...)
#define PROFILE_COUNT_CORE(...)
#define PROFILE_BRANCH_TAKEN(...)
#define PROFILE_BRANCH_UNTAKEN(...)
#define TRACE_INSN(...)
#define TRACE_EVENTS(...)
#define TRACE_CORE(...)
#define TRACE_REGISTER(...)
#define TRACE_BRANCH(...)
#define TRACE_EXTRACT(...)
#define TRACE_DECODE(...)
#define trace_prefix(...)
#define NULL_CIA 0

#define ARRAY_SIZE(a) (sizeof (a) / sizeof ((a)[0]))
#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

#define sim_io_eprintf(sd, ...) fprintf (stderr, __VA_ARGS__)
#define sim_io_error(sd, ...) fprintf (stderr, __VA_ARGS__)
#define sim_io_printf(sd, ...) fprintf (stdout, __VA_ARGS__)
#define sim_io_flush_stdout(sd) fflush (stdout)

#define dv_bfin_mmr_require_32(...) 1
#define dv_bfin_mmr_invalid(...)

typedef uint32_t unsigned_address;
typedef int32_t signed_address;
typedef unsigned_address address_word;

typedef uint32_t unsigned_word;
typedef int32_t signed_word;

typedef uint32_t unsigned_cell;
typedef int32_t signed_cell;
typedef signed_cell cell_word; /* cells are normally signed */

typedef struct sim_state *SIM_DESC;
typedef struct _sim_cpu SIM_CPU;
typedef struct _sim_cpu sim_cpu;

/* Generic address space (maps) and access attributes */
enum {
  read_map = 0,
  write_map = 1,
  exec_map = 2,
  io_map = 3,
  nr_maps = 32, /* something small */
};

enum {
#define I(insn) BFIN_INSN_##insn,
#include "insn_list.def"
#undef I
  BFIN_INSN_MAX
};

enum sim_environment {
  ALL_ENVIRONMENT,
  USER_ENVIRONMENT,
  VIRTUAL_ENVIRONMENT,
  OPERATING_ENVIRONMENT
};

typedef enum {
  SIM_SIGNONE = 64,
  /* illegal insn */
  SIM_SIGILL,
  /* breakpoint */
  SIM_SIGTRAP,
  /* misaligned memory access */
  SIM_SIGBUS,
  /* tried to read/write memory that's not readable/writable */
  SIM_SIGSEGV,
  /* cpu limit exceeded */
  SIM_SIGXCPU,
  /* simulation interrupted (sim_stop called) */
  SIM_SIGINT,
  /* Floating point or integer divide */
  SIM_SIGFPE,
  /* simulation aborted */
  SIM_SIGABRT
} SIM_SIGNAL;

typedef enum {
  SIM_OPEN_STANDALONE, /* simulator used standalone (run.c) */
  SIM_OPEN_DEBUG       /* simulator used by debugger (gdb) */
} SIM_OPEN_KIND;

struct _sim_cpu;
struct sim_state {
  sim_cpu *cpu;
# define STATE_CPU(sd, n) ((sd)->cpu)

  /* Who opened the simulator.  */
  SIM_OPEN_KIND open_kind;
#define STATE_OPEN_KIND(sd) ((sd)->open_kind)

  /* The type of simulation environment (user/operating).  */
  enum sim_environment environment;
#define STATE_ENVIRONMENT(sd) ((sd)->environment)
};

struct _sim_cpu {
  /* Backlink to main state struct.  */
  SIM_DESC state;
#define CPU_STATE(cpu) ((cpu)->state)

  /* Pointer for sim target to store arbitrary cpu data.  Normally the
     target should define a struct and use it here.  */
  void *arch_data;
#define CPU_ARCH_DATA(cpu) ((cpu)->arch_data)

  void *host;
};

typedef struct {

} SIM_MACH;


unsigned
sim_core_read_buffer (SIM_DESC sd,
		      sim_cpu *cpu,
		      unsigned map,
		      void *buffer,
		      address_word addr,
		      unsigned len);

unsigned
sim_core_write_buffer (SIM_DESC sd,
		       sim_cpu *cpu,
		       unsigned map,
		       const void *buffer,
		       address_word addr,
		       unsigned len);


#define sim_write(sd, addr, buffer, size) sim_core_write_buffer(sd, STATE_CPU(sd, 0), write_map, buffer, addr, size)
#define sim_read(sd, addr, buffer, size) sim_core_read_buffer(sd, STATE_CPU(sd, 0), read_map, buffer, addr, size)