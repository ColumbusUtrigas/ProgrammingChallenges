#include <tuple>
#include <string>
#include <fstream>
#include <iomanip>
#include <optional>
#include <iostream>
#include <string_view>
#include <unordered_map>
#include "json.hpp"

using namespace std::string_literals;

enum class NoteState
{
	NotDone,
	InProgress,
	Done,
	Deferred
};

std::ostream& operator<<(std::ostream& Stream, NoteState State)
{
	switch (State)
	{
	case NoteState::NotDone: Stream << "Not done"; break;
	case NoteState::InProgress: Stream << "In progress"; break;
	case NoteState::Done: Stream << "Done"; break;
	case NoteState::Deferred: Stream << "Deferred"; break;
	}

	return Stream;
}

enum class Command
{
	Add,
	Edit,
	Update,
	Exit,
	List
};

NLOHMANN_JSON_SERIALIZE_ENUM(NoteState, {
	{ NoteState::NotDone, "NotDone" },
	{ NoteState::InProgress, "InProgress" },
	{ NoteState::Done, "Done" },
	{ NoteState::Deferred, "Deferred" },
})

struct Note
{
	std::string Description;
	NoteState State;
};

void to_json(nlohmann::json& J, const Note& Value)
{
	J = nlohmann::json{ {"Description", Value.Description}, {"State", Value.State} };
}

void from_json(const nlohmann::json& J, Note& Value)
{
	J["Description"].get_to(Value.Description);
	J["State"].get_to(Value.State);
}

class DataBase
{
private:
	std::unordered_map<std::string, Note> NotesList;
	std::string Name;
	bool Modified = false;

	bool Create(std::string_view FileName);
	bool Read(std::ifstream& DB);
public:
	DataBase() = default;
	DataBase(std::string_view FileName) { if (!Open(FileName)) throw std::exception("Cannot open DB"); }
	DataBase(const DataBase& Base) : NotesList(Base.NotesList), Name(Base.Name) {}
	DataBase(DataBase&& Base) noexcept : NotesList(std::exchange(Base.NotesList, {})), Name(std::exchange(Base.Name, {})) {}
	~DataBase() = default;

	DataBase& operator=(const DataBase& Base) { NotesList = Base.NotesList; Name = Base.Name; return *this; }
	DataBase& operator=(DataBase&& Base) noexcept { std::swap(NotesList, Base.NotesList); std::swap(Name, Base.Name); return *this; }

	auto begin() const { return NotesList.begin(); }
	auto end() const { return NotesList.end(); }

	bool Open(std::string_view FileName);
	bool Update();
	bool WasModified() const { return Modified; }

	bool Add(const std::string& Name, const Note& Value)
	{
		NotesList[Name] = Value;
		Modified = true;
		return true;
	}

	bool Contains(const std::string& Name) const
	{
		if (NotesList.find(Name) == NotesList.end()) return false;
		return true;
	}
};

bool DataBase::Create(std::string_view FileName)
{
	std::ofstream File(FileName.data());
	if (!File.is_open()) return false;
	return true;
}

bool DataBase::Open(std::string_view FileName)
{
	std::ifstream DB;

	for (size_t i = 0; i < 1000; i++)
	{
		DB.open(FileName.data());
		if (!DB.is_open())
		{
			if (!Create(FileName)) return false;
			continue;
		}
		break;
	}

	if (!DB.is_open()) return false;

	Name = std::string(FileName);
	return Read(DB);
}

bool DataBase::Read(std::ifstream& DB)
{
	nlohmann::json J;
	DB >> J;

	for (const auto& Elem : J.items())
	{
		NotesList[Elem.key().c_str()] = std::move(Elem.value().get<Note>());
	}

	return true;
}

bool DataBase::Update()
{
	std::ofstream DB(Name);
	if (!DB.is_open()) return false;

	nlohmann::json J;

	for (const auto& Elem : NotesList)
	{
		J[Elem.first.data()] = Elem.second;
	}

	DB << std::setw(4) << J << std::endl;
	Modified = false;

	return true;
}

std::optional<Command> ReinterpretCommand(std::string&& Cmd)
{
	std::transform(Cmd.begin(), Cmd.end(), Cmd.begin(), tolower);

	if (Cmd == "add") return Command::Add;
	if (Cmd == "edit") return Command::Edit;
	if (Cmd == "update") return Command::Update;
	if (Cmd == "exit") return Command::Exit;
	if (Cmd == "list") return Command::List;

	return {};
}

std::string ReadString(std::string_view Out)
{
	std::string Result;
	std::cout << Out << std::flush;
	std::getline(std::cin, Result);
	return Result;
}

NoteState ReinterpretState(std::string&& State)
{
	std::transform(State.begin(), State.end(), State.begin(), tolower);

	if (State == "not done" || State == "notdone") return NoteState::NotDone;
	if (State == "in progress" || State == "inprogress") return NoteState::InProgress;
	if (State == "done") return NoteState::Done;
	if (State == "deferred") return NoteState::Deferred;

	return NoteState::NotDone;
}

Note ReadNoteWithoutName()
{
	std::string Description = ReadString('\t' + "Enter description: "s);
	std::string State = ReadString('\t' + "Enter initial state: "s);

	Note Value;
	Value.Description = std::move(Description);
	Value.State = ReinterpretState(std::move(State));
	return Value;
}

std::tuple<std::string, Note> ReadNote()
{
	std::string Name = ReadString('\t' + "Enter name: "s);

	return { Name, ReadNoteWithoutName() };
}

void CommandAdd(DataBase& DB)
{
	auto Tuple = ReadNote();

	if (DB.Contains(std::get<std::string>(Tuple)))
	{
		std::string Accept = ReadString('\t' + "This note is already in the database, do you want to replace it?"s);
		std::transform(Accept.begin(), Accept.end(), Accept.begin(), tolower);
		if (Accept == "y" || Accept == "yes")
		{
			if (!DB.Add(std::get<std::string>(Tuple), std::get<Note>(Tuple)))
			{
				std::cout << "Error: couldn't add note" << std::endl;
			}
		}
		else
		{
			return;
		}
	}
	else
	{
		if (!DB.Add(std::get<std::string>(Tuple), std::get<Note>(Tuple)))
		{
			std::cout << "Error: couldn't add note" << std::endl;
		}
	}

	std::cout << "\t" << "Note successfully added" << std::endl;
}

void CommandEdit(DataBase& DB)
{
	if (auto Name = ReadString('\t' + "Enter name: "s); DB.Contains(Name))
	{
		if (!DB.Add(Name, ReadNoteWithoutName()))
		{
			std::cout << '\t' << "Error: couldn't update note" << std::endl;
		}
	}
	else
	{
		std::string Accept = ReadString('\t' + "This note is not already in database, do you want to add new? "s);
		std::transform(Accept.begin(), Accept.end(), Accept.begin(), tolower);
		if (Accept == "y" || Accept == "yes")
		{
			std::cout << std::endl;
			CommandAdd(DB);
		}
	}
}

void CommandExit(DataBase& DB)
{
	if (DB.WasModified())
	{
		std::string Accept = ReadString('\t' + "Database was modified, do you want to update it before exit? "s);
		std::transform(Accept.begin(), Accept.end(), Accept.begin(), tolower);
		if (Accept == "y" || Accept == "yes")
		{
			DB.Update();
		}
	}
}

void CommandList(DataBase& DB)
{
	size_t Counter = 0;

	for (const auto& Elem : DB)
	{
		std::cout << '[' << Counter++ << "] " << Elem.first << std::endl;
		std::cout << '\t' << "Description: " << Elem.second.Description << std::endl;
		std::cout << '\t' << "State: " << Elem.second.State << std::endl << std::endl;
	}
}

int main(int argc, char** argv)
{
	DataBase DB;
	try
	{
		DB = std::move(DataBase("notes.db"));
	}
	catch (std::exception E)
	{
		std::cout << E.what() << std::endl;
		std::cin.get();
		return 1;
	}

	while (true)
	{
		std::string Cmd = ReadString("Enter command: ");

		if (auto Opt = ReinterpretCommand(std::move(Cmd)); Opt.has_value())
		{
			switch (Opt.value())
			{
			case Command::Add:  CommandAdd(DB);  break;
			case Command::Edit: CommandEdit(DB); break;
			case Command::Update: DB.Update(); break;
			case Command::Exit: CommandExit(DB); return 0; break;
			case Command::List: CommandList(DB); break;
			}
		}
		else
		{
			std::cout << "Undefined command" << std::endl;
		}
	}

	return 0;
}


