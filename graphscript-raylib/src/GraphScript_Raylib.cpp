#include "GraphScript_Raylib.h"
#include "raylib.h"
#include "BuiltInNodes.h"
using namespace gs;

void gs::AddRaylib(Context& c)
{
	CustomNode* raylibInitNode = new CustomNode("Raylib_Init");
	raylibInitNode->AddDataInput<u32>("width");
	raylibInitNode->AddDataInput<u32>("height");
	raylibInitNode->AddDataInput<u32>("target-fps");
	raylibInitNode->OnProcess([](auto inDataSockets, auto outDataSockets, auto inExeSockets, auto outExeSockets)
		{
			DataSocketT<u32>* w_soc = (DataSocketT<u32>*) inDataSockets["width"];
			DataSocketT<u32>* h_soc = (DataSocketT<u32>*) inDataSockets["height"];
			DataSocketT<u32>* fps_soc = (DataSocketT<u32>*) inDataSockets["height"];

			if (!w_soc->m_Value.has_value() || !h_soc->m_Value.has_value() || !fps_soc->m_Value.has_value())
			{
				return;
			}

			InitWindow(w_soc->Get().value(), h_soc->Get().value(), "GraphScript Raylib Window");
			SetTargetFPS(fps_soc->Get().value());
		}
	);

	CustomNode* raylibDrawTextNode = new CustomNode("Raylib_DrawText");
	raylibDrawTextNode->AddDataInput<String>("input");
	raylibDrawTextNode->AddDataInput<u32>("x");
	raylibDrawTextNode->AddDataInput<u32>("y");
	raylibDrawTextNode->AddDataInput<u32>("size");
	raylibDrawTextNode->OnProcess([](auto inDataSockets, auto outDataSockets, auto inExeSockets, auto outExeSockets)
		{
			DataSocketT<String>* input_soc = (DataSocketT<String>*) inDataSockets["input"];
			DataSocketT<u32>* x_soc = (DataSocketT<u32>*) inDataSockets["x"];
			DataSocketT<u32>* y_soc = (DataSocketT<u32>*) inDataSockets["y"];
			DataSocketT<u32>* size_soc = (DataSocketT<u32>*) inDataSockets["size"];

			if (!x_soc->m_Value.has_value() || !y_soc->m_Value.has_value() || !size_soc->m_Value.has_value() || !input_soc->m_Value.has_value())
			{
				return;
			}

			DrawText(input_soc->Get().value().c_str(), x_soc->Get().value(), y_soc->Get().value(), size_soc->Get().value(), RAYWHITE);
		}
	);

	CustomNode* raylibWindowShouldCloseNode = new CustomNode("Raylib_WindowShouldClose");
	raylibWindowShouldCloseNode->AddDataOutput<bool>("shouldClose");
	raylibWindowShouldCloseNode->OnProcess([](auto inDataSockets, auto outDataSockets, auto inExeSockets, auto outExeSockets)
		{
			DataSocketT<bool>* run_soc = (DataSocketT<bool>*) outDataSockets["shouldClose"];
			bool close = !WindowShouldClose();
			run_soc->Set(close);
		}
	);

	CustomNode* raylibBeginDrawingNode = new CustomNode("Raylib_BeginDrawing");
	raylibBeginDrawingNode->OnProcess([](auto inDataSockets, auto outDataSockets, auto inExeSockets, auto outExeSockets)
		{
			BeginDrawing();
			ClearBackground(BLACK);
		}
	);

	CustomNode* raylibEndDrawingNode = new CustomNode("Raylib_EndDrawing");
	raylibEndDrawingNode->OnProcess([](auto inDataSockets, auto outDataSockets, auto inExeSockets, auto outExeSockets)
		{
			EndDrawing();
		}
	);

	CustomNode* raylibCloseWindowNode = new CustomNode("Raylib_CloseWindow");
	raylibCloseWindowNode->OnProcess([](auto inDataSockets, auto outDataSockets, auto inExeSockets, auto outExeSockets)
		{
			CloseWindow();
		}
	);


	c.AddNode(raylibInitNode);
	c.AddNode(raylibDrawTextNode);
	c.AddNode(raylibWindowShouldCloseNode);
	c.AddNode(raylibBeginDrawingNode);
	c.AddNode(raylibEndDrawingNode);
	c.AddNode(raylibCloseWindowNode);
}
