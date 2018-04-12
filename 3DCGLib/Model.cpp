#include <d3dcompiler.h>
#include "Model.h"

namespace Lib
{
    // コンストラクタ
    Model::Model()
    {
        world = Matrix::Identify;
        init();
    }

    // デストラクタ
    Model::~Model()
    {
    }

    // モデルの描画
    void Model::render(Color &color)
    {
        auto &directX = DirectX11::getInstance();
        
        // ----- モデルの描画 -----

        // コンスタントバッファの更新
        ConstantBuffer cb;
        cb.world      = Matrix::transpose(world);
        cb.view       = Matrix::transpose(directX.getViewMatrix());
        cb.projection = Matrix::transpose(directX.getProjectionMatrix());
        directX.getDeviceContext()->UpdateSubresource(constantBuffer.Get(), 0, nullptr, &cb, 0, 0);

        // シェーダーの更新
        directX.getDeviceContext()->VSSetShader(vertexShader.Get(), nullptr, 0);
        directX.getDeviceContext()->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
        directX.getDeviceContext()->PSSetShader(psLight.Get(), nullptr, 0);
        directX.getDeviceContext()->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());
        directX.getDeviceContext()->PSSetShaderResources(1, 1, shaderResourceViewN.GetAddressOf());
        directX.getDeviceContext()->PSSetSamplers(0, 1, samplerState.GetAddressOf());

        // 描画
        directX.getDeviceContext()->DrawIndexed(36, 0, 0);

    }

    // ワールド行列を設定
    void Model::setWorldMatrix(Matrix & _world)
    {
        world = _world;
    }

    // ワールド行列を取得
    Matrix Model::getWorldMatrix() const
    {
        return world;
    }

    // 初期化
    HRESULT Model::init()
    {
        auto &directX = DirectX11::getInstance();
        auto hr = S_OK;

        // VertexShaderの読み込み
        auto VSBlob = shaderCompile(L"VertexShader.hlsl", "VS", "vs_4_0");
        if (VSBlob == nullptr) {
            MessageBox(nullptr, L"shaderCompile()の失敗(VS)", L"Error", MB_OK);
            return hr;
        }

        // VertexShaderの作成
        hr = directX.getDevice()->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), nullptr, vertexShader.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"VSコンパイル失敗", L"Error", MB_OK);
            return hr;
        }

        // InputLayouの定義
        D3D11_INPUT_ELEMENT_DESC layout[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            {   "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        UINT numElements = ARRAYSIZE(layout);

        // InputLayoutの作成
        hr = directX.getDevice()->CreateInputLayout(layout, numElements, VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), vertexLayout.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(NULL, L"CreateInputLayoutの失敗(VS)", L"Error", MB_OK);
            return hr;
        }

        // InputLayoutをセット
        directX.getDeviceContext()->IASetInputLayout(vertexLayout.Get());
        
        // PixelShaderの読み込み
        auto PSBlobLight = shaderCompile(L"PixelShader.hlsl", "PS", "ps_4_0");
        if (PSBlobLight == nullptr) {
            MessageBox(nullptr, L"shaderCompile()の失敗(PS)", L"Error", MB_OK);
            return hr;
        }

        // PixelShaderの作成
        hr = directX.getDevice()->CreatePixelShader(PSBlobLight->GetBufferPointer(), PSBlobLight->GetBufferSize(), nullptr, psLight.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"createPixelShader()の失敗", L"Error", MB_OK);
            return hr;
        }

        // VertexBufferの定義
        SimpleVertex vertices[] =
        {
            { { -1.0f,  1.0f, -1.0f }, { 0.0f, 0.0f }, {  0.0f,  1.0f,  0.0f}},
            { {  1.0f,  1.0f, -1.0f }, { 1.0f, 0.0f }, {  0.0f,  1.0f,  0.0f}},
            { {  1.0f,  1.0f,  1.0f }, { 1.0f, 1.0f }, {  0.0f,  1.0f,  0.0f}},
            { { -1.0f,  1.0f,  1.0f }, { 0.0f, 1.0f }, {  0.0f,  1.0f,  0.0f}},
                                                             
            { { -1.0f, -1.0f, -1.0f }, { 0.0f, 0.0f }, {  0.0f, -1.0f,  0.0f}},
            { {  1.0f, -1.0f, -1.0f }, { 1.0f, 0.0f }, {  0.0f, -1.0f,  0.0f}},
            { {  1.0f, -1.0f,  1.0f }, { 1.0f, 1.0f }, {  0.0f, -1.0f,  0.0f}},
            { { -1.0f, -1.0f,  1.0f }, { 0.0f, 1.0f }, {  0.0f, -1.0f,  0.0f}},
                                                            
            { { -1.0f, -1.0f,  1.0f }, { 0.0f, 0.0f }, { -1.0f,  0.0f,  0.0f}},
            { { -1.0f, -1.0f, -1.0f }, { 1.0f, 0.0f }, { -1.0f,  0.0f,  0.0f}},
            { { -1.0f,  1.0f, -1.0f }, { 1.0f, 1.0f }, { -1.0f,  0.0f,  0.0f}},
            { { -1.0f,  1.0f,  1.0f }, { 0.0f, 1.0f }, { -1.0f,  0.0f,  0.0f}},
                                                             
            { {  1.0f, -1.0f,  1.0f }, { 0.0f, 0.0f }, {  1.0f,  0.0f,  0.0f}},
            { {  1.0f, -1.0f, -1.0f }, { 1.0f, 0.0f }, {  1.0f,  0.0f,  0.0f}},
            { {  1.0f,  1.0f, -1.0f }, { 1.0f, 1.0f }, {  1.0f,  0.0f,  0.0f}},
            { {  1.0f,  1.0f,  1.0f }, { 0.0f, 1.0f }, {  1.0f,  0.0f,  0.0f}},
                                                             
            { { -1.0f, -1.0f, -1.0f }, { 0.0f, 0.0f }, {  0.0f,  0.0f, -1.0f}},
            { {  1.0f, -1.0f, -1.0f }, { 1.0f, 0.0f }, {  0.0f,  0.0f, -1.0f}},
            { {  1.0f,  1.0f, -1.0f }, { 1.0f, 1.0f }, {  0.0f,  0.0f, -1.0f}},
            { { -1.0f,  1.0f, -1.0f }, { 0.0f, 1.0f }, {  0.0f,  0.0f, -1.0f}},
                                                             
            { { -1.0f, -1.0f,  1.0f }, { 0.0f, 0.0f }, {  0.0f,  0.0f,  1.0f}},
            { {  1.0f, -1.0f,  1.0f }, { 1.0f, 0.0f }, {  0.0f,  0.0f,  1.0f}},
            { {  1.0f,  1.0f,  1.0f }, { 1.0f, 1.0f }, {  0.0f,  0.0f,  1.0f}},
            { { -1.0f,  1.0f,  1.0f }, { 0.0f, 1.0f }, {  0.0f,  0.0f,  1.0f}},
        };

        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(SimpleVertex) * 24;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA initData;
        ZeroMemory(&initData, sizeof(initData));
        initData.pSysMem = vertices;
        hr = directX.getDevice()->CreateBuffer(&bd, &initData, vertexBuffer.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"createBuffer()の失敗", L"Error", MB_OK);
            return hr;
        }

        // VertexBufferをセット
        UINT stride = sizeof(SimpleVertex);
        UINT offset = 0;
        directX.getDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

        // 頂点バッファの作成
        WORD indices[] =
        {
             3,  1,  0,
             2,  1,  3,

             6,  4,  5,
             7,  4,  6,
             
            11,  9,  8,
            10,  9, 11,

            14, 12, 13,
            15, 12, 14,

            19, 17, 16,
            18, 17, 19,

            22, 20, 21,
            23, 20, 22
        };
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(WORD) * 36; // 36頂点、12三角形
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;
        initData.pSysMem = indices;
        hr = directX.getDevice()->CreateBuffer(&bd, &initData, indexBuffer.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"createBuffer()の失敗", L"Error", MB_OK);
            return hr;
        }

        // 頂点バッファをセット
        directX.getDeviceContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

        // PrimitiveTopologyをセット
        directX.getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // ConstantBufferの作成
        bd.Usage          = D3D11_USAGE_DEFAULT;
        bd.ByteWidth      = sizeof(ConstantBuffer);
        bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        hr = directX.getDevice()->CreateBuffer(&bd, nullptr, constantBuffer.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"createBuffer()の失敗", L"Error", MB_OK);
            return hr;
        }

        // テクスチャの読み込み
        hr = DirectX::CreateWICTextureFromFile(directX.getDevice().Get(), L"Texture/Tex.png", texture.GetAddressOf(), shaderResourceView.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"CreateWICTextureFromFile()の失敗", L"Error", MB_OK);
            return hr;
        }

        hr = DirectX::CreateWICTextureFromFile(directX.getDevice().Get(), L"Texture/Tex_N.png", textureN.GetAddressOf(), shaderResourceViewN.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"CreateWICTextureFromFile()の失敗", L"Error", MB_OK);
            return hr;
        }

        // サンプラーの作成
        D3D11_SAMPLER_DESC smpDesc;
        ZeroMemory(&smpDesc, sizeof(D3D11_SAMPLER_DESC));
        smpDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // テクスチャーのサンプリング時に使用するフィルタリングメソッド
        smpDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;    // ０～１の範囲外にあるuテクスチャー座標を解決するために使用されるメソッド
        smpDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;    // ０～１の範囲外にあるvテクスチャー座標を解決するために使用されるメソッド
        smpDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;    // ０～１の範囲外にあるwテクスチャー座標を解決するために使用されるメソッド
        smpDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;  // 既存のサンプリングデータに対してデータを比較する関数
        smpDesc.MinLOD = 0;                               // アクセスをクランプするミップマップ範囲の下限
        smpDesc.MaxLOD = D3D11_FLOAT32_MAX;               // アクセスをクランプするミップマップ
        hr = directX.getDevice()->CreateSamplerState(&smpDesc, samplerState.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"CreateSamplerState()の失敗", L"Error", MB_OK);
            return hr;
        }

        return S_OK;
    }

    // シェーダーの読み込み
    Microsoft::WRL::ComPtr<ID3DBlob> Model::shaderCompile(WCHAR * filename, LPCSTR entryPoint, LPCSTR shaderModel)
    {
        Microsoft::WRL::ComPtr<ID3DBlob> blobOut = nullptr;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

        DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
        shaderFlags |= D3DCOMPILE_DEBUG;
#endif

        auto hr = D3DCompileFromFile(
            filename,
            nullptr,
            nullptr,
            entryPoint,
            shaderModel,
            shaderFlags,
            0,
            blobOut.GetAddressOf(),
            errorBlob.GetAddressOf()
        );

        if (FAILED(hr)) {
            if (errorBlob != nullptr) {
                MessageBox(nullptr, static_cast<LPWSTR>(errorBlob->GetBufferPointer()), nullptr, MB_OK);
            }
            if (errorBlob) {
                errorBlob.Get()->Release();
            }
        }
        if (errorBlob) {
            errorBlob.Get()->Release();
        }

        return Microsoft::WRL::ComPtr<ID3DBlob>(blobOut);
    }
}
