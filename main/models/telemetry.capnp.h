#ifndef CAPN_ED289BEA3AA8ABA7
#define CAPN_ED289BEA3AA8ABA7
/* AUTO GENERATED - DO NOT EDIT */
#include <capnp_c.h>

#if CAPN_VERSION != 1
#error "version mismatch between capnp_c.h and generated code"
#endif

#ifndef capnp_nowarn
# ifdef __GNUC__
#  define capnp_nowarn __extension__
# else
#  define capnp_nowarn
# endif
#endif


#ifdef __cplusplus
extern "C" {
#endif

struct Telemetry;

typedef struct {capn_ptr p;} Telemetry_ptr;

typedef struct {capn_ptr p;} Telemetry_list;

struct Telemetry {
	int64_t timestampStart;
};

static const size_t Telemetry_word_count = 1;

static const size_t Telemetry_pointer_count = 0;

static const size_t Telemetry_struct_bytes_count = 8;

Telemetry_ptr new_Telemetry(struct capn_segment*);

Telemetry_list new_Telemetry_list(struct capn_segment*, int len);

void read_Telemetry(struct Telemetry*, Telemetry_ptr);

void write_Telemetry(const struct Telemetry*, Telemetry_ptr);

void get_Telemetry(struct Telemetry*, Telemetry_list, int i);

void set_Telemetry(const struct Telemetry*, Telemetry_list, int i);

#ifdef __cplusplus
}
#endif
#endif
