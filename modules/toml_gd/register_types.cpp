#include "register_types.h"

#include "toml_gd.h"

void register_toml_gd_types() {
	ClassDB::register_class<TOMLParseResult>();
	ClassDB::register_class<TOML>();
}

void unregister_toml_gd_types() {
}
