#include "stdafx.h"
#include "Commands.h"
#include "API/Base.h"
#include <vector>
#include <algorithm>

namespace Commands
{
	// Commands containers

	std::vector<ChatCommand*> chatCommands;
	std::vector<ConsoleCommand*> consoleCommands;
	std::vector<RconCommand*> rconCommands;
	std::vector<std::function<void(float)>> onTickCallbacks;
	std::vector<std::function<void()>> onTimerCallbacks;

	// Add commands

	void AddChatCommand(const FString& command, const std::function<void(AShooterPlayerController*, FString*, EChatSendMode::Type)>& callback)
	{
		ChatCommand* chatCommand = new ChatCommand(command, callback);
		chatCommands.push_back(chatCommand);
	}
	
	void RemoveChatCommand(const FString& command)
	{
		auto& v = chatCommands;

		auto iter = std::find_if(v.begin(), v.end(), [command](ChatCommand* data) -> bool { return data->command == command; });

		if (iter != v.end())
		{
			v.erase(std::remove(v.begin(), v.end(), *iter), v.end());
		}
	}

	void AddConsoleCommand(const FString& command, const std::function<void(APlayerController*, FString*, bool)>& callback)
	{
		ConsoleCommand* consoleCommand = new ConsoleCommand(command, callback);
		consoleCommands.push_back(consoleCommand);
	}

	void AddRconCommand(const FString& command, const std::function<void(RCONClientConnection*, RCONPacket*, UWorld*)>& callback)
	{
		RconCommand* rconCommand = new RconCommand(command, callback);
		rconCommands.push_back(rconCommand);
	}

	void AddOnTickCallback(const std::function<void(float)>& callback)
	{
		onTickCallbacks.push_back(callback);
	}
	
	void AddOnTimerCallback(const std::function<void()>& callback)
	{
		onTimerCallbacks.push_back(callback);
	}

	// Callbacks

	bool CheckChatCommands(AShooterPlayerController* _AShooterPlayerController, FString* Message, EChatSendMode::Type Mode)
	{
		bool result = false;

		TArray<FString> Parsed;
		Message->ParseIntoArray(&Parsed, L" ", true);

		if (!Parsed.IsValidIndex(0))
			return false;

		FString chatCommand = Parsed[0];

		for (auto command : chatCommands)
		{
			if (chatCommand.EndsWith(&command->command, ESearchCase::IgnoreCase))
			{
				command->callback(_AShooterPlayerController, Message, Mode);

				result = true;
			}
		}

		return result;
	}

	bool CheckConsoleCommands(APlayerController* _APlayerController, FString* Cmd, bool bWriteToLog)
	{
		bool result = false;

		for (auto command : consoleCommands)
		{
			if (Cmd->StartsWith(command->command, ESearchCase::IgnoreCase))
			{
				command->callback(_APlayerController, Cmd, bWriteToLog);

				result = true;
			}
		}

		return result;
	}

	bool CheckRconCommands(RCONClientConnection* rconClientConnection, RCONPacket* rconPacket, UWorld* uWorld)
	{
		bool result = false;

		FString commandName = rconPacket->Body;

		for (auto command : rconCommands)
		{
			if (commandName.StartsWith(command->command, ESearchCase::IgnoreCase))
			{
				command->callback(rconClientConnection, rconPacket, uWorld);

				result = true;
			}
		}

		return result;
	}

	void CheckOnTickCallbacks(float DeltaSeconds)
	{
		for (auto callback : onTickCallbacks)
		{
			callback(DeltaSeconds);
		}
	}
	
	void CheckOnTimerCallbacks()
	{
		for (auto callback : onTimerCallbacks)
		{
			callback();
		}
	}
}
