#include "logger.h"
#include <direct.h>
#include<cassert>
#include"spdlog/sinks/daily_file_sink.h"
#include"spdlog/sinks/rotating_file_sink.h"
#include"spdlog/sinks/stdout_color_sinks.h"
#include"spdlog/sinks/stdout_sinks.h"
#include"spdlog/pattern_formatter.h"

namespace sun {
	Logger::Logger() {
#if __cplusplus < 201103L
		//非C++11
#endif
		_name = "logger";
		_filePath = "../log/run.log";
		_target = RotateFile;
		_level = Trace;
		_rotateFileSize = 1024 * 1024 * 100;
		_rotateFileCount = 3;
		_dayFileTimeHour = 0;
		_dayFileTimeMinute = 0;
		_logger = nullptr;
		spdlog::set_error_handler([](const std::string& err_msg) {
			//	std::cout << fmt::format("[{}:{} {}()] {}", __FILE__, __LINE__, __FUNCTION__, err_msg) << std::endl;
			});	
	}

	Logger::~Logger() {
		spdlog::shutdown();
	//	spdlog::drop_all();
	}

	short Logger::init() {
		try {
			switch (_target)
			{
				case Console:			_logger = spdlog::stdout_color_mt(_name);	break;
				case DayFile: {
					auto dayFileSink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(_filePath, _dayFileTimeHour, _dayFileTimeMinute);
					dayFileSink->set_pattern("%^[%Y-%m-%d %H:%M:%S:%l:%s(%#)] %v%$");
					_logger = std::make_shared<spdlog::logger>(_name, dayFileSink);
					break;
				}
				case RotateFile: {
#if 0
					if (0 != createFilePath(_filePath)) {
						_error = "create log file directory failed";
						return 1;
					}
#endif
					auto rotateFileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(_filePath, _rotateFileSize, _rotateFileCount);
					rotateFileSink->set_pattern("%^[%Y-%m-%d %H:%M:%S:%l:%s(%#)] %v%$");
					_logger = std::make_shared<spdlog::logger>(_name, rotateFileSink);
					break;
				}
				case MultiMode: {
					auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
					auto rotateFileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(_filePath, _rotateFileSize, _rotateFileCount);
					consoleSink->set_level(spdlog::level::trace);
					rotateFileSink->set_level(spdlog::level::info);
					consoleSink->set_pattern("%^[%Y-%m-%d %H:%M:%S:%l:%s(%#)] %v%$");
					rotateFileSink->set_pattern("%^[%Y-%m-%d %H:%M:%S:%l:%s(%#)] %v%$");
					std::vector<spdlog::sink_ptr>sinks;
					sinks.push_back(consoleSink);
					sinks.push_back(rotateFileSink);
					_logger = std::make_shared<spdlog::logger>(_name, sinks.begin(), sinks.end());
					break;
				}
				default:;
			}
		}
		catch (const std::exception &ex) {
			_error = std::string("logger init failed: ") + ex.what();
			return -1;
		}
		catch (...) {
			_error = "logger init failed: unknown reason";
			return -1;
		}
		setLevel();
		_error = "ok";
		spdlog::set_pattern("%^[%Y-%m-%d %H:%M:%S:%l:%s(%#)] %v%$");
		return 0;
	}

	void Logger::flush() {
		assert(_logger);
		_logger->flush();
	}

	void Logger::setLevel(Level level) {
		_level = level;
	}

	std::shared_ptr<spdlog::logger> Logger::handler() {
		return _logger;
	}

	void Logger::setLevel() {
		assert(_logger);
		spdlog::level::level_enum le;
		switch (_level)
		{
			case Trace:		le = spdlog::level::trace;		break;
			case Debug:	le = spdlog::level::debug;		break;
			case Info:		le = spdlog::level::info;			break;
			case Warn:		le = spdlog::level::warn;		break;
			case Error:		le = spdlog::level::err;			break;
			case Critical:	le = spdlog::level::critical;		break;
			default:			le = spdlog::level::trace;
		}
		_logger->set_level(le);
	}

	std::string Logger::error() {
		return _error;
	}

#if 0
	int Logger::createFilePath(const std::string& path) {
		std::string replacePath = path;
		std::vector<std::string> paths;
		paths.clear();

#ifdef _WIN32
		//replace "\\" to "/" if path contain "\\"
		size_t replacePos = 0;
		while (replacePos != std::string::npos) {
			size_t p = replacePath.find_first_of("\\", replacePos);
			if (std::string::npos == p) {
				break;
			}
			replacePath.replace(p,1, "/");
			p += 1;
			replacePos = p;
		}
#endif
		//split path by '/'
		size_t pos = 0;
		while (pos != std::string::npos) {
			size_t p = replacePath.find_first_of("/", pos);
			if (std::string::npos == p) {
				break;
			}
			p += 1;
			paths.push_back(replacePath.substr(pos, p - pos));
			pos = p;
		}
		//create path if path is not exist
		std::string tmp = "";
		int ret = 0;
		for (auto it = paths.begin(); it != paths.end(); it++){
			tmp += *it;
			if (0 == _access(tmp.c_str(), 0)) {
				continue;
			}
			ret = _mkdir(tmp.c_str());
			if (0 != ret) {
				break;
			}
		}
		return ret;
	}
#endif

	void Logger::setName(const std::string& name) {
		_name = name;
	}

	std::string Logger::name()const {
		return  _name;
	}

	void Logger::setPath(const std::string& path) {
		_filePath = path;
	}

	std::string Logger::path()const {
		return _filePath;
	}

	void Logger::setTarget(Target target) {
		_target = target;
	}

	Logger::Target Logger::target()const {
		return _target;
	}

	Logger::Level Logger::level() const {
		return _level;
	}

	void Logger::setRotateFileSize(int size) {
		_rotateFileSize = size;
	}

	int Logger::rotateFileSize() const {
		return _rotateFileSize;
	}

	void Logger::setDayFileCreateTime(int hour, int minute) {
		_dayFileTimeHour = hour;
		_dayFileTimeMinute = minute;
	}
}
