#pragma once

#include "Includes.h"

#include "Globals.h"

namespace LeagueFuncs
{
	typedef void(__thiscall* tSendChat)(DWORD ChatClient, const char* Message, int Color);
	static void SendChat(const char* message, int color = 0)
	{
		static tSendChat fnSendChat = (tSendChat)(Globals::dwBaseAddress + oSendChat);
		fnSendChat(*(DWORD*)(Globals::dwBaseAddress + oChatClientPtr), message, color);
	}

	// could be unsafe, haven't checked
	typedef void(__thiscall* tPrintChat)(DWORD ChatClient, const char* Message, int Color);
	static void PrintChat(const char* message, int color = 0)
	{
		static tPrintChat fnPrintChat = (tPrintChat)(Globals::dwBaseAddress + oPrintChat);
		fnPrintChat(*(DWORD*)(Globals::dwBaseAddress + oChatClientPtr), message, color);
	}

	static Matrix4x4 GetViewMatrix()
	{
		return *reinterpret_cast<Matrix4x4*>(Globals::dwBaseAddress + oViewMatrix);
	}

	static Matrix4x4 GetProjectionMatrix()
	{
		return *reinterpret_cast<Matrix4x4*>(Globals::dwBaseAddress + oViewMatrix + 0x40);
	}

	static ImVec2 WorldToScreen(const Vector3& pos)
	{
		float matrix[16];

		Matrix4x4::MultiplyMatrices(matrix, GetViewMatrix().matrix, GetProjectionMatrix().matrix);

		ImVec2 returnVec = ImVec2(0, 0);

		Vector4 clipCoords;
		clipCoords.x = pos.x * matrix[0] + pos.y * matrix[4] + pos.z * matrix[8] + matrix[12];
		clipCoords.y = pos.x * matrix[1] + pos.y * matrix[5] + pos.z * matrix[9] + matrix[13];
		clipCoords.z = pos.x * matrix[2] + pos.y * matrix[6] + pos.z * matrix[10] + matrix[14];
		clipCoords.w = pos.x * matrix[3] + pos.y * matrix[7] + pos.z * matrix[11] + matrix[15];

		//clipCoords.X = pos.X * matrix[0] + pos.Y * matrix[1] + pos.Z * matrix[2] + matrix[3];
		//clipCoords.Y = pos.X * matrix[4] + pos.Y * matrix[5] + pos.Z * matrix[6] + matrix[7];
		//clipCoords.Z = pos.X * matrix[8] + pos.Y * matrix[9] + pos.Z * matrix[10] + matrix[11];
		//clipCoords.W = pos.X * matrix[12] + pos.Y * matrix[13] + pos.Z * matrix[14] + matrix[15];

		if (clipCoords.w < 0.1f) return returnVec;

		Vector3 temp;
		temp.x = clipCoords.x / clipCoords.w;
		temp.y = clipCoords.y / clipCoords.w;
		temp.z = clipCoords.z / clipCoords.w;

		// todo move this
		static DWORD dwRenderer = *reinterpret_cast<DWORD*>(Globals::dwBaseAddress + oRenderer);
		static ImVec2 windowSize = ImVec2(*reinterpret_cast<int*>(dwRenderer + oRendererWidth), *reinterpret_cast<int*>(dwRenderer + oRendererHeight));

		returnVec.x = (windowSize.x / 2.0f * temp.x) + (temp.x + windowSize.x / 2.0f);
		returnVec.y = -(windowSize.y / 2.0f * temp.y) + (temp.y + windowSize.y / 2.0f);

		return returnVec;
	}
}