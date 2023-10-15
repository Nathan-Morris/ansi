#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

#ifdef _WIN32
#	include <Windows.h>
#endif
#pragma once

namespace ansi {
	
	//
	//

	inline bool init() {
#ifdef _WIN32
		HANDLE hdl_std_out = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD mode = 0;

		if (!GetConsoleMode(hdl_std_out, &mode))
			return false;

		if (!SetConsoleMode(hdl_std_out, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING))
			return false;
#endif
		return true;
	}

	//
	//

	enum class color_t {
		black = 0,
		red = 1,
		green = 2,
		yellow = 3,
		blue = 4,
		magenta = 5,
		cyan = 6,
		white = 7
	};

	enum class ground_t {
		fore = 3,
		back = 4
	};

	enum class style_t {
		reset = 0,
		bold = 1,
		underline = 4,
		swap = 7
	};

	enum class dir_t {
		up = 0,
		down,
		right,
		left
	};

	//
	//

	struct rgb_t {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		
		constexpr rgb_t() : 
			r(0), g(0), b(0) 
		{ }

		constexpr rgb_t(uint8_t r, uint8_t g, uint8_t b) :
			r(r), g(g), b(b)
		{ }

		constexpr rgb_t(int i) :
			r((i & 0xFF0000) >> 16),
			g((i & 0x00FF00) >> 8),
			b(i & 0x0000FF)
		{ }

		constexpr rgb_t(color_t color) : rgb_t() {
			const uint8_t comps[][3] = {
				{ 0x00, 0x00, 0x00 },
				{ 0xFF, 0x00, 0x00 },
				{ 0x00, 0xFF, 0x00 },
				{ 0xFF, 0xFF, 0x00 },
				{ 0x00, 0x00, 0xFF },
				{ 0xFF, 0x00, 0xFF },
				{ 0x00, 0xFF, 0xFF },
				{ 0xFF, 0xFF, 0xFF }
			};

			r = comps[(uint8_t)color][0];
			g = comps[(uint8_t)color][1];
			b = comps[(uint8_t)color][2];
		}

	};

	// 
	//

	struct istring {
		constexpr istring() = default;

		virtual std::ostream& write(std::ostream& out) const = 0;

		inline std::string str() const {
			std::stringstream ss;
			write(ss);
			return ss.str();
		}

		friend std::ostream& operator<<(std::ostream& out, const istring& str) {
			return str.write(out);
		}
	};

	struct color : public istring {
		color_t value;
		ground_t ground;
		bool bright;

		constexpr color(
			color_t color,
			bool bright = false,
			ground_t ground = ground_t::fore
		) :  
			value(color),
			ground(ground),
			bright(bright)
		{ }

		std::ostream& write(std::ostream& out) const;
	};

	struct style : public istring {
		style_t value;

		constexpr style(style_t value) : 
			value(value) 
		{ }

		std::ostream& write(std::ostream& out) const;
	};

	struct move : public istring {
		dir_t way;
		uint16_t count;

		constexpr move(dir_t way, uint16_t count) : 
			way(way),
			count(count)
		{ }

		std::ostream& write(std::ostream& out) const;
	};

	struct rgb : public istring {
		rgb_t value;
		ground_t ground;

		constexpr rgb(
			rgb_t value, 
			ground_t ground = ground_t::fore
		) : 
			value(value),
			ground(ground)
		{ }

		std::ostream& write(std::ostream& out) const;
	};

	//
	//

	namespace fore {
		constexpr color black	= color(color_t::black);
		constexpr color red		= color(color_t::red);
		constexpr color green	= color(color_t::green);
		constexpr color yellow	= color(color_t::yellow);
		constexpr color blue	= color(color_t::blue);
		constexpr color magenta	= color(color_t::magenta);
		constexpr color cyan	= color(color_t::cyan);
		constexpr color white	= color(color_t::white);

		constexpr color black_bright	= color(color_t::black, true);
		constexpr color red_bright		= color(color_t::red, true);
		constexpr color green_bright	= color(color_t::green, true);
		constexpr color yellow_bright	= color(color_t::yellow, true);
		constexpr color blue_bright		= color(color_t::blue, true);
		constexpr color magenta_bright	= color(color_t::magenta, true);
		constexpr color cyan_bright		= color(color_t::cyan, true);
		constexpr color white_bright	= color(color_t::white, true);
	};

	namespace back {
		constexpr color black	= color(color_t::black, false, ground_t::back);
		constexpr color red		= color(color_t::red, false, ground_t::back);
		constexpr color green	= color(color_t::green, false, ground_t::back);
		constexpr color yellow	= color(color_t::yellow, false, ground_t::back);
		constexpr color blue	= color(color_t::blue, false, ground_t::back);
		constexpr color magenta	= color(color_t::magenta, false, ground_t::back);
		constexpr color cyan	= color(color_t::cyan, false, ground_t::back);
		constexpr color white	= color(color_t::white, false, ground_t::back);

		constexpr color black_bright	= color(color_t::black, true, ground_t::back);
		constexpr color red_bright		= color(color_t::red, true, ground_t::back);
		constexpr color green_bright	= color(color_t::green, true, ground_t::back);
		constexpr color yellow_bright	= color(color_t::yellow, true, ground_t::back);
		constexpr color blue_bright		= color(color_t::blue, true, ground_t::back);
		constexpr color magenta_bright	= color(color_t::magenta, true, ground_t::back);
		constexpr color cyan_bright		= color(color_t::cyan, true, ground_t::back);
		constexpr color white_bright	= color(color_t::white, true, ground_t::back);
	};

	constexpr style reset	= style(style_t::reset);
	constexpr style bold	= style(style_t::bold);
	constexpr style underline	= style(style_t::underline);
	constexpr style swap	= style(style_t::swap);

	//
	//

	class text : public istring {
		std::string m_std_str;
		std::string m_ansi;

	public:

		text(const std::string& std_str, const istring& ansi);

		std::ostream& write(std::ostream& out) const;
	};
	
	//
	//

	enum class char_t : int {
		alpha = 0,
		numeric,
		symbolic,
		blank,
		all
	};

	class mapped : public istring {
		std::string m_std_string;
		std::unordered_map<int, rgb_t> m_map;
		struct {
			bool has;
			rgb_t rgb;
		} m_filters[5] = { 0 };

	public:
		mapped(const std::string& std_str);

		mapped& map(char c, rgb_t rgb);
		mapped& map(char_t filter, rgb_t rgb);

		std::ostream& write(std::ostream& out) const;
	};

	//
	//

	class gradient : public istring {
		std::vector<rgb_t> m_steps;
		std::vector<std::string> m_std_lines;
		size_t m_max_line_len = 0;

	public:
		gradient(
			const std::vector<std::string>& std_lines,
			const std::vector<rgb_t>& steps
		);

		gradient(
			const std::string& std_str,
			const std::vector<rgb_t>& steps
		);

		std::ostream& write(std::ostream& out) const;
	};

	//
	//
}