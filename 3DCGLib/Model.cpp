#include <d3dcompiler.h>
#include "Model.h"

namespace Lib
{
    // �R���X�g���N�^
    Model::Model()
    {
        world = Matrix::Identify;
        init();
    }

    // �f�X�g���N�^
    Model::~Model()
    {
    }

    // ���f���̕`��
    void Model::render(Color &color)
    {
        auto &directX = DirectX11::getInstance();

        // ���C�g�̈ʒu
        float vLightDires[2][4] = 
        {
            { -0.5f,  0.5f,  0.5f, 1.0f },
            {  0.5f, -0.5f, -0.5f, 1.0f },
        };
        // ���C�g�̐F
        float vLightColors[2][4] = 
        {
            { 1.0f, 1.0f, 1.0f, 1.0f },
            { 0.0f, 1.0f, 0.0f, 1.0f },
        };
        // �������̐F
        float defaultOutputColor[4]
        {
            0.0f, 0.0f, 0.0f, 0.0f 
        };

        // ���C�e�B���O����钆�S�̃��f���̕`��
        ConstantBuffer cb;
        cb.world      = Matrix::transpose(world);
        cb.view       = Matrix::transpose(directX.getViewMatrix());
        cb.projection = Matrix::transpose(directX.getProjectionMatrix());
        memcpy(cb.vLightDire[0], vLightDires[0], sizeof(vLightDires[0]));
        memcpy(cb.vLightDire[1], vLightDires[1], sizeof(vLightDires[1]));
        memcpy(cb.vLightColor[0], vLightColors[0], sizeof(vLightColors[0]));
        memcpy(cb.vLightColor[1], vLightColors[1], sizeof(vLightColors[1]));
        memcpy(cb.vOutputColor, defaultOutputColor, sizeof(defaultOutputColor));
        directX.getDeviceContext()->UpdateSubresource(constantBuffer.Get(), 0, nullptr, &cb, 0, 0);

        directX.getDeviceContext()->VSSetShader(vertexShader.Get(), nullptr, 0);
        directX.getDeviceContext()->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
        directX.getDeviceContext()->PSSetShader(psLight.Get(), nullptr, 0);
        directX.getDeviceContext()->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
        directX.getDeviceContext()->DrawIndexed(36, 0, 0);

        // 2�����Ƀ��C�g�̈ʒu�������I�u�W�F�N�g��z�u
        for (int i = 0; i < 2; ++i) {
            auto mtLight  = Matrix::Identify;
            auto mttLight = Matrix::translate(Vector3(vLightDires[i][0], vLightDires[i][1], vLightDires[i][2]) * 5.0f);
            auto mtsLight = Matrix::scale(0.2f, 0.2f, 0.2f);
            mtLight = mtsLight * mttLight;

            cb.world = Matrix::transpose(mtLight);
            memcpy(cb.vOutputColor, vLightColors[i], sizeof(vLightColors[i]));
            directX.getDeviceContext()->UpdateSubresource(constantBuffer.Get(), 0, nullptr, &cb, 0, 0);

            directX.getDeviceContext()->PSSetShader(psSolid.Get(), nullptr, 0);
            directX.getDeviceContext()->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
            directX.getDeviceContext()->DrawIndexed(36, 0, 0);
        }
    }

    // ���[���h�s���ݒ�
    void Model::setWorldMatrix(Matrix & _world)
    {
        world = _world;
    }

    // ���[���h�s����擾
    Matrix Model::getWorldMatrix() const
    {
        return world;
    }

    // ������
    HRESULT Model::init()
    {
        auto &directX = DirectX11::getInstance();
        auto hr = S_OK;

        // VertexShader�̓ǂݍ���
        auto VSBlob = shaderCompile(L"VertexShader.hlsl", "VS", "vs_4_0");
        if (VSBlob == nullptr) {
            MessageBox(nullptr, L"shaderCompile()�̎��s(VS)", L"Error", MB_OK);
            return hr;
        }

        // VertexShader�̍쐬
        hr = directX.getDevice()->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), nullptr, vertexShader.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"VS�R���p�C�����s", L"Error", MB_OK);
            return hr;
        }

        // InputLayou�̒�`
        D3D11_INPUT_ELEMENT_DESC layout[] = {
            { "POSITION", 0,    DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            {   "NORMAL", 0,    DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        UINT numElements = ARRAYSIZE(layout);

        // InputLayout�̍쐬
        hr = directX.getDevice()->CreateInputLayout(layout, numElements, VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), vertexLayout.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(NULL, L"CreateInputLayout�̎��s(VS)", L"Error", MB_OK);
            return hr;
        }

        // InputLayout���Z�b�g
        directX.getDeviceContext()->IASetInputLayout(vertexLayout.Get());

        // PixelShader�̓ǂݍ���
        auto PSBlobSolid = shaderCompile(L"PSSolid.hlsl", "PSSolid", "ps_4_0");
        if (PSBlobSolid == nullptr) {
            MessageBox(nullptr, L"shaderCompile()�̎��s(VS)", L"Error", MB_OK);
            return hr;
        }

        // PixelShader�̍쐬
        hr = directX.getDevice()->CreatePixelShader(PSBlobSolid->GetBufferPointer(), PSBlobSolid->GetBufferSize(), nullptr, psSolid.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"createPixelShader()�̎��s", L"Error", MB_OK);
            return hr;
        }

        // PixelShader�̓ǂݍ���
        auto PSBlobLight = shaderCompile(L"PSLight.hlsl", "PSLight", "ps_4_0");
        if (PSBlobLight == nullptr) {
            MessageBox(nullptr, L"shaderCompile()�̎��s(VS)", L"Error", MB_OK);
            return hr;
        }

        // PixelShader�̍쐬
        hr = directX.getDevice()->CreatePixelShader(PSBlobLight->GetBufferPointer(), PSBlobLight->GetBufferSize(), nullptr, psLight.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"createPixelShader()�̎��s", L"Error", MB_OK);
            return hr;
        }

        // VertexBuffer�̒�`
        SimpleVertex vertices[] =
        {
            { { -1.0f,  1.0f, -1.0f }, {  0.0f,  1.0f,  0.0f} },
            { {  1.0f,  1.0f, -1.0f }, {  0.0f,  1.0f,  0.0f} },
            { {  1.0f,  1.0f,  1.0f }, {  0.0f,  1.0f,  0.0f} },
            { { -1.0f,  1.0f,  1.0f }, {  0.0f,  1.0f,  0.0f} },

            { { -1.0f, -1.0f, -1.0f }, {  0.0f, -1.0f,  0.0f} },
            { {  1.0f, -1.0f, -1.0f }, {  0.0f, -1.0f,  0.0f} },
            { {  1.0f, -1.0f,  1.0f }, {  0.0f, -1.0f,  0.0f} },
            { { -1.0f, -1.0f,  1.0f }, {  0.0f, -1.0f,  0.0f} },

            { { -1.0f, -1.0f,  1.0f }, { -1.0f,  0.0f,  0.0f} },
            { { -1.0f, -1.0f, -1.0f }, { -1.0f,  0.0f,  0.0f} },
            { { -1.0f,  1.0f, -1.0f }, { -1.0f,  0.0f,  0.0f} },
            { { -1.0f,  1.0f,  1.0f }, { -1.0f,  0.0f,  0.0f} },

            { {  1.0f, -1.0f,  1.0f }, {  1.0f,  0.0f,  0.0f} },
            { {  1.0f, -1.0f, -1.0f }, {  1.0f,  0.0f,  0.0f} },
            { {  1.0f,  1.0f, -1.0f }, {  1.0f,  0.0f,  0.0f} },
            { {  1.0f,  1.0f,  1.0f }, {  1.0f,  0.0f,  0.0f} },

            { { -1.0f, -1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f} },
            { {  1.0f, -1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f} },
            { {  1.0f,  1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f} },
            { { -1.0f,  1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f} },

            { { -1.0f, -1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f} },
            { {  1.0f, -1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f} },
            { {  1.0f,  1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f} },
            { { -1.0f,  1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f} },
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
            MessageBox(nullptr, L"createBuffer()�̎��s", L"Error", MB_OK);
            return hr;
        }

        // VertexBuffer���Z�b�g
        UINT stride = sizeof(SimpleVertex);
        UINT offset = 0;
        directX.getDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

        // ���_�o�b�t�@�̍쐬
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
        bd.ByteWidth = sizeof(WORD) * 36; // 36���_�A12�O�p�`
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;
        initData.pSysMem = indices;
        hr = directX.getDevice()->CreateBuffer(&bd, &initData, indexBuffer.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"createBuffer()�̎��s", L"Error", MB_OK);
            return hr;
        }

        // ���_�o�b�t�@���Z�b�g
        directX.getDeviceContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

        // PrimitiveTopology���Z�b�g
        directX.getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // ConstantBuffer�̍쐬
        bd.Usage          = D3D11_USAGE_DEFAULT;
        bd.ByteWidth      = sizeof(ConstantBuffer);
        bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        hr = directX.getDevice()->CreateBuffer(&bd, nullptr, constantBuffer.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"createBuffer()�̎��s", L"Error", MB_OK);
            return hr;
        }

        return S_OK;
    }

    // �V�F�[�_�[�̓ǂݍ���
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
