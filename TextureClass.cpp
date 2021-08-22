#include "stdafx.h"
#include "TextureClass.h"


TextureClass::TextureClass()
{
}


TextureClass::TextureClass(const TextureClass& other)
{
}


TextureClass::~TextureClass()
{
}


bool TextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	int width = 0;
	int height = 0;

	// targa �̹��� �����͸� �޸𸮿� �ε��մϴ�.
	if (!LoadTarga(filename, height, width))
	{
		return false;
	}

	//�ؽ�ó�� ����ü�� �����մϴ�.
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// �� �ؽ�ó�� �����մϴ�.
	HRESULT hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(hResult))
	{
		return false;
	}

	//  targa �̹��� �������� �ʺ� ����� �����մϴ�.
	UINT rowPitch = (width * 4) * sizeof(unsigned char);

	// targa �̹��� �����͸� �ؽ�ó�� �����մϴ�.
	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	// ���̴� ���ҽ� �� ����ü�� �����մϴ�.
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// �ؽ�ó�� ���̴� ���ҽ� �並 ����ϴ�.
	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
	{
		return false;
	}

	// �� �ؽ�ó�� ���� �Ӹ��� �����մϴ�.
	deviceContext->GenerateMips(m_textureView);

	// �̹��� �����Ͱ� �ؽ�ó�� �ε� �Ǿ����Ƿ� targa �̹��� �����͸� �����մϴ�.
	delete[] m_targaData;
	m_targaData = 0;

	return true;
}


void TextureClass::Shutdown()
{
	//�ؽ�ó �� ���ҽ��� �����Ѵ�.
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = 0;
	}

	// �ؽ��ĸ� �����մϴ�.
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	// targa �̹��� �����͸� �����մϴ�.
	if (m_targaData)
	{
		delete[] m_targaData;
		m_targaData = 0;
	}
}


ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_textureView;
}


bool TextureClass::LoadTarga(char* filename, int& height, int& width)
{
	// targa ������ ���̳ʸ� ���� ������ ���ϴ�.
	FILE* filePtr;
	if (fopen_s(&filePtr, filename, "rb") != 0)
	{
		return false;
	}

	// ���� ����� �о�ɴϴ�.
	TargaHeader targaFileHeader;
	unsigned int count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// ����������� �߿� ������ ���ɴϴ�.
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	int bpp = (int)targaFileHeader.bpp;

	// ������ 32bit ���� 24bit���� üũ�մϴ�.
	if (bpp != 32)
	{
		return false;
	}

	// 32 ��Ʈ �̹��� �������� ũ�⸦ ����մϴ�.
	int imageSize = width * height * 4;

	//  targa �̹��� ������ �� �޸𸮸� �Ҵ��մϴ�.
	unsigned char* targaImage = new unsigned char[imageSize];
	if (!targaImage)
	{
		return false;
	}

	// targa �̹��� �����͸� �н��ϴ�.
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// ������ �ݽ��ϴ�.
	if (fclose(filePtr) != 0)
	{
		return false;
	}

	// targa ��� �����Ϳ� ���� �޸𸮸� �Ҵ��մϴ�.
	m_targaData = new unsigned char[imageSize];
	if (!m_targaData)
	{
		return false;
	}

	// targa ��� ������ �迭�� �ε����� �ʱ�ȭ�մϴ�.
	int index = 0;

	// targa �̹��� �����Ϳ� �ε����� �ʱ�ȭ�մϴ�.
	int k = (width * height * 4) - (width * 4);

	// ���� targa ������ �Ųٷ� ����Ǿ����Ƿ� �ùٸ� ������ targa �̹��� �����͸� targa ��� �迭�� �����մϴ�.
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			m_targaData[index + 0] = targaImage[k + 2];  // ����
			m_targaData[index + 1] = targaImage[k + 1];  // ���
			m_targaData[index + 2] = targaImage[k + 0];  // �Ķ�
			m_targaData[index + 3] = targaImage[k + 3];  // ����

														 // �ε����� targa �����ͷ� ������ŵ�ϴ�.
			k += 4;
			index += 4;
		}

		// targa �̹��� ������ �ε����� �������� ���� �� ���� ���� �κп��� ���� ������ �ٽ� �����մϴ�.
		k -= (width * 8);
	}

	// ��� �迭�� ���� �� targa �̹��� �����͸� �����մϴ�.
	delete[] targaImage;
	targaImage = 0;

	return true;
}