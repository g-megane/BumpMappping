#include <d3dcompiler.h>
#include "DirectX11.h"

#pragma comment(lib, "d3dcompiler.lib")

namespace Lib
{
    // �R���X�g���N�^
    DirectX11::DirectX11()
    {
        device           = nullptr;
        deviceContext    = nullptr;
        swapChain        = nullptr;
        renderTargetView = nullptr;
        depthStencil     = nullptr;
        depthStencilView = nullptr;

    }

    // �f�X�g���N�^
    DirectX11::~DirectX11()
    {
    }

    // �t���[���̊J�n
    void DirectX11::begineFrame() const
    {
        // ��ʂ̃N���A
        float ClearColor[4]{ 0.0f, 0.125f, 0.3f, 1.0f };
        deviceContext->ClearRenderTargetView(renderTargetView.Get(), ClearColor);
        // Z�o�b�t�@�[�̃N���A
        deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    }

    // �t���[���̏I��
    void DirectX11::endFrame() const
    {
        swapChain->Present(0, 0);
    }

    // �f�o�C�X�̎擾
    ComPtr<ID3D11Device> DirectX11::getDevice()
    {
        return device;
    }

    // �f�o�C�X�R���e�L�X�g�̎擾
    ComPtr<ID3D11DeviceContext> DirectX11::getDeviceContext()
    {
        return deviceContext;
    }

    // �r���[�s���ݒ�
    void DirectX11::setViewMatrix(const Matrix & _view)
    {
        view = _view;
    }

    // �r���[�s����擾
    Matrix & DirectX11::getViewMatrix()
    {
        return view;
    }

    // �ˉe�s���ݒ�
    void DirectX11::setProjectionMatrix(const Matrix & _projection)
    {
        projection = _projection;
    }

    // �ˉe�s����擾
    Matrix & DirectX11::getProjectionMatrix()
    {
        return projection;
    }

    // ������
    HRESULT DirectX11::initDevice(std::shared_ptr<Window> _window)
    {
        window = _window;
        UINT createDeviceFlags = 0;
#ifdef _DEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        // �h���C�o�[�̎�ނ̃I�v�V����
        D3D_DRIVER_TYPE driverTypes[] =
        {
            D3D_DRIVER_TYPE_HARDWARE,
            D3D_DRIVER_TYPE_WARP,
            D3D_DRIVER_TYPE_REFERENCE,
        };
        UINT numDriverTypes = ARRAYSIZE(driverTypes);

        // Direct3D�f�o�C�X�̃^�[�Q�b�g�ƂȂ�@�\�Z�b�g
        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1,
        };
        UINT numFeatureLevels = ARRAYSIZE(featureLevels);

        UINT windowWidth = window->getWindowRect().right - window->getWindowRect().left;
        UINT windowHeight = window->getWindowRect().bottom - window->getWindowRect().top;

        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = 1;                                // �X���b�v�`�F�[���̃o�b�t�@�[��
        sd.BufferDesc.Width = windowWidth;                 // �𑜓x�̕�
        sd.BufferDesc.Height = windowHeight;               // �𑜓x�̍���
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // �\���t�H�[�}�b�g
        sd.BufferDesc.RefreshRate.Numerator = 60;          // �L�����̍ő�l
        sd.BufferDesc.RefreshRate.Denominator = 1;         // �L�����̍ŏ��l
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // �o�b�N�o�b�t�@�[�̃T�[�t�F�X�g�p�@�����CPU�A�N�Z�X�I�v�V����
        sd.OutputWindow = window->getHWND();               // �o�̓E�B���h�E�ւ�HWND�n���h��
        sd.SampleDesc.Count = 1;                           // �s�N�Z���P�ʂ̃}���`�T���v�����O�̐�
        sd.SampleDesc.Quality = 0;                         // �C���[�W�̕i�����x��
        sd.Windowed = TRUE;                                // �o�͂��E�B���h�E���[�h�̏ꍇ��TRUE����ȊO��FALSE

        HRESULT hr = S_OK;
        for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
        {
            driverType = driverTypes[driverTypeIndex];
            hr = D3D11CreateDeviceAndSwapChain(
                nullptr,                               // �g�p����r�f�I�A�_�v�^�[�ւ̃|�C���^�[�i����̂��̂��g���ꍇNULL�j
                driverType,                            // �g�p����f�o�C�X�̎��
                nullptr,                               // �\�t�g�E�F�A���X�^���C�U�[����������DLL�̃n���h��
                createDeviceFlags,                     // �L���ɂ��郉���^�C�����C���[
                featureLevels,                         // D3D_FEATURE_LEVEL�̔z��ւ̃|�C���^�[
                numFeatureLevels,                      // ��L�z��̗v�f��
                D3D11_SDK_VERSION,                     // SDK�̃o�[�W����
                &sd,                                   // DXGI_SWAP_CHAIN�ւ̃|�C���^�[
                swapChain.GetAddressOf(),              // IDXGISwapChain�I�u�W�F�N�g�ւ̃|�C���^�[�̃A�h���X��Ԃ�
                device.GetAddressOf(),                 // ID3D11Device�I�u�W�F�N�g�ւ̃|�C���^�[�̃A�h���X��Ԃ�
                &featureLevel,	                       // D3D_FEATURE_LEVEL�ւ̃|�C���^�[��Ԃ�
                deviceContext.GetAddressOf()           // ID3D11DeviceContext�I�u�W�F�N�g�ւ̃|�C���^�[�̃A�h���X��Ԃ�
            );

            if (SUCCEEDED(hr)) {
                break;
            }
        }
        if (FAILED(hr)) {
            MessageBox(nullptr, L"D3DCreateDeviceAndSwapChain()�̎��s : " + hr, L"Error", MB_OK);
            return hr;
        }

        Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
        // �o�b�N�o�b�t�@�̎擾
        hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(backBuffer.GetAddressOf()));
        if (FAILED(hr)) {
            MessageBox(nullptr, L"GetBuffer()�̎��s : " + hr, nullptr, MB_OK);
            return hr;
        }
        // �����_�[�^�[�Q�b�g�r���[�̍쐬
        hr = device->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTargetView.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"CreateRenderTargetView()�̎��s : " + hr, nullptr, MB_OK);
            return hr;
        }

        // ���[�x�X�e���V����Z�o�b�t�@�[�Ƃ��Ă΂��
        // �[�x�X�e���V���p�e�N�X�`�����\�[�X���쐬
        D3D11_TEXTURE2D_DESC descDepth;
        ZeroMemory(&descDepth, sizeof(descDepth));
        descDepth.Width = windowWidth;                    // �e�N�X�`���[�̕�(�o�b�N�o�b�t�@�Ɠ����T�C�Y���w��)
        descDepth.Height = windowHeight;                  // �e�N�X�`���[�̍���(�o�b�N�o�b�t�@�Ɠ����T�C�Y���w��)
        descDepth.MipLevels = 1;                          // �~�b�v�}�b�v���x���̍ő吔
        descDepth.ArraySize = 1;                          // �e�N�X�`���[�z����̃e�N�X�`���[�̐�
        descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // �e�N�X�`���[�̃t�H�[�}�b�g
        descDepth.SampleDesc.Count = 1;                   // �s�N�Z���P�ʂ̃}���`�T���v�����O��
        descDepth.SampleDesc.Quality = 0;                 // �C���[�W�̕i�����x��
        descDepth.Usage = D3D11_USAGE_DEFAULT;            // �e�N�X�`���[�̓ǂݍ��݂���я������ݕ��@�����ʂ���l
        descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;   // �p�C�v���C���X�e�[�W�ւ̃o�C���h�Ɋւ���t���O
        descDepth.CPUAccessFlags = 0;                     // ������CPU�A�N�Z�X�̎�ނ��w�蔄��t���O
        descDepth.MiscFlags = 0;                          // ���̈�ʐ��̒Ⴂ���\�[�X�I�v�V���������ʂ���t���O
        hr = device->CreateTexture2D(&descDepth, nullptr, depthStencil.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"CreateTexture2D()�̎��s : " + hr, L"Error", MB_OK);
            return hr;
        }

        // �[�x�X�e���V���r���[����A�N�Z�X�\�ȃe�N�X�`���[�̃T�u���\�[�X���w��
        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
        ZeroMemory(&descDSV, sizeof(descDSV));
        descDSV.Format = descDepth.Format;                     // ���\�[�X�f�[�^�̃t�H�[�}�b�g
        descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; // ���\�[�X�̃^�C�v
        descDSV.Texture2D.MipSlice = 0;                        // �ŏ��Ɏg�p����~�b�v�}�b�v���x���̃C���f�b�N�X
        // ���\�[�X�f�[�^�ւ̃A�N�Z�X�p�ɐ[�x�X�e���V���r���[�̍쐬
        hr = device->CreateDepthStencilView(depthStencil.Get(), &descDSV, depthStencilView.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"CreateDepthStencilView()�̎��s : " + hr, L"Error", MB_OK);
            return hr;
        }
        // �[�x�X�e���V���r���[���^�[�Q�b�g�ɃZ�b�g
        deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

        // Viewport�̏�����
        D3D11_VIEWPORT vp;
        vp.Width    = static_cast<FLOAT>(windowWidth);  // �r���[�|�[�g������x�ʒu
        vp.Height   = static_cast<FLOAT>(windowHeight); // �r���[�|�[�g�㑤��y�ʒu
        vp.MinDepth = 0.0f;                             // �r���[�|�[�g��
        vp.MaxDepth = 1.0f;                             // �r���[�|�[�g�̍���
        vp.TopLeftX = 0;                                // �r���[�|�[�g�̍ŏ��[�x(0�`1)
        vp.TopLeftY = 0;                                // �r���[�|�[�g�̍ő�[�x(0�`1)
        deviceContext->RSSetViewports(1, &vp);

        return S_OK;
    }
}
