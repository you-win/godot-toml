#include "toml_gd.h"

#include "core/array.h"
#include "core/dictionary.h"
#include "core/io/json.h"
#include "toml.hpp"

// https://github.com/ToruNiina/toml11/blob/master/tests/check_toml_test.cpp
struct Converter {
	Variant operator()(toml::boolean v) {
		return Variant((bool)v);
	}

	Variant operator()(toml::integer v) {
		return Variant((int)v);
	}

	Variant operator()(toml::floating v) {
		return Variant((real_t)v); // TODO this might be dangerous since toml::floating is a double
	}

	Variant operator()(const toml::string &v) {
		return Variant(String(v.str.c_str()));
	}

	Variant operator()(const toml::local_time &v) {
		Dictionary d;

		d["hour"] = v.hour;
		d["minute"] = v.minute;
		d["second"] = v.second;
		d["millisecond"] = v.millisecond;
		d["nanosecond"] = v.nanosecond;

		return Variant(d);
	}

	Variant operator()(const toml::local_date &v) {
		Dictionary d;

		d["year"] = v.year;
		d["month"] = v.month;
		d["day"] = v.day;

		return Variant(d);
	}

	Variant operator()(const toml::local_datetime &v) {
		Dictionary d;

		d["year"] = v.date.year;
		d["month"] = v.date.month;
		d["day"] = v.date.day;
		d["hour"] = v.time.hour;
		d["minute"] = v.time.minute;
		d["second"] = v.time.second;
		d["millisecond"] = v.time.millisecond;
		d["nanosecond"] = v.time.nanosecond;

		return Variant(d);
	}

	Variant operator()(const toml::offset_datetime &v) {
		Dictionary d;

		d["year"] = v.date.year;
		d["month"] = v.date.month;
		d["day"] = v.date.day;
		d["hour"] = v.time.hour;
		d["minute"] = v.time.minute;
		d["second"] = v.time.second;
		d["millisecond"] = v.time.millisecond;
		d["nanosecond"] = v.time.nanosecond;
		d["offset_hour"] = v.offset.hour;
		d["offset_minute"] = v.offset.minute;

		return Variant(d);
	}

	Variant operator()(const toml::table& v) {
		Dictionary d;

		
		for (const auto& elem : v) {
			d[String(((std::string)elem.first).c_str())] = toml::visit(*this, elem.second);
		}

		return Variant(d);
	}

	Variant operator()(const toml::array& v) {
		Array a;

		for (const auto& elem : v) {
			a.append(toml::visit(*this, elem));
		}

		return Variant(a);
	}
};

void TOMLParseResult::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_error"), &TOMLParseResult::get_error);
	ClassDB::bind_method(D_METHOD("get_error_string"), &TOMLParseResult::get_error_string);
	ClassDB::bind_method(D_METHOD("get_error_line"), &TOMLParseResult::get_error_line);
	ClassDB::bind_method(D_METHOD("get_result"), &TOMLParseResult::get_result);

	ClassDB::bind_method(D_METHOD("set_error", "error"), &TOMLParseResult::set_error);
	ClassDB::bind_method(D_METHOD("set_error_string", "error_string"), &TOMLParseResult::set_error_string);
	ClassDB::bind_method(D_METHOD("set_error_line", "error_line"), &TOMLParseResult::set_error_line);
	ClassDB::bind_method(D_METHOD("set_result", "result"), &TOMLParseResult::set_result);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "error", PROPERTY_HINT_NONE, "Error", PROPERTY_USAGE_CLASS_IS_ENUM), "set_error", "get_error");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "error_string"), "set_error_string", "get_error_string");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "error_line"), "set_error_line", "get_error_line");
	ADD_PROPERTY(PropertyInfo(Variant::NIL, "result", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NIL_IS_VARIANT), "set_result", "get_result");
}

void TOMLParseResult::set_error(Error p_error) {
	error = p_error;
}

Error TOMLParseResult::get_error() const {
	return error;
}

void TOMLParseResult::set_error_string(const String &p_error_string) {
	error_string = p_error_string;
}

String TOMLParseResult::get_error_string() const {
	return error_string;
}

void TOMLParseResult::set_error_line(int p_error_line) {
	error_line = p_error_line;
}

int TOMLParseResult::get_error_line() const {
	return error_line;
}

void TOMLParseResult::set_result(const Variant &p_result) {
	result = p_result;
}

Variant TOMLParseResult::get_result() const {
	return result;
}

void TOML::_bind_methods() {
	ClassDB::bind_method(D_METHOD("print", "value", "indent", "sort_keys"), &TOML::print, DEFVAL(String()), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("parse", "toml"), &TOML::parse);
}

String TOML::print(const Variant &p_value, const String &p_indent, bool p_sort_keys) {
	return JSON::print(p_value, p_indent, p_sort_keys);
}

Ref<TOMLParseResult> TOML::parse(const String &p_toml) {
	Ref<TOMLParseResult> result;
	result.instance();

	std::wstring ws = p_toml.c_str();
	std::string s(ws.begin(), ws.end());
	// Solution for parsing strings
	// https://github.com/ToruNiina/toml11/issues/88
	std::istringstream is(s, std::ios_base::binary | std::ios_base::in);

	try {
		auto table = toml::parse(is, "std::string");
		Variant v = toml::visit(Converter(), table);

		result->set_result(v);
		result->set_error(OK);
	} catch(const toml::syntax_error& err) {
		ERR_PRINT(vformat("Error parsing TOML: %s", String(err.what())));

		result->set_error(ERR_PARSE_ERROR);
		result->set_error_string(String(err.what()));
		result->set_error_line(err.location().line());
	}

	return result;
}
