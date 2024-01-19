////////////////////////////////////////////////////////////////////////
//
// This file is part of sc4-cpu-priority, a DLL Plugin for SimCity 4
// that provides more CPU priority options.
//
// Copyright (c) 2024 Nicholas Hayes
//
// This file is licensed under terms of the MIT License.
// See LICENSE.txt for more information.
//
////////////////////////////////////////////////////////////////////////

#include "Logger.h"
#include "version.h"
#include "cIGZCmdLine.h"
#include "cIGZFrameWork.h"
#include "cRZBaseString.h"
#include "cRZCOMDllDirector.h"
#include <string>

#include <Windows.h>
#include "wil/resource.h"
#include "wil/result.h"
#include "wil/win32_helpers.h"

static constexpr uint32_t kCPUPriorityDllDirector = 0x04E4C618;

static constexpr std::string_view PluginLogFileName = "SC4CPUPriority.log";

namespace
{
	std::filesystem::path GetDllFolderPath()
	{
		wil::unique_cotaskmem_string modulePath = wil::GetModuleFileNameW(wil::GetModuleInstanceHandle());

		std::filesystem::path temp(modulePath.get());

		return temp.parent_path();
	}

	bool EqualsIgnoreCase(const std::string& lhs, const std::string& rhs)
	{
		return lhs.length() == rhs.length()
			&& std::equal(
				lhs.cbegin(),
				lhs.cend(),
				rhs.cbegin(),
				rhs.cend(),
				[](unsigned char a, unsigned char b) { return std::toupper(a) == std::toupper(b); });
	}

	void ProcessCPUPriorityValue(const std::string& priority)
	{
		Logger& logger = Logger::GetInstance();

		DWORD cpuPriority = 0;

		if (EqualsIgnoreCase(priority, "High"))
		{
			cpuPriority = HIGH_PRIORITY_CLASS;
		}
		else if (EqualsIgnoreCase(priority, "AboveNormal"))
		{
			cpuPriority = ABOVE_NORMAL_PRIORITY_CLASS;
		}
		else if (EqualsIgnoreCase(priority, "Normal"))
		{
			// Normal should be the default for a new process, but there is no harm in
			// allowing the user to select it anyway.
			cpuPriority = NORMAL_PRIORITY_CLASS;
		}
		else if (EqualsIgnoreCase(priority, "BelowNormal"))
		{
			cpuPriority = BELOW_NORMAL_PRIORITY_CLASS;
		}
		else if (EqualsIgnoreCase(priority, "Idle"))
		{
			cpuPriority = IDLE_PRIORITY_CLASS;
		}
		else
		{
			if (EqualsIgnoreCase(priority, "Low"))
			{
				// We treat the games's built-in value of Low as a no-op, because it
				// will have already been applied before the DLL is loaded.
				logger.WriteLine(LogLevel::Info, "SC4 set its CPU priority to Low.");
			}
			else
			{
				logger.WriteLineFormatted(
					LogLevel::Error,
					"Unsupported CPU priority value: %s",
					priority.c_str());
			}
		}

		if (cpuPriority != 0)
		{
			try
			{
				THROW_IF_WIN32_BOOL_FALSE(SetPriorityClass(GetCurrentProcess(), cpuPriority));

				logger.WriteLineFormatted(
					LogLevel::Info,
					"Set the game's CPU priority to %s.",
					priority.c_str());
			}
			catch (const wil::ResultException& e)
			{
				logger.WriteLineFormatted(
					LogLevel::Error,
					"An OS error occurred when setting the CPU priority: %s.",
					e.what());
			}
		}
	}
}

class CPUPriorityDllDirector : public cRZCOMDllDirector
{
public:

	CPUPriorityDllDirector()
	{
		std::filesystem::path dllFolderPath = GetDllFolderPath();

		std::filesystem::path logFilePath = dllFolderPath;
		logFilePath /= PluginLogFileName;

		Logger& logger = Logger::GetInstance();
		logger.Init(logFilePath, LogLevel::Error);
		logger.WriteLogFileHeader("SC4CPUPriority v" PLUGIN_VERSION_STR);
	}

	uint32_t GetDirectorID() const
	{
		return kCPUPriorityDllDirector;
	}

	bool OnStart(cIGZCOM* pCOM)
	{
		cIGZFrameWork* const pFramework = RZGetFramework();

		const cIGZCmdLine* pCmdLine = pFramework->CommandLine();

		cRZBaseString cpuPriorityValue;
		if (pCmdLine->IsSwitchPresent(cRZBaseString("CPUPriority"), cpuPriorityValue, true))
		{
			// We extend the -CPUPriority command line argument with a few more supported values.
			// SC4 only supports 1 value -CPUPriority:Low, which we treat as a no-op because the
			// game will have already set it when the DLL is loaded.
			ProcessCPUPriorityValue(cpuPriorityValue.ToChar());
		}
		else
		{
			Logger& logger = Logger::GetInstance();

			logger.WriteLine(LogLevel::Info, "The -CPUPriority command line argument was not found.");
		}

		return true;
	}
};

cRZCOMDllDirector* RZGetCOMDllDirector() {
	static CPUPriorityDllDirector sDirector;
	return &sDirector;
}