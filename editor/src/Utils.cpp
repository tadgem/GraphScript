#include "Utils.h"

void gs::utils::SaveStringAtPath(String& str, const String& path)
{
	// save builder to string
	FStream outFile = FStream(path);
	outFile << str;
	outFile.close();

}

void gs::utils::SaveGraphAtPath(GraphBuilder* builder, const String& path)
{
	// save builder to string
	String graphString = builder->Serialize();
	SaveStringAtPath(graphString, path);
}
