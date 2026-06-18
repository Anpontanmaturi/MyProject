#pragma once
#include <d3d11.h>
HRESULT LoadTextureFromFile(
    ID3D11Device* device,const wchar_t* filename,
    ID3D11ShaderResourceView** shader_resource_view,
    D3D11_TEXTURE2D_DESC* texture2d_desc);
void ReleaseAllTextures();

HRESULT MakeDummyTexture(
    ID3D11Device* device,
    ID3D11ShaderResourceView** shader_resource_view,
    DWORD value/*0xAABBGGRR*/, UINT dimension);
