#pragma once

#include <spdlog/sinks/base_sink.h>
class WriteChatSink : public spdlog::sinks::base_sink<spdlog::details::null_mutex>
{
protected:
	void sink_it_(const spdlog::details::log_msg& msg) override
	{
		spdlog::memory_buf_t formatted;
		spdlog::sinks::base_sink<spdlog::details::null_mutex>::formatter_->format(msg, formatted);
		WriteChatf("%s", fmt::to_string(formatted).c_str());
	}

	void flush_() override {}
};

class KnightLog
{
private:
	std::shared_ptr<spdlog::logger> m_logger = nullptr;
	std::shared_ptr<WriteChatSink> m_chatSink = std::make_shared<WriteChatSink>();
	static inline std::map<spdlog::level::level_enum, std::string> m_colorlevels =
	{
		{spdlog::level::trace,    "#FF00FF"},
		{spdlog::level::debug,    "#FF8C00"},
		{spdlog::level::info,     "#FFFFFF"},
		{spdlog::level::warn,     "#FFD700"},
		{spdlog::level::err,      "#F22613"},
		{spdlog::level::critical, "#F22613"}
	};
	class flag_format_carat_colors;
	class flag_format_dollar_colors;
	class flag_format_j_macrosource;
	class flag_format_k_macroline;
	class flag_format_q_macrofull;

	static inline std::string m_logformat = "%^[%T] %L [%n] :: %v%$";

	void set_formatter(const char* logformat = m_logformat.data()) const
	{
		auto formatter = std::make_unique<spdlog::pattern_formatter>();
		formatter->add_flag<flag_format_carat_colors>('^');
		formatter->add_flag<flag_format_dollar_colors>('$');
		formatter->set_pattern(logformat);
		m_chatSink->set_formatter(std::move(formatter));
	}

	enum class MacroInfoLevels
	{
		Line,
		SourceFile,
		Full
	};

	static std::string get_macro_info(MacroInfoLevels infolevel)
	{
		std::string strReturn = "";
		if (MQMacroBlockPtr pBlock = GetCurrentMacroBlock())
		{
			MQMacroLine& ml = pBlock->Line.at(gMacroBlock->CurrIndex);
			switch (infolevel)
			{
			case MacroInfoLevels::Line:
				strReturn = std::to_string(ml.LineNumber);
				break;
			case MacroInfoLevels::SourceFile:
				strReturn = ml.SourceFile;
				break;
			case MacroInfoLevels::Full:
				strReturn = fmt::format("({} :: Line {})", ml.SourceFile, ml.LineNumber);
				break;
			}
		}
		return strReturn;
	}
public:
	KnightLog(const char* logformat = m_logformat.data())
	{
		m_logformat = logformat;
		m_logger = std::make_shared<spdlog::logger>(mqplugin::PluginName);
		m_logger->set_level(spdlog::level::info);
		m_logger->flush_on(spdlog::level::info);

		m_chatSink->set_level(spdlog::level::trace);

		set_formatter(logformat);

		m_logger->sinks().push_back(m_chatSink);

		spdlog::set_default_logger(m_logger);	
	}

	~KnightLog()
	{
		spdlog::shutdown();		
	}

	void SetPattern(const char* pattern) const
	{
		m_logformat = pattern;
		set_formatter(pattern);
	}

	std::string GetLogLevel() const
	{
		switch (m_logger->level())
		{
		case spdlog::level::trace:
			return "trace";
		case spdlog::level::debug:
			return "debug";
		case spdlog::level::info:
			return "info";
		case spdlog::level::warn:
			return "warn";
		case spdlog::level::err:
			return "error";
		case spdlog::level::critical:
			return "critical";
		case spdlog::level::off:
			return "off";
		default:
			return std::to_string(m_logger->level());
		}
	}

	static std::string GetColorByLevel(spdlog::level::level_enum i)
	{
		if(!m_colorlevels.empty() && (m_colorlevels.find(i) != m_colorlevels.end()))
			return m_colorlevels[i];

		return "#000000";
	}

	bool SetColorByLevel(const std::map<spdlog::level::level_enum, std::string>& colormap) const
	{
		if (!colormap.empty())
		{
			bool success_state = true;
			for (const auto& it : colormap)
			{
				if(!SetColorByLevel(it.first, it.second, false))
				{
					success_state = false;
				}
			}
			set_formatter();
			return success_state;
		}

		return false;
	}

	bool SetColorByLevel(spdlog::level::level_enum i, const std::string& color, bool recycle = true) const
	{
		if(!m_colorlevels.empty() && m_colorlevels.find(i) != m_colorlevels.end())
		{
			const int length = static_cast<int>(color.size());
			// Valid colors are single letter, 3 letter, or 6 digit hex
			if (length == 1 || length == 3 || length == 7)
			{
				m_colorlevels[i] = color;
				if (recycle)
				{
					set_formatter();
				}
				return true;
			}
		}

		return false;
	}

	bool SetLogLevel(std::string_view level) const
	{
		// off is intentionally excluded
		if (ci_equals(level, "trace"))
		{
			m_logger->set_level(spdlog::level::trace);
		}
		else if (ci_equals(level, "debug"))
		{
			m_logger->set_level(spdlog::level::debug);
		}
		else if (ci_equals(level, "info"))
		{
			m_logger->set_level(spdlog::level::info);
		}
		else if (ci_equals(level, "warn") || ci_equals(level, "warning"))
		{
			m_logger->set_level(spdlog::level::warn);
		}
		else if (ci_equals(level, "err") || ci_equals(level, "error"))
		{
			m_logger->set_level(spdlog::level::err);
		}
		else if (ci_equals(level, "critical") || ci_equals(level, "fatal"))
		{
			m_logger->set_level(spdlog::level::critical);
		}
		else
		{
			return false;
		}
		m_logger->flush_on(m_logger->level());
		return true;
	}
};
extern KnightLog* knightlog;

class KnightLog::flag_format_carat_colors: public spdlog::custom_flag_formatter
{
public:
    void format(const spdlog::details::log_msg& msg, const std::tm&, spdlog::memory_buf_t& dest) override
    {
		std::string color_code = "\a";

		color_code += GetColorByLevel(msg.level);

    	dest.append(color_code.data(), color_code.data() + color_code.size());
    }

    std::unique_ptr<custom_flag_formatter> clone() const override
    {
        return spdlog::details::make_unique<flag_format_carat_colors>();
    }
};

class KnightLog::flag_format_dollar_colors: public spdlog::custom_flag_formatter
{
public:
    void format(const spdlog::details::log_msg&, const std::tm&, spdlog::memory_buf_t& dest) override
    {
		std::string color_code = "\ax";
        dest.append(color_code.data(), color_code.data() + color_code.size());
    }

    std::unique_ptr<custom_flag_formatter> clone() const override
    {
        return spdlog::details::make_unique<flag_format_dollar_colors>();
    }
};

class KnightLog::flag_format_j_macrosource: public spdlog::custom_flag_formatter
{
public:
    void format(const spdlog::details::log_msg& msg, const std::tm&, spdlog::memory_buf_t& dest) override
    {
		std::string out = get_macro_info(MacroInfoLevels::SourceFile);
    	dest.append(out.data(), out.data() + out.size());
    }

    std::unique_ptr<custom_flag_formatter> clone() const override
    {
        return spdlog::details::make_unique<flag_format_j_macrosource>();
    }
};

class KnightLog::flag_format_k_macroline: public spdlog::custom_flag_formatter
{
public:
    void format(const spdlog::details::log_msg& msg, const std::tm&, spdlog::memory_buf_t& dest) override
    {
		std::string out = get_macro_info(MacroInfoLevels::Line);
    	dest.append(out.data(), out.data() + out.size());
    }

    std::unique_ptr<custom_flag_formatter> clone() const override
    {
        return spdlog::details::make_unique<flag_format_k_macroline>();
    }
};

class KnightLog::flag_format_q_macrofull: public spdlog::custom_flag_formatter
{
public:
    void format(const spdlog::details::log_msg& msg, const std::tm&, spdlog::memory_buf_t& dest) override
    {
		std::string out = get_macro_info(MacroInfoLevels::Full);
    	dest.append(out.data(), out.data() + out.size());
    }

    std::unique_ptr<custom_flag_formatter> clone() const override
    {
        return spdlog::details::make_unique<flag_format_q_macrofull>();
    }
};

