#pragma once
#define SPDLOG_ACTIVE_LEVEL		SPDLOG_LEVEL_TRACE
#include<memory>
#include"spdlog/spdlog.h"
//download spdlog https://github.com/gabime/spdlog 


#ifdef LOGGER_COMPILER_LIB
	#ifdef _WIN32
		#ifdef LOGGER_EXPORT
			#define LOGGER_API __declspec(dllexport)
		#else
			#define LOGGER_API __declspec(dllimport)
		#endif
	#else
		#define LOGGER_API __attribute__((visibility ("default")))
	#endif
#else
	#define LOGGER_API
#endif

namespace sun {
	class LOGGER_API Logger {
	public:
		static Logger& instance() {
			static Logger logger;
			return logger;
		}
		enum Level {
			Trace = 0,
			Debug,
			Info,
			Warn,
			Error,
			Critical
		};

		enum Target {
			Console,
			DayFile,
			RotateFile,
			MultiMode					// console and rotate file 
		};

	public:
		~Logger();
		short init();
		void flush();
		std::string error();			
		std::shared_ptr<spdlog::logger> handler();
		void setName(const std::string& name);
		std::string name()const;
		void setPath(const std::string& path);
		std::string path()const;
		void setTarget(Target target);
		Target target()const;
		void setLevel(Level level);
		Level level() const;
		void setRotateFileSize(int size);
		int rotateFileSize() const;
		void setDayFileCreateTime(int hour = 0, int minute = 0);
	private:
		Logger();
		Logger(const Logger&) = delete;
		void  operator=(const Logger&) = delete;
	//	int createFilePath(const std::string& path);
		void setLevel();
	private:
		std::shared_ptr<spdlog::logger> _logger;
		std::string _name;					//default:logger				
		std::string _filePath;				//default:../log/run.log
		Target _target;						//default: Console
		Level _level;							//default: Trace
		int _rotateFileSize;					//default:100MB
		int _rotateFileCount;				//default:3
		int _dayFileTimeHour;			//default:0
		int _dayFileTimeMinute;		//default:0
		std::string _error;					//default: ok
	};
}

#define LOG_INSTANCE								sun::Logger::instance()
#define LOG_TRACE(...)								do{SPDLOG_LOGGER_TRACE(LOG_INSTANCE.handler(),__VA_ARGS__);LOG_INSTANCE.flush();}while(0)
#define LOG_DEBUG(...)							do{SPDLOG_LOGGER_DEBUG(LOG_INSTANCE.handler(),__VA_ARGS__);LOG_INSTANCE.flush();}while(0)
#define LOG_INFO(...)									do{SPDLOG_LOGGER_INFO(LOG_INSTANCE.handler(),__VA_ARGS__);LOG_INSTANCE.flush();}while(0)
#define LOG_WARN(...)								do{SPDLOG_LOGGER_WARN(LOG_INSTANCE.handler(),__VA_ARGS__);LOG_INSTANCE.flush();}while(0)
#define LOG_ERROR(...)								do{SPDLOG_LOGGER_ERROR(LOG_INSTANCE.handler(),__VA_ARGS__);LOG_INSTANCE.flush();}while(0)
#define LOG_CRITICAL(...)						do{SPDLOG_LOGGER_CRITICAL(LOG_INSTANCE.handler(),__VA_ARGS__);LOG_INSTANCE.flush();}while(0)