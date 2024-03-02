#include "Utils.h"

void gs::utils::SaveStringAtPath(String& str, const String& p_UserPath)
{
	// save builder to string
	FStream outFile = FStream(p_UserPath);
	outFile << str;
	outFile.close();

}

void gs::utils::SaveGraphAtPath(GraphBuilder* builder, const String& p_UserPath)
{
	// save builder to string
	String graphString = builder->Serialize();
	SaveStringAtPath(graphString, p_UserPath);
}

gs::String gs::utils::LoadStringAtPath(const String& p_UserPath)
{
	std::ifstream t(p_UserPath);
	SStream buffer;
	buffer << t.rdbuf();
	return buffer.str();

}

gs::Vector<gs::String> gs::utils::SplitStringByChar(const gs::String& str, char c)
{
	using namespace gs;
	auto result = Vector<String>{};
	auto ss = SStream{ str };

	for (String line; std::getline(ss, line, c);)
		result.push_back(line);

	return result;
}

void gs::utils::Trim(gs::String& s)
{
	s.erase(std::remove_if(s.begin(), s.end(), isspace), s.end());
}

