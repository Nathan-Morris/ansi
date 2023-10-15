#include "ansi.h"

namespace ansi {

	//
	//

	std::ostream& color::write(std::ostream& out) const {
		out.put('\u001b');
		out.put('[');
		out.put('0' + (char)ground);
		out.put('0' + (char)value);
		if (bright) {
			out.put(';');
			out.put('1');
		}
		out.put('m');
		return out;
	}
	
	//
	//

	std::ostream& style::write(std::ostream& out) const {
		out.put('\u001b');
		out.put('[');
		out.put('0' + (char)value);
		out.put('m');
		return out;
	}

	//
	//

	std::ostream& move::write(std::ostream& out) const {
		char count_buf[6] = { 0 };
		int count_out = snprintf(count_buf, 6, "%hu", count);
		out.put('\u001b');
		out.put('[');
		out.write(count_buf, count_out);
		out.put('A' + (char)way);
		return out;
	}

	//
	//

	std::ostream& rgb::write(std::ostream& out) const {
		char rgb_buf[12] = { 0 };
		snprintf(rgb_buf, sizeof(rgb_buf), "%03hhu;%03hhu;%03hhu", value.r, value.g, value.b);
		out.put('\u001b');
		out.put('[');
		out.put('0' + (char)ground);
		out.put('8');
		out.put(';');
		out.put('2');
		out.put(';');
		out.write(rgb_buf, sizeof(rgb_buf));
		out.put('m');
		return out;
	}

	//
	//
	//


	text::text(const std::string& std_str, const istring& ansi) :
		m_std_str(std_str),
		m_ansi(ansi.str())
	{ }

	std::ostream& text::write(std::ostream& out) const {
		out.write(m_ansi.c_str(), m_ansi.size());
		out.write(m_std_str.c_str(), m_std_str.size());
		reset.write(out);
		return out;
	}

	//
	//
	//

	mapped::mapped(const std::string& std_str) 
		: m_std_string(std_str)
	{ }

	mapped& mapped::map(char c, rgb_t rgb) {
		m_map[(int)c] = rgb;
		return *this;
	}

	mapped& mapped::map(char_t filter, rgb_t rgb) {
		m_filters[(int)filter].has = true;
		m_filters[(int)filter].rgb = rgb;
		return *this;
	}

	std::ostream& mapped::write(std::ostream& out) const {
		using filter_match_func = int(*)(int);

		const filter_match_func filter_funcs[5] = {
			isalpha,
			isdigit,
			isspace,
			[](int i) { return (int)(strchr("!@#$%^&*()_+-=[]{}\\|;':\",./<>?", i) != nullptr); },
			[](int i) { return 1; }
		};

		for (const char& c : m_std_string) {
			auto iter = m_map.find((int)c);

			if (iter != m_map.end()) {
				rgb(iter->second).write(out);
				out.put(c);
				reset.write(out);

				continue;
			}
			
			size_t i = 0;

			for (; i != 5; i++) {
				if (!m_filters[i].has || !filter_funcs[i](c))
					continue;

				rgb(m_filters[i].rgb).write(out);
				out.put(c);
				reset.write(out);

				break;
			}

			if (i != 5)
				continue;

			out.put(c);
		}

		return out;
	}

	//
	//
	//

	gradient::gradient(
		const std::vector<std::string>& std_lines,
		const std::vector<rgb_t>& steps
	) : 
		m_steps(steps.begin(), steps.end()), 
		m_std_lines(std_lines.begin(), std_lines.end()) 
	{ }

	gradient::gradient(
		const std::string& std_str,
		const std::vector<rgb_t>& steps
	) :
		m_steps(steps.begin(), steps.end())
	{
		std::stringstream ss(std_str);
		std::string std_line;

		m_std_lines.reserve(std::count(std_str.begin(), std_str.end(), '\n') + 1);

		while (std::getline(ss, std_line)) {
			m_std_lines.emplace_back(std_line.begin(), std_line.end());
			m_max_line_len = max(m_max_line_len, m_std_lines.back().size());
		}
	}

	std::ostream& gradient::write(std::ostream& out) const {
		if (m_max_line_len == 0 || m_std_lines.empty())
			return out;

		//	Generate horizontal gradient
		std::vector<rgb> grad;
		grad.reserve(m_max_line_len);

		// gradient step of 0
		if (m_max_line_len == m_steps.size()) {
			for (size_t i = 0; i != m_steps.size(); i++) {
				grad.push_back(m_steps[i]);
			}
		}
		else if (m_steps.size() > 1) {
			size_t step_len = (m_max_line_len) / (m_steps.size() - 1);

			for (size_t i = 0; i != m_steps.size() - 1; i++) {

				int16_t dv[3] = {
					((int16_t)m_steps[i + 1].r - m_steps[i].r) / (int16_t)step_len,
					((int16_t)m_steps[i + 1].g - m_steps[i].g) / (int16_t)step_len,
					((int16_t)m_steps[i + 1].b - m_steps[i].b) / (int16_t)step_len
				};

				rgb_t nv = m_steps[i];

				for (size_t j = 0; j != step_len; j++) {
					grad.push_back(nv);
					
					nv.r += dv[0];
					nv.g += dv[1];
					nv.b += dv[2];
				}
			}

			// shitty code begets shitty solutions
			while (grad.size() < m_max_line_len) {
				size_t ri = rand() % grad.size();
				grad.insert(grad.begin() + ri, grad.at(ri));
			}
		}
		else {
			grad.insert(
				grad.begin(), 
				m_max_line_len, 
				m_steps.empty() ? rgb_t(color_t::white) : m_steps.front()
			);
		}

		//	Write lines
		for (size_t i = 0; i != m_std_lines.size(); i++) {
			const auto& line = m_std_lines[i];

			if (i) {
				out << std::endl;
			}

			for (size_t j = 0; j != min(line.size(), m_max_line_len); j++) {
				grad[j].write(out);
				out.put(line[j]);
			}
		}

		reset.write(out);

		return out;
	}

};