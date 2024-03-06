#include "RuntimeUtils.h"

using namespace gs;

void utils::SaveStringAtPath(String& str, const String& p_UserPath)
{
	// save builder to string
	FStream outFile = FStream(p_UserPath);
	outFile << str;
	outFile.close();

}

void utils::SaveGraphAtPath(GraphBuilder* builder, const String& p_UserPath)
{
	// save builder to string
	String graphString = builder->Serialize();
	SaveStringAtPath(graphString, p_UserPath);
}

String utils::LoadStringAtPath(const String& p_UserPath)
{
	std::ifstream t(p_UserPath);
	SStream buffer;
	buffer << t.rdbuf();
	return buffer.str();

}

Vector<String> utils::SplitStringByChar(const String& str, char c)
{
	using namespace gs;
	auto result = Vector<String>{};
	auto ss = SStream{ str };

	for (String line; std::getline(ss, line, c);)
		result.push_back(line);

	return result;
}

void utils::Trim(String& s)
{
	s.erase(std::remove_if(s.begin(), s.end(), isspace), s.end());
	s.erase(std::remove_if(s.begin(), s.end(), isblank), s.end());
	s.erase(std::remove_if(s.begin(), s.end(), [](char c) { return c == '\0'; }), s.end());
	s.shrink_to_fit();
}

GraphBuilder* utils::ParseGraph(Context& c, String line)
{
	String source = utils::LoadStringAtPath(line);
	GraphBuilder* builder = c.DeserializeGraph(source);
	return builder;
}

RuntimeVariableSet utils::ParseVariableSet(Context& c, String line)
{
	Vector<String> parts = utils::SplitStringByChar(line, ',');

	GS_ASSERT(parts.size() > 0, "Too few parts to be a variable set");

	if (parts.size() == 1)
	{
		return {};
	}

	RuntimeVariableSet vars;

	for (int i = 1; i < parts.size(); i++)
	{
		Vector<String> components = utils::SplitStringByChar(parts[i], ':');
		GS_ASSERT(components.size() == 3, "Incorrect number of parts");
		HashString name = components[0];
		u64 typeHash = std::stoull(components[1]);
		Any val = utils::StringToAny(components[2], typeHash);

		for (auto& proto : c.GetAllVariables())
		{
			if (proto->m_Type.m_TypeHash.m_Value == typeHash)
			{
				Variable* clone = proto->Clone();
				clone->SetValue(val);
				vars.emplace(name, clone);
			}
		}
	}
	return vars;

}

Graph* utils::ParseGraphInstance(Context& c, String line)
{
	return nullptr;
}

