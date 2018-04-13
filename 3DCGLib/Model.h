#pragma once
#ifndef MODEL_H
#define MODEL_H
#include "DirectX11.h"
#include "Matrix.h"
#include "Lib\WICTextureLoader.h"

namespace Lib
{
    using namespace Microsoft::WRL;

    class Model
    {
    public:
        Model();
        ~Model();

        void render(Color &color);

        void setWorldMatrix(Matrix &_world);
        Matrix getWorldMatrix() const;
        
    private:
        HRESULT init();
        ComPtr<ID3DBlob> shaderCompile(WCHAR* filename, LPCSTR entryPoint, LPCSTR shaderModel);

        struct SimpleVertex
        {
            float pos[3];
            float texture[2];
        };

        struct ConstantBuffer
        {
            Matrix world;
            Matrix view;
            Matrix projection;
        };

        ComPtr<ID3D11VertexShader>       vertexShader;
        ComPtr<ID3D11PixelShader>        psLight;
        ComPtr<ID3D11InputLayout>        vertexLayout;
        ComPtr<ID3D11Buffer>             vertexBuffer;
        ComPtr<ID3D11Buffer>             indexBuffer;
        ComPtr<ID3D11Buffer>             constantBuffer;
        ComPtr<ID3D11Resource>           texture;
        ComPtr<ID3D11Resource>           textureN;
        ComPtr<ID3D11ShaderResourceView> shaderResourceView;
        ComPtr<ID3D11ShaderResourceView> shaderResourceViewN;
        ComPtr<ID3D11SamplerState>       samplerState;
         
        Matrix world;
    };
}
#endif
