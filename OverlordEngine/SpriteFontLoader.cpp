#include "stdafx.h"
#include "SpriteFontLoader.h"
#include "BinaryReader.h"
#include "ContentManager.h"
#include "TextureData.h"

unsigned short JoingUShort(unsigned char a, unsigned char b)
{
	return (a << CHAR_BIT) + b;
}

#define JOIN_U_SHORT(a, b) (a << CHAR_BIT) + b

SpriteFont* SpriteFontLoader::LoadContent(const std::wstring& assetFile)
{
	auto pBinReader = new BinaryReader();
	pBinReader->Open(assetFile);

	if (!pBinReader->Exists())
	{
		Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > Failed to read the assetFile!\nPath: \'%s\'", assetFile.c_str());
		return nullptr;
	}

	//See BMFont Documentation for Binary Layout

	// Parse the id verification
	char id[3]{};
	for (size_t i{}; i < 3; ++i)
	{
		if (i >= 3) continue; // Unnessecary, but whatever

		id[i] = pBinReader->Read<char>();
	}

	if (id[0] != 'B' || id[1] != 'M' || id[2] != 'F')
	{
		Logger::Log(LogLevel::Error, L"SpriteFontLoader::LoadContent > Not a valid .fnt font");
		return nullptr;
	}

	//Parse the version (version 3 required)
	unsigned char version{};
	version = pBinReader->Read<char>();
	if (version < 3)
	{
		Logger::Log(LogLevel::Error, L"SpriteFontLoader::LoadContent > Only .fnt version 3 is supported");
		return nullptr;
	}

	//Valid .fnt file
	auto pSpriteFont = new SpriteFont();
	//SpriteFontLoader is a friend class of SpriteFont
	//That means you have access to its privates (pSpriteFont->m_FontName = ... is valid)

	//**********
	// BLOCK 0 *
	//**********
	//Retrieve the blockId and blockSize
	char blockID0 = pBinReader->Read<char>();
	int blockSize0 = pBinReader->Read<int>();
	UNREFERENCED_PARAMETER(blockID0);
	UNREFERENCED_PARAMETER(blockSize0);

	//Retrieve the FontSize (will be -25, BMF bug) [SpriteFont::m_FontSize]
	short fontSize{};
	fontSize = pBinReader->Read<short>();

	pSpriteFont->m_FontSize = fontSize;

	//Move the binreader to the start of the FontName [BinaryReader::MoveBufferPosition(...) or you can set its position using BinaryReader::SetBufferPosition(...))
	pBinReader->MoveBufferPosition(12);

	//Retrieve the FontName [SpriteFont::m_FontName]
	char stringCharacter{'a'};
	std::wstring fontName;
	stringCharacter;
	while (stringCharacter != '\0')
	{
		stringCharacter = pBinReader->Read<char>();
		fontName += (wchar_t)stringCharacter;
	}

	pSpriteFont->m_FontName = fontName;

	//**********
	// BLOCK 1 *
	//**********
	//Retrieve the blockId and blockSize
	char blockID1 = pBinReader->Read<char>();
	int blockSize1 = pBinReader->Read<int>();
	UNREFERENCED_PARAMETER(blockID1);
	UNREFERENCED_PARAMETER(blockSize1);

	pBinReader->MoveBufferPosition(4);
	//Retrieve Texture Width & Height [SpriteFont::m_TextureWidth/m_TextureHeight]
	short texWidth = pBinReader->Read<short>();
	short texHeight = pBinReader->Read<short>();
	pSpriteFont->m_TextureWidth = texWidth;
	pSpriteFont->m_TextureHeight = texHeight;

	//Retrieve PageCount
	short pageCount = pBinReader->Read<short>();

	if (pageCount > 1) Logger::Log(LogLevel::Warning, L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Only one texture per font allowed ");

	// Go to block 2:
	pBinReader->MoveBufferPosition(5);
	//**********
	// BLOCK 2 *
	//**********
	//Retrieve the blockId and blockSize
	char blockID2 = pBinReader->Read<char>();
	int blockSize2 = pBinReader->Read<int>();
	UNREFERENCED_PARAMETER(blockID2);
	UNREFERENCED_PARAMETER(blockSize2);

	//Retrieve the PageName (store Local)
	std::wstring pageName;
	char character{ 'a' };
	character;

	while (character != '\0')
	{
		character = pBinReader->Read<char>();
		pageName += (wchar_t)character;
	}

	if (pageName.empty())
	{
		Logger::Log(LogLevel::Error, L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid Font Sprite [Empty]");
	}
	
	//>Retrieve texture filepath from the assetFile path 
	//> (ex. c:/Example/somefont.fnt => c:/Example/) [Have a look at: wstring::rfind()]
	std::wstring path = assetFile.substr(0, assetFile.rfind(L'/') + 1);

	//>Use path and PageName to load the texture using the ContentManager [SpriteFont::m_pTexture]
	//> (ex. c:/Example/ + 'PageName' => c:/Example/somefont_0.png)
	std::wstring finalString = path + pageName;
	pSpriteFont->m_pTexture = ContentManager::Load<TextureData>(finalString);

	//**********
	// BLOCK 3 *
	//**********
	//Retrieve the blockId and blockSize
	char blockID3 = pBinReader->Read<char>();
	int blockSize3 = pBinReader->Read<int>();
	UNREFERENCED_PARAMETER(blockID3);
	UNREFERENCED_PARAMETER(blockSize3);

	//Retrieve Character Count (see documentation)
	uint32_t charCount = blockSize3 / 20; // 20 == the size of the charInfo

	//Retrieve Every Character, For every Character:
	for (size_t i{}; i < charCount; ++i)
	{
		wchar_t ID = (wchar_t)pBinReader->Read<int>();

		if (!pSpriteFont->IsCharValid(ID))
		{
			Logger::Log(LogLevel::Warning, L"SpriteFontLoader::LoadContent > Invalid Character-ID!");
			continue;
		}

		FontMetric& metric = pSpriteFont->GetMetric(ID);

		metric.IsValid = true;
		metric.Character = ID;

		short xPos = pBinReader->Read<short>();
		short yPos = pBinReader->Read<short>();

		short width = pBinReader->Read<short>();
		short height = pBinReader->Read<short>();

		metric.Width = width;
		metric.Height = height;

		short xOffset = pBinReader->Read<short>();
		short yOffset = pBinReader->Read<short>();

		metric.OffsetX = xOffset;
		metric.OffsetY = yOffset;

		short xAdvance = pBinReader->Read<short>();
		metric.AdvanceX = xAdvance;
		
		char page = pBinReader->Read<char>();
		metric.Page = page;

		char channel = pBinReader->Read<char>();
		if (channel & 0b0001) metric.Channel = 2;
		else if (channel & 0b0010) metric.Channel = 1;
		else if (channel & 0b0100) metric.Channel = 0;
		else if (channel & 0b1000) metric.Channel = 3;

		metric.TexCoord = CalcTexCoord(xPos, yPos, texWidth, texHeight);
	}

	//> Retrieve Channel (BITFIELD!!!) 
	//	> See documentation for BitField meaning [FontMetrix::Channel]

	//DONE :)

	delete pBinReader;
	return pSpriteFont;
}

void SpriteFontLoader::Destroy(SpriteFont* objToDestroy)
{
	SafeDelete(objToDestroy);
}

DirectX::XMFLOAT2 SpriteFontLoader::CalcTexCoord(const short posX, const short posY, const short texWidth, const short texHeight) const
{
	DirectX::XMFLOAT2 result{};

	result.x = (float)posX / (float)texWidth;
	result.y = (float)posY / (float)texHeight;

	return result;
}
