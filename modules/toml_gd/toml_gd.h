#ifndef TOML_GD_H
#define TOML_GD_H

#include "core/reference.h"
#include "core/ustring.h"
#include "core/variant.h"

#include "thirdparty/toml11/toml.hpp"

class TOMLParseResult : public Reference {
	GDCLASS(TOMLParseResult, Reference);

	Error error;
	String error_string;
	int error_line;

	Variant result;

protected:
	static void _bind_methods();

public:
	void set_error(Error p_error);
	Error get_error() const;

	void set_error_string(const String &p_error_string);
	String get_error_string() const;

	void set_error_line(int p_error_line);
	int get_error_line() const;

	void set_result(const Variant &p_result);
	Variant get_result() const;

	TOMLParseResult() :
			error_line(-1) {}
};

class TOML : public Reference {
	GDCLASS(TOML, Reference);

private:
	toml::value _to_toml_string(const Variant &p_value);

protected:
	static void _bind_methods();

public:
	Ref<TOMLParseResult> parse(const String &p_toml);
	String to_toml_string(const Variant &p_value, int p_width = 80);
};

#endif // TOML_GD_H
