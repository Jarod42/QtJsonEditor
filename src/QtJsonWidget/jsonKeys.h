#pragma once

namespace json_keys
{

	inline constexpr auto key_additionalProperties = "additionalProperties";
	inline constexpr auto key_description = "description";
	inline constexpr auto key_default = "default";
	inline constexpr auto key_dependencies = "dependencies";
	inline constexpr auto key_enum = "enum";
	inline constexpr auto key_format = "format";
	inline constexpr auto key_items = "items";
	inline constexpr auto key_minimum = "minimum";
	inline constexpr auto key_maximum = "maximum";
	inline constexpr auto key_patternProperties = "patternProperties";
	inline constexpr auto key_placeholder = "placeholder";
	inline constexpr auto key_properties = "properties";
	inline constexpr auto key_propertyOrder = "propertyOrder";
	inline constexpr auto key_ref = "$ref";
	inline constexpr auto key_required = "required";
	inline constexpr auto key_style = "style";
	inline constexpr auto key_title = "title";
	inline constexpr auto key_type = "type";

	inline constexpr auto format_color = "color";
	inline constexpr auto format_date = "date";

	inline constexpr auto style_calendar = "calendar";
	inline constexpr auto style_dateedit = "dateedit";
	inline constexpr auto style_lineedit = "lineedit";
	inline constexpr auto style_spinbox = "spinbox";

	inline constexpr auto type_any = "any";
	inline constexpr auto type_array = "array";
	inline constexpr auto type_boolean = "boolean";
	inline constexpr auto type_integer = "integer";
	inline constexpr auto type_number = "number";
	inline constexpr auto type_null = "null";
	inline constexpr auto type_object = "object";
	inline constexpr auto type_string = "string";

} // namespace json_keys
