#include "telemetry.capnp.h"
/* AUTO GENERATED - DO NOT EDIT */
#ifdef __GNUC__
# define capnp_unused __attribute__((unused))
# define capnp_use(x) (void) x;
#else
# define capnp_unused
# define capnp_use(x)
#endif


Telemetry_ptr new_Telemetry(struct capn_segment *s) {
	Telemetry_ptr p;
	p.p = capn_new_struct(s, 8, 0);
	return p;
}
Telemetry_list new_Telemetry_list(struct capn_segment *s, int len) {
	Telemetry_list p;
	p.p = capn_new_list(s, len, 8, 0);
	return p;
}
void read_Telemetry(struct Telemetry *s capnp_unused, Telemetry_ptr p) {
	capn_resolve(&p.p);
	capnp_use(s);
	s->timestampStart = (int64_t) ((int64_t)(capn_read64(p.p, 0)));
}
void write_Telemetry(const struct Telemetry *s capnp_unused, Telemetry_ptr p) {
	capn_resolve(&p.p);
	capnp_use(s);
	capn_write64(p.p, 0, (uint64_t) (s->timestampStart));
}
void get_Telemetry(struct Telemetry *s, Telemetry_list l, int i) {
	Telemetry_ptr p;
	p.p = capn_getp(l.p, i, 0);
	read_Telemetry(s, p);
}
void set_Telemetry(const struct Telemetry *s, Telemetry_list l, int i) {
	Telemetry_ptr p;
	p.p = capn_getp(l.p, i, 0);
	write_Telemetry(s, p);
}
