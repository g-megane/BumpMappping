// �R���X�^���g�o�b�t�@
cbuffer ConstantBuffer : register(b0)
{
    matrix World;          // ���[���h�s��
    matrix View;           // �r���[�s��
    matrix Projection;     // �ˉe�s��
    float4 vLightDir[2];   // ���C�g�̍��W
    float4 vLightColor[2]; // ���C�g�̐F
    float4 vOutputColor;   // ���C�g�ȊO�̃��f���̐F
}

struct PS_INPUT
{
    float4 Pos : SV_POSITION; // ���݂̃s�N�Z���ʒu
    float3 Norm : TEXCOORD0;  // �@��
};

// �s�N�Z���V�F�[�_�[(���C�e�B���O)
float4 PSLight(PS_INPUT input) : SV_TARGET
{
    float4 finalColor = 0;

    // 2�����̃��C�g���v�Z
    for (int i = 0; i < 2; ++i) {
        // ���C�g�x�N�g���Ɩ@���x�N�g���̓���(dot)���v�Z
        // ���߂����ς̒l�łŌ��̓�������-1 �` 1�͈̔͂ŋ��܂�(saturate��0 �` �P�ɗ}��)
        // ���߂����ςƃ��C�g����ŏI�I�Ȗʂ̒l���Z�o
        finalColor += saturate(dot((float3)vLightDir[i], input.Norm) * vLightColor[i]);
    }
    finalColor.a = 1;

	return finalColor;
}